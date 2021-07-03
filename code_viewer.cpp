/**
 * @file idaplugin/code_viewer.cpp
 * @brief Module contains classes/methods dealing with decompiled code
 *        visualization.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include <regex>

#include "code_viewer.h"
#include "config_generator.h"
#include "idaplugin.h"

namespace idaplugin {

//
//==============================================================================
//

/**
 * @brief Get tagged line on current position.
 * @param v     Control.
 * @param mouse Current for mouse pointer?
 * @param[out] line Current line.
 * @param x     This is horizontal position in line string *WITHOUT* tags.
 * @param y     This is vertical position (line number) in viewer.
 * @param rx    This is horizontal position in line string *WITH* tags.
 * @return False if OK, true otherwise.
 */
static bool get_current_line_with_tags(
		TWidget* v,
		bool mouse,
		std::string& line,
		int& x,
		int& y,
		unsigned& rx)
{
	if (get_custom_viewer_place(v, mouse, &x, &y) == nullptr)
	{
		return true;
	}

	line = get_custom_viewer_curline(v, mouse);

	rx = x;
	for (unsigned i = 0; i <= rx && i<line.size(); ++i)
	{
		unsigned char c = line[i];
		if (c == COLOR_ON || c == COLOR_OFF)
		{
			rx += 2; // {ON,OFF} + COLOR = 1 + 1 = 2
			++i;
		}
		if (c == COLOR_ESC || c == COLOR_INV)
		{
			rx += 1;
		}
	}

	return false;
}

/**
 * @brief Get line without tags on current position.
 * @param v     Control.
 * @param mouse Current for mouse pointer?
 * @param[out] line Current line.
 * @param x     This is horizontal position in line string *WITHOUT* tags.
 * @param y     This is vertical position (line number) in viewer.
 * @return False if OK, true otherwise.
 */
bool get_current_line_without_tags(
		TWidget* v,
		bool mouse,
		std::string &line,
		int& x,
		int& y)
{
	unsigned rx_unused;
	if (get_current_line_with_tags(v, mouse, line, x, y, rx_unused))
	{
		return true;
	}

	qstring buf;
	tag_remove(&buf, line.c_str());
	if (x >= static_cast<int>(buf.length()))
	{
		return true;
	}

	line = buf.c_str();
	return false;
}

/**
 * @brief Get current word
 * @param v Control
 * @param mouse bool mouse (current for mouse pointer?)
 * @param[out] word result
 * @param[out] color resulted word color
 * @return False if OK, true otherwise.
 */
static bool get_current_word(
		TWidget* v,
		bool mouse,
		std::string& word,
		int& color)
{
	// Use SDK function to get highlighted ID.
	//
	qstring buf;
	if (!get_highlight(&buf, v, nullptr))
	{
		return true;
	}

	int x, y;
	unsigned rx;
	std::string taggedLine;
	if (get_current_line_with_tags(v, mouse, taggedLine, x, y, rx))
	{
		return true;
	}

	int prevColor = -1;
	int nextColor = -1;

	auto onColor = taggedLine.find_last_of(COLOR_ON, rx);
	if (onColor != std::string::npos && onColor > 0
			&& taggedLine[onColor-1] == COLOR_ON)
	{
		prevColor = taggedLine[onColor];
	}
	else if (onColor != std::string::npos && (onColor+1) < taggedLine.length())
	{
		prevColor = taggedLine[onColor+1];
	}

	auto offColor = taggedLine.find_first_of(COLOR_OFF, rx);
	if (offColor != std::string::npos && (offColor+1) < taggedLine.length())
	{
		nextColor = taggedLine[offColor+1];
	}

	if (prevColor == -1 || prevColor != nextColor)
	{
		return false;
	}

	word = buf.c_str();
	color = nextColor;
	return false;
}

bool GhidraDec::isWordGlobal(const std::string& word, int color)
{
	return color == COLOR_DEFAULT
			&& decompInfo->configDB.globals.getObjectByNameOrRealName(word)
					!= nullptr;
}

const retdec::config::Object* GhidraDec::getWordGlobal(const std::string& word, int color)
{
	return !word.empty() && color == COLOR_DEFAULT
			? decompInfo->configDB.globals.getObjectByNameOrRealName(word)
			: nullptr;
}

