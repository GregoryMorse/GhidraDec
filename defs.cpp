/**
 * @file idaplugin/defs.cpp
 * @brief Plugin-global definitions and includes.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "defs.h"
#include "plugin_config.h"
#include "decompiler.h"
#include "sleighinterface.h"

#if defined(OS_WINDOWS)
	#include <windows.h>
#ifndef __X64__
	#include <tlhelp32.h>
#endif
#else
#define _pipe(x,y,z) pipe2(x,z)
#define _dup dup
#define _dup2 dup2
#define _close close
#define STDOUT_FILENO 1
#define STDIN_FILENO 0
#define _O_NOINHERIT O_CLOEXEC
#define _O_BINARY 0
#endif

namespace idaplugin {

#if defined(OS_WINDOWS) && !defined(__X64__)
	BOOL CALLBACK enumWinCB(HWND hWnd, LPARAM lParam)
	{
		unsigned long pid = 0;
		GetWindowThreadProcessId(hWnd, &pid);

		if (lParam != pid || GetWindow(hWnd, GW_OWNER) != (HWND)0 || !IsWindowVisible(hWnd)) {
			return TRUE;
		}
		ShowWindow(hWnd, SW_HIDE);
		return FALSE;
	}
#endif

	void stopDecompilation(RdGlobalInfo* di, bool deregister, bool checkonly, bool closehandles)
	{
		if (di->decompPid) {
			try {
				if (deregister) di->idacb->decInt->deregisterProgram();
			} catch (DecompError& e) {
				WARNING_GUI("Caught deregistration error: " << e.explain);
			}
			int rc = 0;
			if (check_process_exit(di->hDecomp, &rc, 0) != 0) {
				if (checkonly) return;
				term_process(di->hDecomp);
			}
			di->decompPid = 0;
		}
		if (closehandles) {
			if (di->rdHandle != -1) {
				_close(di->rdHandle); di->rdHandle = -1;
			}
			if (di->wrHandle != -1) {
				_close(di->wrHandle); di->wrHandle = -1;
			}
		}
	}
/**
 * Run command using IDA SDK API.
 */
	int runCommand(
		const std::string& cmd,
		const std::string& args,
		intptr_t* pid,
		void** hdl,
		int* readpipe,
		int* writepipe,
		bool showWarnings)
{
	launch_process_params_t procInf;
	procInf.path = cmd.c_str();
	procInf.args = args.size() == 0 ? NULL : args.c_str();
	procInf.flags = LP_HIDE_WINDOW;
#if defined(OS_WINDOWS)
	/*SECURITY_ATTRIBUTES saAttr; // Set the bInheritHandle flag so pipe handles are inherited. 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	HANDLE g_hChildStd_IN_Rd = NULL;
	HANDLE g_hChildStd_IN_Wr = NULL;
	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;
	// Create a pipe for the child process's STDOUT. 
	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
		WARNING_GUI(TEXT("StdoutRd CreatePipe"));
	// Ensure the read handle to the pipe for STDOUT is not inherited.
	if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
		WARNING_GUI(TEXT("Stdout SetHandleInformation"));
	// Create a pipe for the child process's STDIN. 
	if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
		WARNING_GUI(TEXT("Stdin CreatePipe"));
	// Ensure the write handle to the pipe for STDIN is not inherited. 
	if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
		WARNING_GUI(TEXT("Stdin SetHandleInformation"));
	procInf.in_handle = (ssize_t)g_hChildStd_IN_Rd;
	procInf.out_handle = (ssize_t)g_hChildStd_OUT_Wr;
	procInf.err_handle = procInf.out_handle;
	*writepipe = _open_osfhandle((intptr_t)g_hChildStd_IN_Wr, _O_WRONLY);
	*readpipe = _open_osfhandle((intptr_t)g_hChildStd_OUT_Rd, _O_RDONLY);*/
	PROCESS_INFORMATION pi{};
	procInf.info = &pi;
#endif
//IDA blocks stdout/stdin but on Windows could use _fileno(__acrt_iob_func(1/0)), which returns invalid value -2 as no handle is assigned since no console window present
	int rpipefd[2], wpipefd[2];
	//qhandle_t rpipefd[2], wpipefd[2]; //problem is the inheritence or close on exec flags are critical for the stdout/stdin child process
	if (_pipe(rpipefd, 4096, _O_BINARY | _O_NOINHERIT) < 0) {
	//if (qpipe_create(rpipefd) < 0) {
		WARNING_GUI("Pipe creation failed");
	}
	if (_pipe(wpipefd, 4096, _O_BINARY | _O_NOINHERIT) < 0) {
	//if (qpipe_create(wpipefd) < 0) {
		WARNING_GUI("Pipe creation failed");
	}
#if defined(OS_WINDOWS)
	
	int fdStdIn = _dup(wpipefd[0]);// _dup(STDIN_FILENO);
	int fdStdOut = _dup(rpipefd[1]); // _dup(STDOUT_FILENO);
#else
	int fdStdIn = _dup(STDIN_FILENO);
	int fdStdOut = _dup(STDOUT_FILENO);
	if (_dup2(wpipefd[0], STDIN_FILENO) != 0) WARNING_GUI("Pipe handle duplication failed");
	if (_dup2(rpipefd[1], STDOUT_FILENO) != 0) WARNING_GUI("Pipe handle duplication failed");
#endif
	_close(rpipefd[1]);
	_close(wpipefd[0]);
	*readpipe = rpipefd[0];
	*writepipe = wpipefd[1];
#if defined(OS_WINDOWS)
	procInf.in_handle = _get_osfhandle(fdStdIn); // (ssize_t)wpipefd[0];
	procInf.out_handle = _get_osfhandle(fdStdOut); // (ssize_t)rpipefd[1];
	procInf.err_handle = procInf.out_handle;// (ssize_t)rpipefd[1];
#endif
	qstring errbuf;
	DBG_MSG("Launching: " << cmd << " " << args << "\n");
	void* p = launch_process(procInf, &errbuf);
/*#if defined(OS_WINDOWS)
	CloseHandle(g_hChildStd_IN_Rd);
	//CloseHandle(g_hChildStd_IN_Wr);
	//CloseHandle(g_hChildStd_OUT_Rd);
	CloseHandle(g_hChildStd_OUT_Wr);
#else*/
#if !defined(OS_WINDOWS)
	if (_dup2(fdStdOut, STDOUT_FILENO) != 0) WARNING_GUI("Pipe handle duplicatoin restoration failed");
	if (_dup2(fdStdIn, STDIN_FILENO) != 0) WARNING_GUI("Pipe handle duplication restoration failed");;
#endif
	_close(fdStdOut);
	_close(fdStdIn);
//#endif
	if (p == nullptr)
	{
		if (*readpipe != -1) {
			_close(*readpipe); *readpipe = -1;
		}
		if (*writepipe != -1) {
			_close(*writepipe); *writepipe = -1;
		}
		if (showWarnings)
		{
			WARNING_GUI("launch_process(" << procInf.path << " "
					<< procInf.args << ") failed to launch " << errbuf.c_str()
					<< "\n");
		}
		return 1;
	}
	*hdl = p;
	if (pid)
	{
#if defined(OS_WINDOWS)
	*pid = pi.dwProcessId;
#ifndef __X64__
	//of course there is no nice way to do this, hope the delay allows the process to create its window or it wont hide	
	int rc = 0, ctr = 0; //up to one second only before bailing out as user could have closed the window or other situation would cause a hang
	while (check_process_exit(p, &rc, 0) == 1 && EnumWindows(enumWinCB, (LPARAM)* pid) && ctr != 20) {
		qsleep(50); ctr++;
	}
	/*HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcessSnap, &pe32)) {
			do {
				if (pe32.th32ParentProcessID == *pid)
					EnumWindows(enumWinCB, (LPARAM)pe32.th32ProcessID);
			} while (Process32Next(hProcessSnap, &pe32));
		}
		CloseHandle(hProcessSnap);
	}*/
#endif
	//*hdl = pi.hProcess;
	//CloseHandle(pi.hProcess); //must keep because its a child process and use for check status and but must close process in Windows...
	//WARNING_GUI("p: " << std::hex << (__int64)p << " pi.hProcess: " << pi.hProcess << " pi.hThread: " << pi.hThread);
	if (pi.hThread != INVALID_HANDLE_VALUE) {
		if (!CloseHandle(pi.hThread)) WARNING_GUI("Failed to close thread handle");
	}
#else // Linux || macOS
	*pid = reinterpret_cast<intptr_t>(p);
#endif
	}

	int rc = 0;
	auto cpe = check_process_exit(p, &rc, 0); //1,-1 for wait infinitely
	DBG_MSG("Launched Pid: " << p << " Exit code: " << cpe << " Return code: " << rc << "\n");

	if (pid && (cpe != 1 || rc != 0))
	{
		*pid = 0;
	}
	if (cpe != 1)
	{
		if (*readpipe != -1) {
			_close(*readpipe); *readpipe = -1;
		}
		if (*writepipe != -1) {
			_close(*writepipe); *writepipe = -1;
		}
		if (showWarnings)
		{
			WARNING_GUI("Error in check_process_exit() while executing: "
					<< procInf.path << " " << procInf.args << "\n");
		}

		return 1;
	}

	if (rc != 0)
	{
		if (*readpipe != -1) {
			_close(*readpipe); *readpipe = -1;
		}
		if (*writepipe != -1) {
			_close(*writepipe); *writepipe = -1;
		}
		if (showWarnings)
		{
			WARNING_GUI("launch_process(" << procInf.path << " " << procInf.args
					<< ") failed with error code " << rc << "\n");
		}

		return 1;
	}

	return 0;
}

