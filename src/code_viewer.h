/**
 * @file idaplugin/code_viewer.h
 * @brief Module contains classes/methods dealing with decompiled code visualization.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#ifndef IDAPLUGIN_CODE_VIEWER_H
#define IDAPLUGIN_CODE_VIEWER_H

#include <cstdint>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

#include "retdec/utils/string.h"
#include "defs.h"

namespace idaplugin {

/// @name Callbacks reacting on custom viewer.
/// @{
bool idaapi ct_keyboard(TWidget* cv, int key, int shift, void* ud);
bool idaapi ct_double(TWidget* cv, int shift, void* ud);
void idaapi ct_close(TWidget* cv, void* ud);
/// @}

void registerPermanentActions();
ssize_t idaapi ui_callback(void* ud, int notification_code, va_list va);

/// @name Functions working with GUI from threads.
/// @{
//int idaapi showDecompiledCode(void *ud);
/// @}

/**
 * All the handlers for our custom view.
 */
static const custom_viewer_handlers_t handlers(
		ct_keyboard, // keyboard
		nullptr,     // popup
		nullptr,     // mouse_moved
		nullptr,     // click
		ct_double,   // dblclick
		nullptr,     // current position change
		ct_close,     // close
		nullptr,     // help
		nullptr,     // adjust_place
		nullptr,
		nullptr,
		nullptr
);

/**
 * This is a structure defining execute() method which will be executed
 * by master thread through 'execute_sync()' call.
 * Its purpose is to display decompiled code in custom viewer.
 *
 * It sets plugin's tform and viewer.
 *
 * It uses current 'decompiledFunction' to get associated code from 'fnc2code'.
 *
 * The viewer keeps one stable line buffer so navigation can refresh in-place
 * without closing the widget that is currently dispatching an action.
 */
struct ShowOutput : public exec_request_t
{
	RdGlobalInfo *di;

	ShowOutput(RdGlobalInfo *i) : di(i) {}
	virtual ~ShowOutput() override {}

	virtual GHIDRADEC_EXEC_RETURN idaapi execute() override
	{
		if (!loadFunctionCode())
		{
			return 0;
		}

		simpleline_place_t minPlace;
		simpleline_place_t curPlace = minPlace;
		simpleline_place_t maxPlace((int)di->viewerLines.size()-1);

		if (di->custViewer != nullptr && di->codeViewer != nullptr)
		{
			set_custom_viewer_range(di->custViewer, &minPlace, &maxPlace);
			refresh_custom_viewer(di->custViewer);
			repaint_custom_viewer(di->custViewer);
			return 0;
		}

		if (di->custViewer != nullptr)
		{
			close_widget(di->custViewer, 0);
			di->custViewer = nullptr;
		}
		if (di->codeViewer != nullptr)
		{
			close_widget(di->codeViewer, 0);
			di->codeViewer = nullptr;
		}

		di->custViewer = create_custom_viewer(
				di->viewerName.c_str(), // name of viewer
				&minPlace,              // first location of the viewer
				&maxPlace,              // last location of the viewer
				&curPlace,              // set current location
				nullptr,                // renderer information (can be NULL)
				&di->viewerLines,       // contents of viewer
				&handlers,              // handlers for the viewer (can be NULL)
				di,                     // handlers' user data
				nullptr                 // widget to hold viewer
		);

		di->codeViewer = create_code_viewer(di->custViewer);

		set_code_viewer_is_source(di->codeViewer);

		display_widget(
				di->codeViewer,
				WOPN_DP_TAB |
#if !defined(IDA_SDK_VERSION) || IDA_SDK_VERSION < 720
				WOPN_MENU |
#endif
				/// we want to catch ESC and use it for navigation
				WOPN_NOT_CLOSED_BY_ESC
		);

		return 0;
	}

