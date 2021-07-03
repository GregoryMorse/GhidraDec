// GhidraDecIface.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// Root include for parsing using SLEIGH
#include "loadimage.hh"
#include "sleigh.hh"
#include "emulate.hh"

#include "types.h"
#include <iostream>
#include <memory>
#include "sleighinterface.h"

// This is a tiny LoadImage class which feeds the executable bytes to the translator
class MyLoadImage : public LoadImage {
	uintb baseaddr;
	int4 length;
	uint1* data;
public:
	MyLoadImage(uintb ad, uint1* ptr, int4 sz) : LoadImage("nofile") { baseaddr = ad; data = ptr; length = sz; }
	virtual void loadFill(uint1* ptr, int4 size, const Address& addr);
	virtual string getArchType(void) const { return "unknown"; }
	virtual void adjustVma(long adjust) { }
};

// This is the only important method for the LoadImage. It returns bytes from the static array
// depending on the address range requested
void MyLoadImage::loadFill(uint1* ptr, int4 size, const Address& addr)

{
	uintb start = addr.getOffset();
	uintb max = baseaddr + (length - 1);
	for (int4 i = 0; i < size; ++i) {	// For every byte requestes
		uintb curoff = start + i; // Calculate offset of byte
		if ((curoff < baseaddr) || (curoff > max)) {	// If byte does not fall in window
			ptr[i] = 0;		// return 0
			continue;
		}
		uintb diff = curoff - baseaddr;
		ptr[i] = data[(int4)diff];	// Otherwise return data from our window
	}
}

// -------------------------------
//
// These are the classes/routines relevant to doing disassembly 

// Here is a simple class for emitting assembly.  In this case, we send the strings straight
// to standard out.
class AssemblyRaw : public AssemblyEmit {
public:
	virtual void dump(const Address& addr, const string& mnem, const string& body) {
		addr.printRaw(cout);
		cout << ": " << mnem << ' ' << body << endl;
	}
};
#define getDefaultSpace getDefaultCodeSpace //Ghidra 10
static void dumpAssembly(Translate& trans, uintb startaddr, uintb endaddr)

{ // Print disassembly of binary code
	AssemblyRaw assememit;	// Set up the disassembly dumper
	int4 length;			// Number of bytes of each machine instruction

	Address addr(trans.getDefaultSpace(), startaddr); // First disassembly address
	Address lastaddr(trans.getDefaultSpace(), endaddr); // Last disassembly address

	while (addr < lastaddr) {
		length = trans.printAssembly(assememit, addr);
		addr = addr + length;
	}
}

// -------------------------------
//
// These are the classes/routines relevant to printing a pcode translation

// Here is a simple class for emitting pcode. We simply dump an appropriate string representation
// straight to standard out.
class PcodeRawOut : public PcodeEmit {
public:
	virtual void dump(const Address& addr, OpCode opc, VarnodeData* outvar, VarnodeData* vars, int4 isize);
	std::string packedPcodes;
};

static void print_vardata(ostream& s, VarnodeData& data)
{
	s << '(' << data.space->getName() << ',';
	data.space->printOffset(s, data.offset);
	s << ',' << dec << data.size << ')';
}

void PcodeRawOut::dump(const Address& addr, OpCode opc, VarnodeData* outvar, VarnodeData* vars, int4 isize)
{
	if (outvar != (VarnodeData*)0) {
		print_vardata(cout, *outvar);
		cout << " = ";
	}
	cout << get_opname(opc);
	// Possibly check for a code reference or a space reference
	for (int4 i = 0; i < isize; ++i) {
		cout << ' ';
		print_vardata(cout, vars[i]);
	}
	cout << endl;
}

static void dumpPcode(Translate& trans, uintb startaddr, uintb endaddr)

{ // Dump pcode translation of machine instructions
	PcodeRawOut emit;		// Set up the pcode dumper
	AssemblyRaw assememit;	// Set up the disassembly dumper
	int4 length;			// Number of bytes of each machine instruction

	Address addr(trans.getDefaultSpace(), startaddr); // First address to translate
	Address lastaddr(trans.getDefaultSpace(), endaddr); // Last address

	while (addr < lastaddr) {
		cout << "--- ";
		trans.printAssembly(assememit, addr);
		length = trans.oneInstruction(emit, addr); // Translate instruction
		addr = addr + length;		// Advance to next instruction
	}

	//if a failure occurs:
	//packed += unimpl_tag;
	//packed += dumpOffset(fallLength);

}