bool GhidraDec::isWordFunction(const std::string& word, int color)
{
	return color == COLOR_DEFAULT
			&& decompInfo->configDB.functions.hasFunction(word);
}

bool isWordIdentifier(const std::string& word, int color)
{
	return color == COLOR_DREF;
}

const retdec::config::Function* GhidraDec::getWordFunction(
		const std::string& word,
		int color)
{
	return !word.empty() && color == COLOR_DEFAULT
			? decompInfo->configDB.functions.getFunctionByName(word)
			: nullptr;
}

func_t* GhidraDec::getIdaFunction(const std::string& word, int color)
{
	if (word.empty())
	{
		return nullptr;
	}
	if (!isWordFunction(word, color))
	{
		return nullptr;
	}

	auto* cfgFnc = decompInfo->configDB.functions.getFunctionByName(word);
	if (cfgFnc == nullptr)
	{
		return nullptr;
	}

	return get_func((ea_t)cfgFnc->getStart());
}

bool GhidraDec::isCurrentFunction(func_t* fnc)
{
	return decompInfo->navigationActual != decompInfo->navigationList.end()
			&& fnc == *decompInfo->navigationActual;
}

func_t* GhidraDec::getCurrentFunction()
{
	return decompInfo->navigationActual != decompInfo->navigationList.end() ?
			*decompInfo->navigationActual :
			nullptr;
}

bool GhidraDec::isWordCurrentParameter(const std::string& word, int color)
{
	if (!isWordIdentifier(word, color))
	{
		return false;
	}

	auto* idaCurrentFnc = getCurrentFunction();
	if (idaCurrentFnc == nullptr)
	{
		return false;
	}
	qstring name;
	get_func_name(&name, idaCurrentFnc->start_ea);

	auto* ccFnc = decompInfo->configDB.functions.getFunctionByName(name.c_str());
	if (ccFnc == nullptr)
	{
		return false;
	}

	for (auto& p : ccFnc->parameters)
	{
		auto realName = p.getRealName();
		if ((!realName.empty() && realName == word) || p.getName() == word)
		{
			return true;
		}
	}

	return false;
}

//
//==============================================================================
//

/**
 * Decompile or just show function.
 * @param cv        Current custom control.
 * @param calledFnc Called function name.
 * @param force     If function to decompile/show is the same as current function,
 *                  decompile/show it again only if this is set to @c true.
 * @param forceDec  Force new decompilation.
 */
void GhidraDec::decompileFunction(
		TWidget* cv,
		const std::string& calledFnc,
		bool force,
		bool forceDec)
{
	auto* globVar = decompInfo->configDB.globals.getObjectByNameOrRealName(
			calledFnc);

	if (globVar && globVar->getStorage().isMemory())
	{
		INFO_MSG("Global variable -> jump to ASM.\n");
		jumpto((ea_t)globVar->getStorage().getAddress() );
		return;
	}

	auto* cfgFnc = decompInfo->configDB.functions.getFunctionByName(calledFnc);

	if (!cfgFnc)
	{
		INFO_MSG("Unknown function to decompile \"" << calledFnc << "\" -> do nothing.\n");
		return;
	}

	if (cfgFnc->isUserDefined())
	{
		for (unsigned i = 0; i < get_func_qty(); ++i)
		{
			func_t *fnc = getn_func(i);

			if (fnc->start_ea != cfgFnc->getStart())
			{
				continue;
			}
			if (!force && isCurrentFunction(fnc))
			{
				INFO_MSG("The current function is not decompiled/shown again.\n");
				return;
			}

			// Decompile found function.
			//
			runSelectiveDecompilation(fnc, forceDec);
			return;
		}
	}

	// Such function exists in config file, but not in IDA functions.
	// This is import/export or something similar -> jump to IDA disasm view.
	//
	INFO_MSG("Not a user-defined function -> jump to ASM.\n");
	jumpto((ea_t)cfgFnc->getStart() );
}

const char* GhidraDec::move_backward_ah_t::actionName = "ghidradec:ActionMoveBackward";
const char* GhidraDec::move_backward_ah_t::actionLabel = "Move backward";
const char* GhidraDec::move_backward_ah_t::actionHotkey = "ESC";

