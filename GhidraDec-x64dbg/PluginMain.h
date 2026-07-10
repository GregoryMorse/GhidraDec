#ifndef GHIDRADEC_X64DBG_PLUGINMAIN_H
#define GHIDRADEC_X64DBG_PLUGINMAIN_H

#include <windows.h>
#include <_plugins.h>

namespace Plugin
{
	extern int handle;
	extern HWND hwndDlg;
	extern int hMenu;
	extern int hMenuDisasm;
	extern int hMenuDump;
	extern int hMenuStack;
	extern int hMenuGraph;
	extern int hMenuMemmap;
	extern int hMenuSymmod;
}

#endif