/*
// -------------------------------------
//
// These are the classes/routines relevant for emulating the executable

// A simple class for emulating the system "puts" call.
// It justs looks up the string data and dumps it to standard out.
class PutsCallBack : public BreakCallBack {
public:
	virtual bool addressCallback(const Address& addr);
};

bool PutsCallBack::addressCallback(const Address& addr)

{
	MemoryState* mem = static_cast<EmulateMemory*>(emulate)->getMemoryState();
	uint1 buffer[256];
	uint4 esp = mem->getValue("ESP");
	AddrSpace* ram = mem->getTranslate()->getSpaceByName("ram");

	uint4 param1 = mem->getValue(ram, esp + 4, 4);
	mem->getChunk(buffer, ram, param1, 255);

	cout << (char*)& buffer << endl;

	uint4 returnaddr = mem->getValue(ram, esp, 4);
	mem->setValue("ESP", esp + 8);
	emulate->setExecuteAddress(Address(ram, returnaddr));

	return true;			// This replaces the indicated instruction
}

// A simple class for emulating the system "printf" call.
// We don't really emulate all of it.  The only printf call in the example
// has an initial string of "%d\n". So we grab the second parameter from the
// memory state and print it as an integer
class PrintfCallBack : public BreakCallBack {
public:
	virtual bool addressCallback(const Address& addr);
};

bool PrintfCallBack::addressCallback(const Address& addr)

{
	MemoryState* mem = static_cast<EmulateMemory*>(emulate)->getMemoryState();

	AddrSpace* ram = mem->getTranslate()->getSpaceByName("ram");

	uint4 esp = mem->getValue("ESP");
	uint4 param2 = mem->getValue(ram, esp + 8, 4);
	cout << (int4)param2 << endl;

	uint4 returnaddr = mem->getValue(ram, esp, 4);
	mem->setValue("ESP", esp + 12);
	emulate->setExecuteAddress(Address(ram, returnaddr));

	return true;
}

// A callback that terminates the emulation
class TerminateCallBack : public BreakCallBack {
public:
	virtual bool addressCallback(const Address& addr);
};

bool TerminateCallBack::addressCallback(const Address& addr)

{
	emulate->setHalt(true);

	return true;
}

static void doEmulation(Translate& trans, LoadImage& loader)

{
	// Set up memory state object
	MemoryImage loadmemory(trans.getDefaultSpace(), 8, 4096, &loader);
	MemoryPageOverlay ramstate(trans.getDefaultSpace(), 8, 4096, &loadmemory);
	MemoryHashOverlay registerstate(trans.getSpaceByName("register"), 8, 4096, 4096, (MemoryBank*)0);
	MemoryHashOverlay tmpstate(trans.getUniqueSpace(), 8, 4096, 4096, (MemoryBank*)0);

	MemoryState memstate(&trans);	// Instantiate the memory state object
	memstate.setMemoryBank(&ramstate);
	memstate.setMemoryBank(&registerstate);
	memstate.setMemoryBank(&tmpstate);

	BreakTableCallBack breaktable(&trans); // Set up the callback object
	EmulatePcodeCache emulater(&trans, &memstate, &breaktable); // Set up the emulator

	// Set up the initial register state for execution
	memstate.setValue("ESP", 0xbffffffc);
	emulater.setExecuteAddress(Address(trans.getDefaultSpace(), 0x80483b4));

	// Register callbacks
	PutsCallBack putscallback;
	PrintfCallBack printfcallback;
	TerminateCallBack terminatecallback;
	breaktable.registerAddressCallback(Address(trans.getDefaultSpace(), 0x80482c8), &putscallback);
	breaktable.registerAddressCallback(Address(trans.getDefaultSpace(), 0x80482b8), &printfcallback);
	breaktable.registerAddressCallback(Address(trans.getDefaultSpace(), 0x804846b), &terminatecallback);

	emulater.setHalt(false);

	do {
		emulater.executeInstruction();
	} while (!emulater.getHalt());
}*/