RdGlobalInfo::RdGlobalInfo() :
		pluginConfigFile(get_user_idadir())
{
	pluginInfo.id = pluginID.data();
	pluginInfo.name = pluginName.data();
	pluginInfo.producer = pluginProducer.data();
	pluginInfo.version = pluginVersion.data();
	pluginInfo.url = pluginContact.data();
	pluginInfo.freeform = pluginCopyright.data();

	navigationActual = navigationList.end();

	pluginConfigFile.append(pluginConfigFileName);
}

bool RdGlobalInfo::isAllDecompilation()
{
	return !outputFile.empty();
}

bool RdGlobalInfo::isSelectiveDecompilation()
{
	return !isAllDecompilation();
}

bool RdGlobalInfo::isDecompilerInSpecifiedPath() const
{
	struct stat st;
	return stat((decompilerExePath + decompilerExeName).data(), &st) == 0;
}

bool RdGlobalInfo::isDecompilerInSystemPath()
{
	char buff[MAXSTR];
	if (search_path(buff, sizeof(buff), decompilerExeName.c_str(), false))
	{
		struct stat st;
		if (stat((std::string(buff) + "/" + decompilerExeName).data(), &st) == 0) {
			decompilerExePath = std::string(buff) + "/";
			return true;
		}
	}

	return false;
}