const char* GhidraDec::move_forward_ah_t::actionName = "ghidradec:ActionMoveForward";
const char* GhidraDec::move_forward_ah_t::actionLabel = "Move forward";
const char* GhidraDec::move_forward_ah_t::actionHotkey = "Ctrl+F";

const char* GhidraDec::change_fnc_comment_ah_t::actionName = "ghidradec:ActionChangeFncComment";
const char* GhidraDec::change_fnc_comment_ah_t::actionLabel = "Edit func comment";
const char* GhidraDec::change_fnc_comment_ah_t::actionHotkey = ";";

const char* GhidraDec::change_fnc_global_name_ah_t::actionName = "ghidradec:ActionChangeFncGlobName";
const char* GhidraDec::change_fnc_global_name_ah_t::actionLabel = "Rename";
const char* GhidraDec::change_fnc_global_name_ah_t::actionHotkey = "N";

const char* GhidraDec::open_xrefs_ah_t::actionName = "ghidradec:ActionOpenXrefs";
const char* GhidraDec::open_xrefs_ah_t::actionLabel = "Open xrefs window";
const char* GhidraDec::open_xrefs_ah_t::actionHotkey = "X";

const char* GhidraDec::open_calls_ah_t::actionName = "ghidradec:ActionOpenCalls";
const char* GhidraDec::open_calls_ah_t::actionLabel = "Open calls window";
const char* GhidraDec::open_calls_ah_t::actionHotkey = "C";

const char* GhidraDec::change_fnc_type_ah_t::actionName = "ghidradec:ActionChangeFncType";
const char* GhidraDec::change_fnc_type_ah_t::actionLabel = "Change type declaration";
const char* GhidraDec::change_fnc_type_ah_t::actionHotkey = "Y";

const char* GhidraDec::jump_to_asm_ah_t::actionName = "ghidradec:ActionJumpToAsm";
const char* GhidraDec::jump_to_asm_ah_t::actionLabel = "Jump to ASM";
const char* GhidraDec::jump_to_asm_ah_t::actionHotkey = "A";


//
//==============================================================================
//

bool idaapi GhidraDec::moveToPrevious()
{
	DBG_MSG("\t ESC : [");
	for (auto& fnc : decompInfo->navigationList)
	{
		DBG_MSG(" " << std::hex << fnc->start_ea);
	}
	DBG_MSG(" ] (# " << std::dec << decompInfo->navigationList.size()
			<< ") : from " << std::hex << (*decompInfo->navigationActual)->start_ea
			<< " => BACK\n");

	if (decompInfo->navigationList.size() <= 1)
	{
		return false;
	}

	if (decompInfo->navigationActual != decompInfo->navigationList.begin())
	{
		decompInfo->navigationActual--;

		DBG_MSG("\t\t=> " << std::hex
				<< (*decompInfo->navigationActual)->start_ea << "\n");

		auto fit = decompInfo->fnc2code.find(*decompInfo->navigationActual);
		if (fit == decompInfo->fnc2code.end())
		{
			return false;
		}

		decompInfo->decompiledFunction = fit->first;
		ShowOutput show(decompInfo);
		show.execute();
	}
	else
	{
		DBG_MSG("\t\t=> FIRST : cannot move to the previous\n");
	}

	return false;
}

//
//==============================================================================
//

bool idaapi GhidraDec::moveToNext()
{
	DBG_MSG("\t CTRL + F : [");
	for (auto& fnc : decompInfo->navigationList)
	{
		DBG_MSG(" " << std::hex << fnc->start_ea);
	}
	DBG_MSG(" ] (#" << std::dec << decompInfo->navigationList.size()
			<< ") : from " << std::hex << (*decompInfo->navigationActual)->start_ea
			<< " => FORWARD\n");

	if (decompInfo->navigationList.size() <= 1)
	{
		return false;
	}

	auto last = decompInfo->navigationList.end();
	last--;
	if (decompInfo->navigationActual != last)
	{
		decompInfo->navigationActual++;

		DBG_MSG("\t\t=> " << std::hex
				<< (*decompInfo->navigationActual)->start_ea << "\n");

		auto fit = decompInfo->fnc2code.find(*decompInfo->navigationActual);
		if (fit != decompInfo->fnc2code.end())
		{
			decompInfo->decompiledFunction = fit->first;
			ShowOutput show(decompInfo);
			show.execute();

			return false;
		}
	}
	else
	{
		DBG_MSG("\t\t=> LAST : cannot move to the next\n");
	}

	return false;
}