void doSleigh(int action, string sleighfilename, string pspec, uintb baseAddr, uint1* prog, uintb size, uintb dumpsize) //0 for disassemble, 1 for pcode, 2 for emulate
{
	// Set up the loadimage
	MyLoadImage loader(baseAddr, prog, size);
	//  loader->open();
	//    loader->adjustVma(adjustvma);
	// Set up the context object
	ContextInternal context;
	// Set up the assembler/pcode-translator
	Sleigh trans(&loader, &context);
	// Read sleigh file into DOM
	DocumentStorage docstorage;
	Element* sleighroot = docstorage.openDocument(sleighfilename)->getRoot();
	docstorage.registerTag(sleighroot);
	trans.initialize(docstorage); // Initialize the translator
	// Now that context symbol names are loaded by the translator
	// we can set the default context
	//Ghidra/Processors/x86/data/languages/x86-64.pspec context_set:
	ifstream s(pspec);
	Document* doc = xml_tree(s);
	s.close();
	Element* el = doc->getRoot();
	const List& list(el->getChildren());
	List::const_iterator iter;
	for (iter = list.begin(); iter != list.end(); ++iter) {
		el = *iter;
		if (el->getName() == "context_data") {
			const List& lst(el->getChildren());
			for (iter = lst.begin(); iter != lst.end(); ++iter) {
				el = *iter;
				if (el->getName() == "context_set") {
					const List& lt(el->getChildren());
					for (iter = lt.begin(); iter != lt.end(); ++iter) {
						el = *iter;
						if (el->getName() == "set") {
							context.setVariableDefault(el->getAttributeValue("name"), strtol(el->getAttributeValue("val").c_str(), nullptr, 10));
						}
					}
					break;
				}
			}
			break;
		}
	}
	/*context.setVariableDefault("addrsize", 2); // Address size is 32-bit
	context.setVariableDefault("bit64", 1); // sizes are 64-bit
	context.setVariableDefault("opsize", 1); // Operand size is 32-bit
	*/
	if (action == 0)
		dumpAssembly(trans, baseAddr, baseAddr + dumpsize);
	else if (action == 1)
		dumpPcode(trans, baseAddr, baseAddr + dumpsize);
	//else if (action == 2)
		//doEmulation(trans, loader);
	else
		throw "Unknown action: " + action;
}

