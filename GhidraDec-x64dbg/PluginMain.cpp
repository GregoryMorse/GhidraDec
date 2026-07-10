#include "PluginMain.h"

#include <_scriptapi_module.h>

#include <shlwapi.h>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <string>

#define PLUGIN_NAME "GhidraDec"
#define PLUGIN_VERSION 1

int Plugin::handle;
HWND Plugin::hwndDlg;
int Plugin::hMenu;
int Plugin::hMenuDisasm;
int Plugin::hMenuDump;
int Plugin::hMenuStack;
int Plugin::hMenuGraph;
int Plugin::hMenuMemmap;
int Plugin::hMenuSymmod;

namespace
{
	enum MenuEntry
	{
		MENU_OPEN_GHIDRA = 1,
		MENU_ANALYZE_CURRENT_MODULE,
		MENU_OPEN_PROJECT,
		MENU_LOG_SELECTED_ADDRESS,
		MENU_DISASM_ANALYZE_MODULE,
	};

	std::string quoteArg(const std::string& value)
	{
		std::string result = "\"";
		for (char ch : value)
		{
			if (ch == '"')
				result += "\\\"";
			else
				result += ch;
		}
		result += '"';
		return result;
	}

	void logLine(const std::string& line)
	{
		GuiAddLogMessage(("[GhidraDec] " + line + "\n").c_str());
	}

	void statusLine(const std::string& line)
	{
		GuiAddStatusBarMessage(("[GhidraDec] " + line + "\n").c_str());
	}

	std::string pathJoin(const std::string& left, const std::string& right)
	{
		if (left.empty())
			return right;
		const char tail = left[left.size() - 1];
		if (tail == '\\' || tail == '/')
			return left + right;
		return left + "\\" + right;
	}

	std::string envVar(const char* name)
	{
		char value[MAX_PATH] = {};
		const DWORD count = GetEnvironmentVariableA(name, value, MAX_PATH);
		if (count == 0 || count >= MAX_PATH)
			return std::string();
		return value;
	}

	std::string ghidraInstallDir()
	{
		return envVar("GHIDRA_INSTALL_DIR");
	}

	std::string ghidraPath(const char* relative)
	{
		return pathJoin(ghidraInstallDir(), relative);
	}

	bool fileExists(const std::string& path)
	{
		const DWORD attrs = GetFileAttributesA(path.c_str());
		return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0;
	}