//
//==============================================================================
//

bool idaapi GhidraDec::insertCurrentFunctionComment()
{
	auto* fnc = getCurrentFunction();
	if (fnc == nullptr)
	{
		return false;
	}

	qstring qCmt;
	get_func_cmt(&qCmt, fnc, false);

	qstring buff;
	if (ask_text(
			&buff,
			MAXSTR,
			qCmt.c_str(),
			"Please enter function comment (max %d characters)",
			MAXSTR))
	{
		set_func_cmt(fnc, buff.c_str(), false);
		decompInfo->decompiledFunction = fnc;
		ShowOutput show(decompInfo);
		show.execute();

	}

	return false;
}

//
//==============================================================================
//

bool idaapi GhidraDec::changeFunctionGlobalName(TWidget* cv)
{
	std::string word;
	int color = -1;
	if (get_current_word(cv, false, word, color))
	{
		return false;
	}

	std::string askString;
	ea_t address;
	const retdec::config::Function* fnc = nullptr;
	const retdec::config::Object* gv = nullptr;
	if ((fnc = getWordFunction(word, color)))
	{
		askString = "Please enter function name";
		address = (ea_t)fnc->getStart();
	}
	else if ((gv = getWordGlobal(word, color)))
	{
		askString = "Please enter global variable name";
		address = (ea_t)gv->getStorage().getAddress();
	}
	else
	{
		return false;
	}

	qstring qNewName = word.c_str();
	if (!ask_str(&qNewName, HIST_IDENT, "%s", askString.c_str())
			|| qNewName.empty())
	{
		return false;
	}
	std::string newName = qNewName.c_str();
	if (newName == word)
	{
		return false;
	}
	auto fit = decompInfo->fnc2code.find(*decompInfo->navigationActual);
	if (fit == decompInfo->fnc2code.end())
	{
		return false;
	}

	std::regex e(std::string(SCOLOR_ON)
			+ "."
			+ newName
			+ SCOLOR_OFF
			+ ".");

	if (decompInfo->configDB.globals.getObjectByNameOrRealName(newName) != nullptr
			|| decompInfo->configDB.functions.hasFunction(newName)
			|| std::regex_search(fit->second.code, e))
	{
		WARNING_GUI("Name \"" << newName << "\" is not unique\n");
		return false;
	}

	if (set_name(address, newName.c_str()) == false)
	{
		return false;
	}

	std::string oldName = std::string(SCOLOR_ON)
			+ SCOLOR_DEFAULT
			+ word
			+ SCOLOR_OFF
			+ SCOLOR_DEFAULT;

	std::string replace = std::string(SCOLOR_ON)
			+ SCOLOR_DEFAULT
			+ newName
			+ SCOLOR_OFF
			+ SCOLOR_DEFAULT;

	for (auto& fncItem : decompInfo->fnc2code)
	{
		auto& code = fncItem.second.code;
		std::string::size_type n = 0;
		while (( n = code.find(oldName, n)) != std::string::npos)
		{
			code.replace(n, oldName.size(), replace);
			n += replace.size();
		}
	}

	// TODO: just setting a new name to function/global would be faster.
	//
	ConfigGenerator jg(*decompInfo);
	decompInfo->dbFile = jg.generate();

	decompInfo->decompiledFunction = fit->first;
	ShowOutput show(decompInfo);
	show.execute();

	return false;
}

//
//==============================================================================
//

bool idaapi GhidraDec::openXrefsWindow(func_t* fnc)
{
	open_xrefs_window(fnc->start_ea);
	return false;
}

//
//==============================================================================
//

bool idaapi GhidraDec::openCallsWindow(func_t* fnc)
{
	open_calls_window(fnc->start_ea);
	return false;
}

//
//==============================================================================
//

