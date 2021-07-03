/**
 * @file idaplugin/idaplugin.h
 * @brief Main idaplugin header.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#ifndef IDAPLUGIN_IDAPLUGIN_H
#define IDAPLUGIN_IDAPLUGIN_H

#include "defs.h"

namespace idaplugin {
	class GhidraDec	
#if IDA_SDK_VERSION >= 750
		: public plugmod_t, public event_listener_t
#endif
	{
	public:
		GhidraDec();
		virtual ~GhidraDec();
		bool canDecompileInput();
		void runSelectiveDecompilation(func_t* fnc2decomp = nullptr, bool force = false);
		/**
		 * General info used by plugin.
		 */
		RdGlobalInfo* decompInfo;
		ea_t globalAddress = 0;

		void killDecompilation(bool bTerminate);
		void saveIdaDatabase(bool inSitu = false, const std::string& suffix = ".dec-backup");
		void generatePluginDatabase();
		bool isRelocatable();
		void runAllDecompilation();
		bool setInputPath();
#if IDA_SDK_VERSION < 750
		static
#endif
#ifdef __X64__
		bool idaapi run(size_t arg);
#else
		void idaapi run(int arg);
#endif
#if IDA_SDK_VERSION >= 750
		ssize_t idaapi on_event(ssize_t notification_code, va_list va);
#else
		static ssize_t idaapi view_callback(void* ud, int notification_code, va_list va);