// These are the bytes for an example x86 binary
// These bytes are loaded at address 0x80483b4
static uint1 myprog[] = {
  0x8d, 0x4c, 0x24, 0x04, 0x83, 0xe4, 0xf0, 0xff, 0x71, 0xfc, 0x55,
  0x89, 0xe5, 0x51, 0x81, 0xec, 0xb4, 0x01, 0x00, 0x00, 0xc7, 0x45, 0xf4,
  0x00, 0x00, 0x00, 0x00, 0xeb, 0x12, 0x8b, 0x45, 0xf4, 0xc7, 0x84,
  0x85, 0x64, 0xfe, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x83, 0x45, 0xf4,
  0x01, 0x83, 0x7d, 0xf4, 0x63, 0x7e, 0xe8, 0xc7, 0x45, 0xf4, 0x02,
  0x00, 0x00, 0x00, 0xeb, 0x28, 0x8b, 0x45, 0xf4, 0x01, 0xc0, 0x89, 0x45,
  0xf8, 0xeb, 0x14, 0x8b, 0x45, 0xf8, 0xc7, 0x84, 0x85, 0x64, 0xfe,
  0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x8b, 0x45, 0xf4, 0x01, 0x45, 0xf8,
  0x83, 0x7d, 0xf8, 0x63, 0x7e, 0xe6, 0x83, 0x45, 0xf4, 0x01, 0x83,
  0x7d, 0xf4, 0x31, 0x7e, 0xd2, 0xc7, 0x04, 0x24, 0x40, 0x85, 0x04, 0x08,
  0xe8, 0x9c, 0xfe, 0xff, 0xff, 0xc7, 0x45, 0xf4, 0x02, 0x00, 0x00,
  0x00, 0xeb, 0x25, 0x8b, 0x45, 0xf4, 0x8b, 0x84, 0x85, 0x64, 0xfe, 0xff,
  0xff, 0x85, 0xc0, 0x75, 0x13, 0x8b, 0x45, 0xf4, 0x89, 0x44, 0x24,
  0x04, 0xc7, 0x04, 0x24, 0x47, 0x85, 0x04, 0x08, 0xe8, 0x62, 0xfe, 0xff,
  0xff, 0x83, 0x45, 0xf4, 0x01, 0x83, 0x7d, 0xf4, 0x63, 0x7e, 0xd5,
  0x81, 0xc4, 0xb4, 0x01, 0x00, 0x00, 0x59, 0x5d, 0x8d, 0x61, 0xfc, 0xc3,
  0x90, 0x90, 0x90, 0x90, 0x55, 0x89, 0xe5, 0x5d, 0xc3, 0x8d, 0x74,
  0x26, 0x00, 0x8d, 0xbc, 0x27, 0x00, 0x00, 0x00, 0x00, 0x55, 0x89, 0xe5,
  0x57, 0x56, 0x53, 0xe8, 0x5e, 0x00, 0x00, 0x00, 0x81, 0xc3, 0xa5,
  0x11, 0x00, 0x00, 0x83, 0xec, 0x1c, 0xe8, 0xd7, 0xfd, 0xff, 0xff, 0x8d,
  0x83, 0x20, 0xff, 0xff, 0xff, 0x89, 0x45, 0xf0, 0x8d, 0x83, 0x20,
  0xff, 0xff, 0xff, 0x29, 0x45, 0xf0, 0xc1, 0x7d, 0xf0, 0x02, 0x8b, 0x55,
  0xf0, 0x85, 0xd2, 0x74, 0x2b, 0x31, 0xff, 0x89, 0xc6, 0x8d, 0xb6,
  0x00, 0x00, 0x00, 0x00, 0x8b, 0x45, 0x10, 0x83, 0xc7, 0x01, 0x89, 0x44,
  0x24, 0x08, 0x8b, 0x45, 0x0c, 0x89, 0x44, 0x24, 0x04, 0x8b, 0x45,
  0x08, 0x89, 0x04, 0x24, 0xff, 0x16, 0x83, 0xc6, 0x04, 0x39, 0x7d, 0xf0,
  0x75, 0xdf, 0x83, 0xc4, 0x1c, 0x5b, 0x5e, 0x5f, 0x5d, 0xc3, 0x8b,
  0x1c, 0x24, 0xc3, 0x90, 0x90, 0x90, 0x55, 0x89, 0xe5, 0x53, 0xbb, 0x50,
  0x95, 0x04, 0x08, 0x83, 0xec, 0x04, 0xa1, 0x50, 0x95, 0x04, 0x08,
  0x83, 0xf8, 0xff, 0x74, 0x0c, 0x83, 0xeb, 0x04, 0xff, 0xd0, 0x8b, 0x03,
  0x83, 0xf8, 0xff, 0x75, 0xf4, 0x83, 0xc4, 0x04, 0x5b, 0x5d, 0xc3,
  0x55, 0x89, 0xe5, 0x53, 0x83, 0xec, 0x04, 0xe8, 0x00, 0x00, 0x00, 0x00,
  0x5b, 0x81, 0xc3, 0x0c, 0x11, 0x00, 0x00, 0xe8, 0x00, 0xfe, 0xff,
  0xff, 0x59, 0x5b, 0xc9, 0xc3, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x72, 0x69, 0x6d, 0x65, 0x73,
  0x00, 0x25, 0x64, 0x0a, 0x00, 0x00
};  // Size of 408 bytes