	bool idaapi loadFunctionCode()
	{
		addCommentToFunctionCode();
		func_t* fnc = di->decompiledFunction;
		auto fit = di->fnc2code.find(fnc);
		if (fit == di->fnc2code.end())
		{
			return false;
		}

		std::istringstream f(fit->second.code);
		std::string line;
		di->viewerLines.clear();
		while (std::getline(f, line))
		{
			di->viewerLines.push_back(simpleline_t(line.c_str()));
		}
		if (di->viewerLines.empty())
		{
			di->viewerLines.push_back(simpleline_t(""));
		}

		return true;
	}

	void idaapi addCommentToFunctionCode()
	{
		func_t* fnc = di->decompiledFunction;
		auto fit = di->fnc2code.find(fnc);
		if (fit == di->fnc2code.end())
		{
			return;
		}
		qstring fncCmt;
		if (get_func_cmt(&fncCmt, fnc, false) <= 0)
		{
			return;
		}

		auto& code = fit->second.code;
		std::istringstream f(code);
		std::string line;
		std::list<std::string> lines;
		while (std::getline(f, line))
		{
			lines.push_back(line.c_str());
		}
		bool active = false;
		for (auto it = lines.begin(); it!= lines.end(); ++it)
		{
			std::string l = *it;

			std::regex e1(std::string(SCOLOR_ON)
					+ SCOLOR_AUTOCMT
					+ "// -* Functions -*"
					+ SCOLOR_OFF
					+ SCOLOR_AUTOCMT);

			if (std::regex_match(l, e1))
			{
				active = true;
				continue;
			}

			if (!active)
			{
				continue;
			}

			std::regex e2(std::string(SCOLOR_ON)
					+ SCOLOR_AUTOCMT
					+ "// Comment:.*"
					+ SCOLOR_OFF
					+ SCOLOR_AUTOCMT);

			if (std::regex_match(l, e2))
			{
				std::regex e3(std::string(SCOLOR_ON)
						+ SCOLOR_AUTOCMT
						+ "// .*"
						+ SCOLOR_OFF
						+ SCOLOR_AUTOCMT);

				while (std::regex_match(l, e3))
				{
					it = lines.erase(it);
					if (it == lines.end())
						break;
					l = *it;
				}
			}
			if (it == lines.end())
				break;

			qstring cFncName;
			get_func_name(&cFncName, fnc->start_ea);
			std::string tmpFncName = cFncName.c_str();
			std::string tmpFncNameTrim = retdec::utils::removeLeadingCharacter(
					tmpFncName,
					'_');

			std::regex e4(".*"
					+ std::string(SCOLOR_ON)
					+ SCOLOR_DEFAULT
					+ tmpFncName
					+ SCOLOR_OFF
					+ SCOLOR_DEFAULT
					+ ".*" );

			std::regex e5(".*"
					+ std::string(SCOLOR_ON)
					+ SCOLOR_DEFAULT
					+ tmpFncNameTrim
					+ SCOLOR_OFF
					+ SCOLOR_DEFAULT
					+ ".*" );

			if (std::regex_match(l, e4) || std::regex_match(l, e5))
			{
				bool first = true;
				std::istringstream ff(fncCmt.c_str());
				std::string cLine;
				while (std::getline(ff, cLine))
				{
					if (first)
					{
						std::string prolog = std::string(SCOLOR_ON)
								+ SCOLOR_AUTOCMT
								+ "// Comment:"
								+ SCOLOR_OFF
								+ SCOLOR_AUTOCMT;

						lines.insert(it, prolog);
						first = false;
					}

					cLine = std::string(SCOLOR_ON)
							+ SCOLOR_AUTOCMT
							+ "//     "
							+ cLine
							+ SCOLOR_OFF
							+ SCOLOR_AUTOCMT;

					lines.insert(it, cLine);
				}

				break;
			}
		}

		fit->second.code.clear();
		for (auto& l : lines)
			fit->second.code += l + "\n";
	}
};

} // namespace idaplugin

#endif