bool RdGlobalInfo::isUseThreads() const
{
	return useThreads;
}

void RdGlobalInfo::setIsUseThreads(bool f)
{
	useThreads = f;
}

/**
 * @return @c True if canceled, @c false otherwise.
 */
bool RdGlobalInfo::configureDecompilation()
{
#if defined(OS_WINDOWS)
	decompilerExePath = ghidraPath + "/Ghidra/Features/Decompiler/os/win64/";
#else
#if defined(OS_LINUX)
	decompilerExePath = ghidraPath + "/Ghidra/Features/Decompiler/os/linux64/";
#else
	decompilerExePath = ghidraPath + "/Ghidra/Features/Decompiler/os/osx64/";
#endif
#endif
	if (isDecompilerInSpecifiedPath())
	{
		INFO_MSG("Found " << decompilerExeName << " at " << decompilerExePath
				<< " -> plugin is properly configured.\n");
		//decompilationCmd = decompilerExePath;
		return false;
	}
	else if (isDecompilerInSystemPath())
	{
		INFO_MSG("Found " << decompilerExeName << " at system PATH "
				<< decompilerExePath << " -> plugin is properly configured.\n");
		//decompilationCmd = decompilerExePath;
		return false;
	}
	else
	{
		WARNING_GUI("Decompilation is not properly configured.\n"
				"The path to " << decompilerExeName << " must be provided in the configuration menu.");
		auto canceled = pluginConfigurationMenu(*this);
		if (canceled)
		{
			return canceled;
		}
		else
		{
			return configureDecompilation();
		}
	}
}

} // namespace idaplugin