#endif
		int idaapi init();
		void idaapi term();
		bool isWordGlobal(const std::string& word, int color);
		const retdec::config::Object* getWordGlobal(const std::string& word, int color);
		bool isWordFunction(const std::string& word, int color);
		const retdec::config::Function* getWordFunction(
			const std::string& word,
			int color);
		func_t* getIdaFunction(const std::string& word, int color);
		bool isCurrentFunction(func_t* fnc);
		func_t* getCurrentFunction();
		bool isWordCurrentParameter(const std::string& word, int color);
		void decompileFunction(
			TWidget* cv,
			const std::string& calledFnc,
			bool force = false,
			bool forceDec = false);
		bool idaapi moveToPrevious();

		struct move_backward_ah_t : public action_handler_t
		{
			move_backward_ah_t(GhidraDec* pm = nullptr) : action_handler_t() {
				this->pm = pm;
			}
			GhidraDec* pm;
			static const char* actionName;
			static const char* actionLabel;
			static const char* actionHotkey;

			virtual int idaapi activate(action_activation_ctx_t*)
			{
				pm->moveToPrevious();
				return false;
			}

			virtual action_state_t idaapi update(action_update_ctx_t*)
			{
				return AST_ENABLE_ALWAYS;
			}
		};
		move_backward_ah_t move_backward_ah = move_backward_ah_t(this);
		const action_desc_t move_backward_ah_desc = ACTION_DESC_LITERAL_PLUGMOD(
			move_backward_ah_t::actionName,
			move_backward_ah_t::actionLabel,
			&move_backward_ah,
			this,
			nullptr,
			move_backward_ah_t::actionHotkey,
			-1);
		bool idaapi moveToNext();
		struct move_forward_ah_t : public action_handler_t
		{
			move_forward_ah_t(GhidraDec* pm = nullptr) : action_handler_t() {
				this->pm = pm;
			}
			GhidraDec* pm;
			static const char* actionName;
			static const char* actionLabel;
			static const char* actionHotkey;

			virtual int idaapi activate(action_activation_ctx_t*)
			{
				pm->moveToNext();
				return false;
			}

			virtual action_state_t idaapi update(action_update_ctx_t*)
			{
				return AST_ENABLE_ALWAYS;
			}
		};


		move_forward_ah_t move_forward_ah;
		const action_desc_t move_forward_ah_desc = ACTION_DESC_LITERAL_PLUGMOD(
			move_forward_ah_t::actionName,
			move_forward_ah_t::actionLabel,
			&move_forward_ah,
			this,
			nullptr,
			move_forward_ah_t::actionHotkey,
			-1);
		bool idaapi insertCurrentFunctionComment();
		struct change_fnc_comment_ah_t : public action_handler_t
		{
			change_fnc_comment_ah_t(GhidraDec* pm = nullptr) : action_handler_t() {
				this->pm = pm;
			}
			GhidraDec* pm;
			static const char* actionName;
			static const char* actionLabel;
			static const char* actionHotkey;

			virtual int idaapi activate(action_activation_ctx_t*)
			{
				pm->insertCurrentFunctionComment();
				return false;
			}

			virtual action_state_t idaapi update(action_update_ctx_t*)
			{
				return AST_ENABLE_ALWAYS;
			}
		};

		change_fnc_comment_ah_t change_fnc_comment_ah;
		const action_desc_t change_fnc_comment_ah_desc = ACTION_DESC_LITERAL_PLUGMOD(
			change_fnc_comment_ah_t::actionName,
			change_fnc_comment_ah_t::actionLabel,
			&change_fnc_comment_ah,
			this,
			nullptr,
			change_fnc_comment_ah_t::actionHotkey,
			-1);
		bool idaapi changeFunctionGlobalName(TWidget* cv);
		struct change_fnc_global_name_ah_t : public action_handler_t
		{
			change_fnc_global_name_ah_t(GhidraDec* pm = nullptr) : action_handler_t() {
				this->pm = pm;
			}
			GhidraDec* pm;
			TWidget* view = nullptr;
			static const char* actionName;
			static const char* actionLabel;
			static const char* actionHotkey;

			virtual int idaapi activate(action_activation_ctx_t*)
			{
				pm->changeFunctionGlobalName(view);
				return false;
			}

			virtual action_state_t idaapi update(action_update_ctx_t*)
			{
				return AST_ENABLE_ALWAYS;
			}

			void setView(TWidget* v)
			{
				view = v;
			}
		};
		bool idaapi openXrefsWindow(func_t* fnc);
		change_fnc_global_name_ah_t change_fnc_global_name_ah;
		const action_desc_t change_fnc_global_name_ah_desc = ACTION_DESC_LITERAL_PLUGMOD(
			change_fnc_global_name_ah_t::actionName,
			change_fnc_global_name_ah_t::actionLabel,
			&change_fnc_global_name_ah,
			this,
			nullptr,
			change_fnc_global_name_ah_t::actionHotkey,
			-1);
		struct open_xrefs_ah_t : public action_handler_t
		{
			open_xrefs_ah_t(GhidraDec* pm = nullptr) : action_handler_t() {
				this->pm = pm;
			}
			GhidraDec* pm;
			func_t* fnc = nullptr;
			static const char* actionName;
			static const char* actionLabel;
			static const char* actionHotkey;

			virtual int idaapi activate(action_activation_ctx_t*)
			{
				pm->openXrefsWindow(fnc);
				return false;
			}

			virtual action_state_t idaapi update(action_update_ctx_t*)
			{
				return AST_ENABLE_ALWAYS;
			}

			void setFunction(func_t* f)
			{
				fnc = f;
			}
		};

		bool idaapi openCallsWindow(func_t* fnc);
		open_xrefs_ah_t open_xrefs_ah;
		const action_desc_t open_xrefs_ah_desc = ACTION_DESC_LITERAL_PLUGMOD(
			open_xrefs_ah_t::actionName,
			open_xrefs_ah_t::actionLabel,
			&open_xrefs_ah,
			this,
			nullptr,
			open_xrefs_ah_t::actionHotkey,
			-1);
		struct open_calls_ah_t : public action_handler_t
		{
			open_calls_ah_t(GhidraDec* pm = nullptr) : action_handler_t() {
				this->pm = pm;
			}
			GhidraDec* pm;
			func_t* fnc = nullptr;
			static const char* actionName;
			static const char* actionLabel;
			static const char* actionHotkey;

			virtual int idaapi activate(action_activation_ctx_t*)
			{
				pm->openCallsWindow(fnc);
				return false;
			}

			virtual action_state_t idaapi update(action_update_ctx_t*)
			{
				return AST_ENABLE_ALWAYS;
			}

			void setFunction(func_t* f)
			{
				fnc = f;
			}
		};

		open_calls_ah_t open_calls_ah;
		const action_desc_t open_calls_ah_desc = ACTION_DESC_LITERAL_PLUGMOD(
			open_calls_ah_t::actionName,
			open_calls_ah_t::actionLabel,
			&open_calls_ah,
			this,
			nullptr,
			open_calls_ah_t::actionHotkey,
			-1);
		bool idaapi changeTypeDeclaration(TWidget* cv);
		struct change_fnc_type_ah_t : public action_handler_t
		{
			change_fnc_type_ah_t(GhidraDec* pm = nullptr) : action_handler_t() {
				this->pm = pm;
			}
			GhidraDec* pm;
			TWidget* view = nullptr;
			static const char* actionName;
			static const char* actionLabel;
			static const char* actionHotkey;

			virtual int idaapi activate(action_activation_ctx_t*)
			{
				pm->changeTypeDeclaration(view);
				return false;
			}

			virtual action_state_t idaapi update(action_update_ctx_t*)
			{
				return AST_ENABLE_ALWAYS;
			}

			void setView(TWidget* v)
			{
				view = v;
			}
		};

		bool idaapi jumpToASM(ea_t ea);
		change_fnc_type_ah_t change_fnc_type_ah;
		const action_desc_t change_fnc_type_ah_desc = ACTION_DESC_LITERAL_PLUGMOD(
			change_fnc_type_ah_t::actionName,
			change_fnc_type_ah_t::actionLabel,
			&change_fnc_type_ah,
			this,
			nullptr,
			change_fnc_type_ah_t::actionHotkey,
			-1);
		struct jump_to_asm_ah_t : public action_handler_t
		{
			jump_to_asm_ah_t(GhidraDec* pm = nullptr) : action_handler_t() {
				this->pm = pm;
			}
			GhidraDec* pm;
			ea_t addr;
			static const char* actionName;
			static const char* actionLabel;
			static const char* actionHotkey;

			virtual int idaapi activate(action_activation_ctx_t*)
			{
				pm->jumpToASM(addr);
				return false;
			}

			virtual action_state_t idaapi update(action_update_ctx_t*)
			{
				return AST_ENABLE_ALWAYS;
			}

			void setAddress(ea_t a)
			{
				addr = a;
			}
		};

		jump_to_asm_ah_t jump_to_asm_ah;
		const action_desc_t jump_to_asm_ah_desc = ACTION_DESC_LITERAL_PLUGMOD(
			jump_to_asm_ah_t::actionName,
			jump_to_asm_ah_t::actionLabel,
			&jump_to_asm_ah,
			this,
			nullptr,
			jump_to_asm_ah_t::actionHotkey,
			-1);
		static ssize_t idaapi ui_callback(void* ud, int notification_code, va_list va);
		void registerPermanentActions();
	};

} // namespace idaplugin

#endif