bool idaapi GhidraDec::changeTypeDeclaration(TWidget* cv)
{
	std::string word;
	int color = -1;
	if (get_current_word(cv, false, word, color))
	{
		return false;
	}
	auto* idaFnc = getIdaFunction(word, color);
	auto* cFnc = getWordFunction(word, color);
	auto* cGv = getWordGlobal(word, color);

	ea_t addr = 0;
	if (cFnc && idaFnc && isCurrentFunction(idaFnc) && cFnc->getName() != "main")
	{
		addr = (ea_t)cFnc->getStart();
	}
	else if (cGv && cGv->getStorage().isMemory())
	{
		WARNING_MSG("Setting type for global variable is not supported at the moment.\n");
		return false;
	}
	else
	{
		return false;
	}

	qstring buf;
	int flags = PRTYPE_1LINE | PRTYPE_SEMI;
	if (print_type(&buf, addr, flags))
	{
		std::string askString = "Please enter type declaration:";

		qstring qNewDeclr = buf;
		if (!ask_str(&qNewDeclr, HIST_IDENT, "%s", askString.c_str())
				|| qNewDeclr.empty())
		{
			return false;
		}

		if (apply_cdecl(nullptr, addr, qNewDeclr.c_str()))
		{
			decompileFunction(cv, word, true, true);
		}
		else
		{
			WARNING_MSG("Cannot change declaration to: " << qNewDeclr.c_str() << "\n");
		}
	}
	else
	{
		WARNING_MSG("Cannot change declaration for: " << cFnc->getName() << "\n");
	}

	return false;
}

//
//==============================================================================
//

/**
 * Jump to specified address in IDA's disassembly.
 * @param ud Address to jump to.
 */
bool idaapi GhidraDec::jumpToASM(ea_t ea)
{
	jumpto(ea);
	return false;
}

//
//==============================================================================
//

/**
 * Callback for keybord action in custom viewer.
 */
bool idaapi ct_keyboard(TWidget* cv, int key, int shift, void* ud)
{
	RdGlobalInfo* di = static_cast<RdGlobalInfo*>(ud);
	// ESC : move to the previous saved position.
	//
	if (key == 27 && shift == 0)
	{
		return di->pm->moveToPrevious();
	}
	// CTRL + F : move to the next saved position.
	// 70 = 'F'
	//
	else if (key == 70 && shift == 4)
	{
		return di->pm->moveToNext();
	}

	// Get word, function, global, ...
	//
	std::string word;
	int color = -1;
	if (get_current_word(cv, false, word, color))
	{
		return false;
	}
	auto* idaFnc = di->pm->getIdaFunction(word, color);
	const retdec::config::Function* cFnc = di->pm->getWordFunction(word, color);
	const retdec::config::Object* cGv = di->pm->getWordGlobal(word, color);

	// 45 = INSERT
	// 186 = ';'
	//
	if ((key == 45 && shift == 0) || (key == 186 && shift == 0))
	{
		return di->pm->insertCurrentFunctionComment();
	}
	// 78 = N
	//
	else if (key == 78 && shift == 0)
	{
		if (di->navigationActual == di->navigationList.end())
		{
			return false;
		}

		if (cFnc || cGv)
		{
			return di->pm->changeFunctionGlobalName(cv);
		}
		else
		{
			if (di->pm->isWordCurrentParameter(word, color))
			{
				// TODO
			}

			return false;
		}
	}
	// 88 = X
	//
	else if (key == 88 && shift == 0)
	{
		if (idaFnc == nullptr)
		{
			return false;
		}
		di->pm->openXrefsWindow(idaFnc);
	}
	// 67 = C
	//
	else if (key == 67 && shift == 0)
	{
		if (idaFnc == nullptr)
		{
			return false;
		}
		di->pm->openCallsWindow(idaFnc);
	}
	// 89 = Y
	//
	else if (key == 89 && shift == 0)
	{
		return di->pm->changeTypeDeclaration(cv);
	}
	// 65 = A
	//
	else if (key == 65 && shift == 0)
	{
		ea_t addr = 0;
		if (idaFnc)
		{
			addr = idaFnc->start_ea;
		}
		else if (cGv)
		{
			addr = (ea_t)cGv->getStorage().getAddress();
		}
		else
		{
			return false;
		}
		di->pm->jumpToASM(addr);
	}

	return false;
}