uint1 probe[] = { 0x40, 0x53 };
const char result[] = R"(!"`"0 !  %`"!%! 2"`(%#8`(`"4%#@`(%#@`(% (`(`"##$"%#@`(%! 2"`(``)";
uint1 probe2[] = { 0x48, 0x83, 0xec, 0x40 };
const char result2[] = R"(!$`"2 !  %`"/%# (`!%#@`(%  !`(`"7%#+(`!%#@`(%  !`(`"4%#@`(%#@`(%  !`(`"-%#'(`!%#@`(% `(`"+%#&(`!%#@`(% `(``)";
uint1 probe3[] = { 0x4c, 0x8b, 0x94, 0x24, 0x80, 0x00, 0x00, 0x00 };
const char result3[] = R"(!(`"6 !  %`"3%!0?`(%  "`(%#@`(`""%!P]!`($"%!0?`(`"!%#0"`(%!P]!`(``)";
uint1 probe4[] = { 0x74, 0x5c };
const char result4[] = R"(!"`"D !  %`"%#%"""!  %`(%#&(`!``)";

#ifdef _WINDOWS
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&& ... args) {
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#define _pipe(x,y,z) pipe2(x,z)
#define _dup dup
#define _dup2 dup2
#define _close close
#define _write write
#define _read read
#define STDOUT_FILENO 1
#define STDIN_FILENO 0
#define _O_NOINHERIT O_CLOEXEC
#define _O_BINARY 0
typedef unsigned long long DWORD;
#endif
#include <fcntl.h>

#define WARNING_GUI(x) 0;

class MyCallback : public DecompileCallback
{
public:
	std::string ghidraPath;
	intptr_t decompPid = 0;
	int rdHandle = -1;
	int wrHandle = -1;
	virtual ~MyCallback()
	{
		if (rdHandle != -1) {
			_close(rdHandle); rdHandle = -1;
		}
		if (wrHandle != -1) {
			_close(wrHandle); wrHandle = -1;
		}
	}
	void launchDecompiler()
	{
#if defined(_WINDOWS)
		std::string cmd = ghidraPath + "/Ghidra/Features/Decompiler/os/win64/" "decompile.exe";
		SECURITY_ATTRIBUTES saAttr; // Set the bInheritHandle flag so pipe handles are inherited. 
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
		STARTUPINFOA si = { sizeof(si) };
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdInput = g_hChildStd_IN_Rd;
		si.hStdOutput = g_hChildStd_OUT_Wr;
		si.hStdError = g_hChildStd_OUT_Wr;
		wrHandle = _open_osfhandle((intptr_t)g_hChildStd_IN_Wr, _O_WRONLY);
		rdHandle = _open_osfhandle((intptr_t)g_hChildStd_OUT_Rd, _O_RDONLY);
		PROCESS_INFORMATION pi{};
#else
#if defined(OS_LINUX)
		std::string cmd = ghidraPath + "/Ghidra/Features/Decompiler/os/linux64/" "decompile";
#else
		std::string cmd = ghidraPath + "/Ghidra/Features/Decompiler/os/osx64/" "decompile";
#endif
		int rpipefd[2], wpipefd[2];
		if (_pipe(rpipefd, 4096, _O_BINARY | _O_NOINHERIT) < 0) {
			WARNING_GUI("Pipe creation failed");
		}
		if (_pipe(wpipefd, 4096, _O_BINARY | _O_NOINHERIT) < 0) {
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
		*handleIn = rpipefd[0];
		*handleOut = wpipefd[1];
#endif
		void* p;
#if defined(_WINDOWS)
		CreateProcessA(cmd.c_str(), NULL, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		p = pi.hProcess;
		CloseHandle(g_hChildStd_IN_Rd);
		//CloseHandle(g_hChildStd_IN_Wr);
		//CloseHandle(g_hChildStd_OUT_Rd);
		CloseHandle(g_hChildStd_OUT_Wr);
#else
		//_wspawnvp(cmd.c_str(), NULL);
		if (_dup2(fdStdOut, STDOUT_FILENO) != 0) WARNING_GUI("Pipe handle duplicatoin restoration failed");
		if (_dup2(fdStdIn, STDIN_FILENO) != 0) WARNING_GUI("Pipe handle duplication restoration failed");;
		_close(fdStdOut);
		_close(fdStdIn);
#endif
		if (p == nullptr)
		{
			if (rdHandle != -1) {
				_close(rdHandle); rdHandle = -1;
			}
			if (wrHandle != -1) {
				_close(wrHandle); wrHandle = -1;
			}
			//if (showWarnings)
			{
				WARNING_GUI("launch_process(" << procInf.path << " "
					<< procInf.args << ") failed to launch " << errbuf.c_str()
					<< "\n");
			}
			return;
		}
		//if (decompPid)
		{
#if defined(_WINDOWS)
			decompPid = pi.dwProcessId;
			if (pi.hThread != INVALID_HANDLE_VALUE) CloseHandle(pi.hThread);
#else // Linux || macOS
			decompPid = reinterpret_cast<intptr_t>(p);
#endif
		}

		DWORD rc = 0;
		DWORD cpe;
#if defined(_WINDOWS)
		cpe = WaitForSingleObject(pi.hProcess, 0);
		if (cpe == WAIT_OBJECT_0) rc = GetExitCodeProcess(pi.hProcess, &cpe);
		else if (cpe == WAIT_TIMEOUT) cpe = 0;
#endif
		//if (decompPid)
		{
			decompPid = 0;
		}
		if (cpe != 0)
		{
			if (rdHandle != -1) {
				_close(rdHandle); rdHandle = -1;
			}
			if (wrHandle != -1) {
				_close(wrHandle); wrHandle = -1;
			}
			//if (showWarnings)
			{
				WARNING_GUI("Error in check_process_exit() while executing: "
					<< procInf.path << " " << procInf.args << "\n");
			}
			return;
		}

		if (rc != 0)
		{
			if (rdHandle != -1) {
				_close(rdHandle); rdHandle = -1;
			}
			if (wrHandle != -1) {
				_close(wrHandle); wrHandle = -1;
			}
			//if (showWarnings)
			{
				WARNING_GUI("launch_process(" << procInf.path << " " << procInf.args
					<< ") failed with error code " << rc << "\n");
			}
			return;
		}
	}
	size_t readDec(void* Buf, size_t MaxCharCount)
	{
		return _read(rdHandle, Buf, MaxCharCount);
	}
	size_t writeDec(void const* Buf, size_t MaxCharCount)
	{
		return _write(wrHandle, Buf, MaxCharCount);
	}
	void terminate()
	{
	}
	void getInits(std::vector<InitStateItem>& inits)
	{
	}
	int getBytes(unsigned char* ptr, int size, AddrInfo addr)
	{
		for (int i = 0; i < size; i++) {
			ptr[i] = myprog[addr.offset - 0x80483b4 + i];
		}
		return size;
	}
	std::string emit(std::string type, std::string color, std::string str)
	{
		return str;
	}
	void getMappedSymbol(AddrInfo addr, MappedSymbolInfo& msi)
	{
	}
	void getExternInfo(AddrInfo addr, std::string& callName, std::string& modName, FuncProtoInfo& func)
	{
	}
	void getMetaType(std::string typeName, std::vector<TypeInfo>& typeChain)
	{
	}
	void getComments(AddrInfo addr, std::vector<CommentInfo>& comments)
	{
	}
	std::string getSymbol(AddrInfo addr)
	{
		return "";
	}
	std::string getPcodeInject(int type, std::string name, AddrInfo addr, std::string fixupbase, unsigned long long fixupoffset)
	{
		return "";
	}
	void getCPoolRef(const std::vector<unsigned long long>& refs, CPoolRecord& rec)
	{
	}
};

int __cdecl main()
{
	std::string ghidraPath;
#ifdef _WINDOWS
	char buf[256];
	GetEnvironmentVariableA("USERPROFILE", buf, 256);
	ghidraPath = std::string(buf) + "/Documents/ghidra_9.0.4/";
#endif
	std::vector<LangInfo> li;
	std::map<std::string, std::vector<int>> toolMap;
	DecompInterface::getLangFiles(ghidraPath + "Ghidra/Processors/", "IDA-PRO", toolMap, li);
	std::unique_ptr<ContextInternal> context;
	std::unique_ptr<Sleigh> trans;
	context = make_unique<ContextInternal>();
	MyLoadImage loader(0x80483b4, myprog, 408);
	trans = make_unique<Sleigh>(&loader, context.get());
	ifstream ss(ghidraPath + "Ghidra/Processors/x86/data/languages/x86-16.pspec");
	Document* doc = xml_tree(ss);
	ss.close();
	Element* el = doc->getRoot();
	const List& list(el->getChildren());
	List::const_iterator iter;
	for (iter = list.begin(); iter != list.end(); ++iter) {
		el = *iter;
		if (el->getName() == "context_data") {
			context->restoreFromSpec(el, trans.get());
			break;
		}
	}
	delete doc;
	//doSleigh(0, ghidraPath + "Ghidra/Processors/x86/data/languages/x86.sla", 0x80483b4, myprog, 408, 184);
	//doSleigh(1, ghidraPath + "Ghidra/Processors/x86/data/languages/x86.sla", 0x80483b4, myprog, 408, 11);
	//doSleigh(0, ghidraPath + "Ghidra/Processors/x86/data/languages/x86-64.sla", );
	//doSleigh(1, ghidraPath + "Ghidra/Processors/x86/data/languages/x86-64.sla", ghidraPath + "Ghidra/Processors/x86/data/languages/x86-64.pspec", 0x140001010, probe, sizeof(probe), sizeof(probe));
	//doSleigh(1, ghidraPath + "Ghidra/Processors/x86/data/languages/x86-64.sla", ghidraPath + "Ghidra/Processors/x86/data/languages/x86-64.pspec", 0x140001012, probe2, sizeof(probe2), sizeof(probe2));
	//doSleigh(1, ghidraPath + "Ghidra/Processors/x86/data/languages/x86-64.sla", ghidraPath + "Ghidra/Processors/x86/data/languages/x86-64.pspec", 0x140001016, probe3, sizeof(probe3), sizeof(probe3));
	//doSleigh(1, ghidraPath + "Ghidra/Processors/x86/data/languages/x86-64.sla", ghidraPath + "Ghidra/Processors/x86/data/languages/x86-64.pspec", 0x140001024, probe4, sizeof(probe4), sizeof(probe4));
	MyCallback* cb = new MyCallback();
	cb->ghidraPath = ghidraPath;
	std::string display, funcProto, funcColorProto;
	FuncProtoInfo paramInfo = {};
	std::vector<std::tuple<std::vector<unsigned int>, std::string, unsigned int>> blockGraph;
	int timeout = 30, maxpayload = 50;
	DecompInterface di;
	std::vector<CoreType> cts(&defaultCoreTypes[0], &defaultCoreTypes[numDefCoreTypes]);
	di.setup(cb, ghidraPath + "Ghidra/Processors/x86/data/languages/x86-64.sla", ghidraPath + "Ghidra/Processors/x86/data/languages/x86-64.pspec",
		ghidraPath + "Ghidra/Processors/x86/data/languages/x86-64-win.cspec", cts, defaultOptions, timeout, maxpayload);
	cout << di.doDecompile(defaultDecMode, AddrInfo{ "ram", 0x80483b4 }, display, funcProto, funcColorProto, paramInfo, blockGraph);
	//cout << di.doDecompile(cb, ghidraPath + "Ghidra/Processors/x86/data/languages/x86-64.sla", ghidraPath + "Ghidra/Processors/x86/data/languages/x86-64.pspec",
	//	ghidraPath + "Ghidra/Processors/x86/data/languages/x86-64-gcc.cspec", std::vector<CoreType>(&defaultCoreTypes[0], &defaultCoreTypes[numDefCoreTypes]), defaultOptions, defaultDecMode, 0x80483b4);
	di.setup(cb, ghidraPath + "Ghidra/Processors/x86/data/languages/x86.sla", ghidraPath + "Ghidra/Processors/x86/data/languages/x86.pspec",
		ghidraPath + "Ghidra/Processors/x86/data/languages/x86gcc.cspec", cts, defaultOptions, timeout, maxpayload);
	cout << di.doDecompile(defaultDecMode, AddrInfo{ "ram", 0x80483b4 }, display, funcProto, funcColorProto, paramInfo, blockGraph);
	delete cb;
}