	bool directoryExists(const std::string& path)
	{
		const DWORD attrs = GetFileAttributesA(path.c_str());
		return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

	void ensureDirectory(const std::string& path)
	{
		if (!path.empty() && !directoryExists(path))
			CreateDirectoryA(path.c_str(), nullptr);
	}

	std::string defaultProjectDir()
	{
		const std::string configured = envVar("GHIDRADEC_X64DBG_PROJECT_DIR");
		if (!configured.empty())
			return configured;

		const std::string localAppData = envVar("LOCALAPPDATA");
		if (!localAppData.empty())
			return pathJoin(localAppData, "GhidraDec-x64dbg\\projects");

		char tempPath[MAX_PATH] = {};
		if (!GetTempPathA(MAX_PATH, tempPath))
			return ".";
		return pathJoin(tempPath, "GhidraDec-x64dbg");
	}

	std::string projectName()
	{
		return "x64dbg";
	}

	std::string projectFile()
	{
		return pathJoin(defaultProjectDir(), projectName() + ".gpr");
	}

	bool launchProcess(const std::string& file, const std::string& params, const std::string& workingDir = std::string())
	{
		SHELLEXECUTEINFOA sei = {};
		sei.cbSize = sizeof(sei);
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		sei.hwnd = Plugin::hwndDlg;
		sei.lpVerb = "open";
		sei.lpFile = file.c_str();
		sei.lpParameters = params.empty() ? nullptr : params.c_str();
		sei.lpDirectory = workingDir.empty() ? nullptr : workingDir.c_str();
		sei.nShow = SW_SHOWNORMAL;

		if (!ShellExecuteExA(&sei))
		{
			char message[512] = {};
			std::snprintf(message, sizeof(message), "ShellExecute failed for %s (GetLastError=%lu)", file.c_str(), GetLastError());
			logLine(message);
			return false;
		}

		if (sei.hProcess)
			CloseHandle(sei.hProcess);
		return true;
	}

	duint selectedAddress()
	{
		SELECTIONDATA selection = {};
		if (GuiSelectionGet(GUI_DISASSEMBLY, &selection))
			return selection.start;
		return Script::Module::GetMainModuleBase();
	}

	bool modulePathFromAddress(duint address, std::string& path)
	{
		char buffer[MAX_PATH] = {};
		if (address && Script::Module::PathFromAddr(address, buffer))
		{
			path = buffer;
			return true;
		}
		if (Script::Module::GetMainModulePath(buffer))
		{
			path = buffer;
			return true;
		}
		return false;
	}

	std::string moduleNameFromAddress(duint address)
	{
		char buffer[MAX_MODULE_SIZE] = {};
		if (address && Script::Module::NameFromAddr(address, buffer))
			return buffer;
		if (Script::Module::GetMainModuleName(buffer))
			return buffer;
		return "<unknown>";
	}

	void logSelectedAddress()
	{
		const duint address = selectedAddress();
		const duint base = Script::Module::BaseFromAddr(address);
		char line[512] = {};
		std::snprintf(line, sizeof(line), "selection=%p module=%s base=%p rva=%p",
			reinterpret_cast<void*>(address),
			moduleNameFromAddress(address).c_str(),
			reinterpret_cast<void*>(base),
			reinterpret_cast<void*>(base ? address - base : 0));
		logLine(line);
	}

	bool launchGhidra(const std::string& args)
	{
		const std::string ghidraRun = ghidraPath("ghidraRun.bat");
		if (ghidraInstallDir().empty())
		{
			logLine("GHIDRA_INSTALL_DIR is not set.");
			return false;
		}
		if (!fileExists(ghidraRun))
		{
			logLine("Could not find " + ghidraRun);
			return false;
		}
		return launchProcess(ghidraRun, args, ghidraInstallDir());
	}

	void openGhidra()
	{
		if (launchGhidra(""))
			statusLine("launched Ghidra");
	}

	void openProject()
	{
		const std::string gpr = projectFile();
		if (!fileExists(gpr))
		{
			logLine("No Ghidra project exists yet at " + gpr + ". Run Analyze current module first.");
			return;
		}

		if (launchGhidra(quoteArg(gpr)))
			statusLine("opened Ghidra project " + gpr);
	}

	void analyzeCurrentModule()
	{
		std::string modulePath;
		const duint address = selectedAddress();
		if (!modulePathFromAddress(address, modulePath))
		{
			logLine("No debuggee module is available. Start or attach to a process first.");
			return;
		}

		const std::string analyzeHeadless = ghidraPath("support\\analyzeHeadless.bat");
		if (ghidraInstallDir().empty())
		{
			logLine("GHIDRA_INSTALL_DIR is not set.");
			return;
		}
		if (!fileExists(analyzeHeadless))
		{
			logLine("Could not find " + analyzeHeadless);
			return;
		}

		const std::string projectDir = defaultProjectDir();
		ensureDirectory(projectDir);

		const std::string params = quoteArg(projectDir) + " " + quoteArg(projectName()) +
			" -import " + quoteArg(modulePath) + " -overwrite";

		logLine("starting Ghidra headless analysis for " + modulePath);
		logLine("project: " + projectFile());

		if (launchProcess(analyzeHeadless, params, ghidraInstallDir()))
			statusLine("Ghidra analysis started for " + moduleNameFromAddress(address));
	}

	void setupMenus()
	{
		_plugin_menuaddentry(Plugin::hMenu, MENU_OPEN_GHIDRA, "Launch Ghidra");
		_plugin_menuaddentry(Plugin::hMenu, MENU_ANALYZE_CURRENT_MODULE, "Analyze current module");
		_plugin_menuaddentry(Plugin::hMenu, MENU_OPEN_PROJECT, "Open analysis project");
		_plugin_menuaddseparator(Plugin::hMenu);
		_plugin_menuaddentry(Plugin::hMenu, MENU_LOG_SELECTED_ADDRESS, "Log selected address context");

		_plugin_menuaddentry(Plugin::hMenuDisasm, MENU_DISASM_ANALYZE_MODULE, "Ghidra: analyze current module");
		_plugin_menuentrysethotkey(Plugin::handle, MENU_DISASM_ANALYZE_MODULE, "Ctrl+Shift+F5");
	}
}

extern "C" __declspec(dllexport) bool pluginit(PLUG_INITSTRUCT* initStruct)
{
	initStruct->pluginVersion = PLUGIN_VERSION;
	initStruct->sdkVersion = PLUG_SDKVERSION;
	strcpy_s(initStruct->pluginName, PLUGIN_NAME);
	Plugin::handle = initStruct->pluginHandle;
	return true;
}

extern "C" __declspec(dllexport) void plugsetup(PLUG_SETUPSTRUCT* setupStruct)
{
	Plugin::hwndDlg = setupStruct->hwndDlg;
	Plugin::hMenu = setupStruct->hMenu;
	Plugin::hMenuDisasm = setupStruct->hMenuDisasm;
	Plugin::hMenuDump = setupStruct->hMenuDump;
	Plugin::hMenuStack = setupStruct->hMenuStack;
	Plugin::hMenuGraph = setupStruct->hMenuGraph;
	Plugin::hMenuMemmap = setupStruct->hMenuMemmap;
	Plugin::hMenuSymmod = setupStruct->hMenuSymmod;
	setupMenus();
	logLine("loaded. Ghidra path: " + ghidraInstallDir());
}

extern "C" __declspec(dllexport) bool plugstop()
{
	logLine("unloaded");
	return Plugin::handle != 0;
}

extern "C" __declspec(dllexport) void CBMENUENTRY(CBTYPE, PLUG_CB_MENUENTRY* info)
{
	switch (info->hEntry)
	{
	case MENU_OPEN_GHIDRA:
		openGhidra();
		break;
	case MENU_ANALYZE_CURRENT_MODULE:
	case MENU_DISASM_ANALYZE_MODULE:
		analyzeCurrentModule();
		break;
	case MENU_OPEN_PROJECT:
		openProject();
		break;
	case MENU_LOG_SELECTED_ADDRESS:
		logSelectedAddress();
		break;
	default:
		break;
	}
}