//
//==============================================================================
//

ssize_t idaapi GhidraDec::ui_callback(void* ud, int notification_code, va_list va)
{
	RdGlobalInfo* di = static_cast<RdGlobalInfo*>(ud);

	switch (notification_code)
	{
		// called when IDA is preparing a context menu for a view
		// Here dynamic context-depending user menu items can be added.
		case ui_populating_widget_popup:
		{
			TWidget* view = va_arg(va, TWidget*);
			if (view != di->custViewer && view != di->codeViewer)
			{
				return false;
			}

			std::string word;
			int color = -1;
			if (get_current_word(view, false, word, color))
			{
				// fail -> nothing
			}

			auto* idaFnc = di->pm->getIdaFunction(word, color);
			const retdec::config::Function* cFnc = di->pm->getWordFunction(word, color);
			const retdec::config::Object* cGv = di->pm->getWordGlobal(word, color);

			TPopupMenu* p = va_arg(va, TPopupMenu*);

			// Function context.
			//
			if (idaFnc && cFnc)
			{
				attach_action_to_popup(view, p, "-");

				di->pm->jump_to_asm_ah.setAddress(idaFnc->start_ea);
				attach_action_to_popup(view, p, jump_to_asm_ah_t::actionName);

				di->pm->change_fnc_global_name_ah.setView(view);
				attach_action_to_popup(view, p, change_fnc_global_name_ah_t::actionName);

				if (di->pm->isCurrentFunction(idaFnc))
				{
					di->pm->change_fnc_type_ah.setView(view);
					attach_action_to_popup(view, p, change_fnc_type_ah_t::actionName);
				}

				di->pm->open_xrefs_ah.setFunction(idaFnc);
				attach_action_to_popup(view, p, open_xrefs_ah_t::actionName);

				di->pm->open_calls_ah.setFunction(idaFnc);
				attach_action_to_popup(view, p, open_calls_ah_t::actionName);
			}
			// Global var context.
			//
			else if (cGv)
			{
				di->pm->globalAddress = (ea_t)cGv->getStorage().getAddress();

				attach_action_to_popup(view, p, "-");

				di->pm->jump_to_asm_ah.setAddress(di->pm->globalAddress);
				attach_action_to_popup(view, p, jump_to_asm_ah_t::actionName);

				di->pm->change_fnc_global_name_ah.setView(view);
				attach_action_to_popup(view, p, change_fnc_global_name_ah_t::actionName);
			}

			// Common for all contexts.
			//
			attach_action_to_popup(view, p, "-");
			attach_action_to_popup(view, p, change_fnc_comment_ah_t::actionName);
			attach_action_to_popup(view, p, move_backward_ah_t::actionName);
			attach_action_to_popup(view, p, move_forward_ah_t::actionName);
			attach_action_to_popup(view, p, "-");
			break;
		}
	}

	return false;
}

void GhidraDec::registerPermanentActions()
{
	register_action(jump_to_asm_ah_desc);
	register_action(change_fnc_global_name_ah_desc);
	register_action(open_xrefs_ah_desc);
	register_action(open_calls_ah_desc);
	register_action(change_fnc_type_ah_desc);
	register_action(change_fnc_comment_ah_desc);
	register_action(move_forward_ah_desc);
	register_action(move_backward_ah_desc);
}

//
//==============================================================================
//

/**
 * Callback for double click in custom viewer.
 */
bool idaapi ct_double(TWidget* cv, int shift, void* ud)
{
	RdGlobalInfo* di = static_cast<RdGlobalInfo*>(ud);
	std::string word;
	int color = -1;

	if (get_current_word(cv, false, word, color))
	{
		return false;
	}

	if (color == COLOR_DEFAULT || color == COLOR_IMPNAME)
	{
		di->pm->decompileFunction(cv, word);
		return false;
	}

	return false;
}

//
//==============================================================================
//

void idaapi ct_close(TWidget* cv, void* ud)
{
	RdGlobalInfo* di = static_cast<RdGlobalInfo*>(ud);

	di->custViewer = nullptr; //==cv
	if (di->codeViewer) {
		close_widget(di->codeViewer, 0);
		di->codeViewer = nullptr;
	}
}

} // namespace idaplugin
