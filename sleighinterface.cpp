//Java has its own entire Sleigh library implementation used by Ghidra UI app:
//Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/program/model/lang
//In C the equivalent is building a custom one starting with:
//Ghidra/Features/Decompiler/src/decompile/cpp/sleighexample.cc
//Requires source:
//SLEIGH=sleigh pcodeparse pcodecompile sleighbase slghsymbol
//  slghpatexpress slghpattern semantics context filemanage
//CORE=	xml space float address pcoderaw translate opcodes globalcontext
//LIBSLA_NAMES=$(CORE) $(SLEIGH) loadimage sleigh memstate emulate opbehavior
//also types.h error.hh partmap.hh
//address.cc conflicts with address.cpp of retdec so rename it to address_.cpp
//pcodeparse.y and xml.y require: https://sourceforge.net/projects/winflexbison/
//Project->Build Dependencies->Build Customizations... Find Targets win_flex_bison\custom_build_rules\win_flex_bison_custom_build.targets
//copy win_bison.exe and data folder to Project folder and add *.y files as top level items
//pcodeparsetab.cpp and xml.tab.cpp conflict requires -p zz bison option to change yy to zz in one of them for:
//functions yylex, yyerror, yyparse, and variables yychar, yylval, yynerrs
//
//sleigh.exe meanwhile merely compiles raw specification files .slaspec into .sla compiled specification files
//  it appears to be unused in the Ghidra UI app
//JVM/Dalvik - C Pool Ref and 3 Pcode injection calls dynamic implementation needs to be ripped from Java code
//Building decompiler: need GNU BFD (binary file descriptor library) - https://sourceware.org/binutils/

#define _CRT_SECURE_NO_WARNINGS

// Dump the raw pcode instructions

// Root include for parsing using SLEIGH
#include "loadimage.hh"
#include "sleigh.hh"
#include "emulate.hh"
#include "pcodeparse.hh"
#include "filemanage.hh"

//g++ -std=c++1y -m64 -I include -I . -D__IDP__ -D__PLUGIN__ -DNO_OBSOLETE_FUNCS -D__X64__ -D__LINUX__ -fpermissive sleighinterface.cpp
#include <iostream>
#include <memory>
#include <thread>
#include <future>
#include <stack>

#include "sleighinterface.h"

//typeop.cc
//ZEXT#(I)_#(O) - cast to unsigned #(O) size
//SEXT#(I)_#(O) - cast to signed #(O) size
//#define CONCAT(SZI1, SZI2, VAL1, VAL2) (((VAL1) << (SZI2 * 8)) | (VAL2))
//CONCAT#(I1)_#(I2) - shift left/or idiom
//#define SUB(SZI, SZO, VAL) (((VAL) >> (SZ1 * 8)) & ((~0ull) >> (64 - SZO * 8)))
//SUB#(I)_#(O) - shift right/and idiom, careful that 64 bits does not get truncated
//#define CARRY(SZ, VAL1, VAL2) (((((1ull << (SZ * 8 - 1)) & (VAL1)) != 0) && (((1ull << (SZ * 8 - 1)) & (VAL2)) != 0)) || (((1ull << (SZ * 8 - 1)) & (VAL1)) ^ ((1ull << (SZ * 8 - 1)) & (VAL2))) != 0 && (((VAL1) + (VAL2)) & (1ull << (SZ * 8 - 1))) == 0)
//CARRY#(I) - addition comparison idiom - both high bits are set or one high bit is set and their addition causes the high bit to clear 0 1 0/1 0 0/1 1 _
//#define SCARRY(SZ, VAL1, VAL2) (((1ull << (SZ * 8 - 1)) & (VAL1)) == ((1ull << (SZ * 8 - 1)) & (VAL2)) && (((VAL1) + (VAL2)) & (1ull << (SZ * 8 - 1))) != ((1ull << (SZ * 8 - 1)) & (VAL1)))
//SCARRY#(I) - signed addition comparison idiom - both are positive or both are negative and their addition causes the high bit to be opposite of inputs 0 0 1/1 1 0
//#define SBORROW(SZ, VAL1, VAL2) (((1ull << (SZ * 8 - 1)) & (VAL1)) != ((1ull << (SZ * 8 - 1)) & (VAL2)) && (((VAL1) - (VAL2)) & (1ull << (SZ * 8 - 1))) != ((1ull << (SZ * 8 - 1)) & (VAL1)))
//SBORROW#(I) - signed subtraction comparison idiom - one is negative and one is positive and their subtraction causes the high bit to be opposite first input/same as second input 0 1 1/1 0 0
//NAN - needs math.h along with ABS, SQRT, CEIL, FLOOR, ROUND
//ABS - fabs, fabsf, fabsl
//SQRT - sqrtf, sqrt, sqrtl
//CEIL - ceil, ceilf, ceill
//FLOOR - floor, floorf, floorl
//ROUND - round, roundf, roundl

const char* szExtensionMacros[] = { "ZEXT", "SEXT", //2 numeric values for input size, output size
	"CARRY", "SCARRY", "SBORROW", //1 numeric value for input size
	"NAN", "ABS", "SQRT",
	/*"INT2FLOAT", "FLOAT2FLOAT", "TRUNC",*/ //handled with type casts in printc.cc
	"CEIL", "FLOOR", "ROUND",
	"CONCAT", //2 numeric values for 1st input size, 2nd input size
	"SUB" //2 numeric values for input size, output size
};

const Options defaultOptions = { true, true, true, true, false, true, false, false, true, false,
	100, 2, 20, std::string("c"), false, true, false, false, true, true, true,
	std::string("best"), std::string("c-language") };
const DecMode defaultDecMode = { std::string("decompile"), true, true, false, false };

// Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/program/model/data/ *DataType.java
// Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/program/model/pcode/PcodeDataTypeManager.java
// Ghidra/Features/Decompiler/src/decompile/cpp/ghidra_arch.cc setCoreType - specifies default if none are sent
//type.cc Datatype::hashName produces the IDs - first name of aliases will be the preferred one currently
CoreType defaultCoreTypes[] = {
	{ std::string("void"), 0, std::string("void") }, //ghidra_arch.cc

	{ std::string("undefined"), 1, std::string("unknown") }, //ghidra_arch.cc
	{ std::string("undefined1"), 1, std::string("unknown") },
	{ std::string("undefined2"), 2, std::string("unknown") }, //ghidra_arch.cc
	{ std::string("undefined3"), 3, std::string("unknown") },
	{ std::string("undefined4"), 4, std::string("unknown") }, //ghidra_arch.cc
	{ std::string("undefined5"), 5, std::string("unknown") },
	{ std::string("undefined6"), 6, std::string("unknown") },
	{ std::string("undefined7"), 7, std::string("unknown") },
	{ std::string("undefined8"), 8, std::string("unknown") }, //ghidra_arch.cc

	{ std::string("sbyte"), 1, std::string("int") }, //ghidra_arch.cc
	{ std::string("short"), 2, std::string("int") },
	{ std::string("sword"), 2, std::string("int") }, //ghidra_arch.cc
	{ std::string("int3"), 3, std::string("int") },
	{ std::string("int"), 4, std::string("int") },
	{ std::string("sdword"), 4, std::string("int") }, //ghidra_arch.cc
	{ std::string("int5"), 5, std::string("int") },
	{ std::string("int6"), 6, std::string("int") },
	{ std::string("int7"), 7, std::string("int") },
	{ std::string("long"), 8, std::string("int") },
	{ std::string("sqword"), 8, std::string("int") }, //ghidra_arch.cc
	{ std::string("longlong"), 8, std::string("int") }, //alias
	{ std::string("int16"), 16, std::string("int") },

	{ std::string("byte"), 1, std::string("uint") }, //ghidra_arch.cc
	{ std::string("ushort"), 2, std::string("uint") },
	{ std::string("word"), 2, std::string("uint") }, //ghidra_arch.cc
	{ std::string("uint3"), 3, std::string("uint") },
	{ std::string("uint"), 4, std::string("uint") },
	{ std::string("dword"), 4, std::string("uint") }, //ghidra_arch.cc
	{ std::string("uint5"), 5, std::string("uint") },
	{ std::string("uint6"), 6, std::string("uint") },
	{ std::string("uint7"), 7, std::string("uint") },
	{ std::string("ulong"), 8, std::string("uint") },
	{ std::string("qword"), 8, std::string("uint") }, //ghidra_arch.cc
	{ std::string("ulonglong"), 8, std::string("uint") }, //alias
	{ std::string("uint16"), 16, std::string("uint") },

	{ std::string("float2"), 2, std::string("float") },
	{ std::string("float"), 4, std::string("float") }, //ghidra_arch.cc
	{ std::string("double"), 8, std::string("float") },
	{ std::string("float8"), 8, std::string("float") }, //ghidra_arch.cc
	{ std::string("float10"), 10, std::string("float") },
	{ std::string("float16"), 16, std::string("float") }, //ghidra_arch.cc
	{ std::string("longdouble"), 16, std::string("float") }, //alias

	{ std::string("code"), 1, std::string("code") }, //ghidra_arch.cc

	{ std::string("char"), sizeof(char), std::string("int"), sizeof(char) == 1, sizeof(char) == 2 }, //metatype=int/uint, size=1 for char/2 for utf  //ghidra_arch.cc

	{ std::string("wchar_t"), sizeof(wchar_t), std::string("int"), false, true }, //size=2/4
	{ std::string("wchar16"), 2, std::string("int"), false, true },
	{ std::string("wchar32"), 4, std::string("int"), false, true },

	{ std::string("bool"), 1, std::string("bool") } //ghidra_arch.cc
};
const int numDefCoreTypes = sizeof(defaultCoreTypes) / sizeof(defaultCoreTypes[0]);

const char* szCoreTypeDefs[] = {
nullptr, "__int8", "__int8", "__int16", "__int32", "__int32", "__int64", "__int64", "__int64", "__int64",
"__int8", nullptr, "__int16", "__int32", nullptr, "__int32", "__int64", "__int64", "__int64", nullptr, "__int64", "__int64", "__int64",
"unsigned __int8", "unsigned __int16" , "unsigned __int16", "unsigned __int32", "unsigned __int32", "unsigned __int32", "unsigned __int64", "unsigned __int64", "unsigned __int64", "unsigned __int64", "unsigned __int64", "unsigned __int64", "unsigned __int64",
"float", nullptr, nullptr, "double", "long double", "long double", "long double",
"void*", nullptr, nullptr, "char16_t", "char32_t", nullptr
};

const char* cpoolreftags[] = { "primitive", "string", "classref", "method",
	"field", "arraylength", "instanceof", "checkcast" };

/// If a type id is explicitly provided for a data-type, this routine is used
/// to produce an id based on a hash of the name.  IDs produced this way will
/// have their sign-bit set to distinguish it from other IDs.
/// \param nm is the type name to be hashed
uint8 hashName(const std::string& nm)
{
	uint8 res = 123;
	for (uint4 i = 0; i < nm.size(); ++i) {
		res = (res << 8) | (res >> 56);
		res += (uint8)nm[i];
		if ((res & 1) == 0)
			res ^= 0xfeabfeab;	// Some kind of feedback
	}
	uint8 tmp = 1;
	tmp <<= 63;
	res |= tmp;	// Make sure the hash is negative (to distinguish it from database id's)
	return res;
}

template <class T>
string to_string(T t, ios_base& (__cdecl *f)(ios_base&))
{
	ostringstream oss;
	oss << f << t;
	return oss.str();
}

// This is a tiny LoadImage class which feeds the executable bytes to the translator
class CallbackLoadImage : public LoadImage {
	DecompileCallback* callback;
public:
	CallbackLoadImage(DecompileCallback* cb) : LoadImage("nofile"), callback(cb) { }
	virtual void loadFill(uint1* ptr, int4 size, const Address& addr);
	virtual string getArchType(void) const { return "unknown"; } //unused
	virtual void adjustVma(long adjust) { } //unused
};

// This is the only important method for the LoadImage. It returns bytes from the static array
// depending on the address range requested
void CallbackLoadImage::loadFill(uint1* ptr, int4 size, const Address& addr)
{
	if (callback->getBytes(ptr, size, addr.getSpace()->getName(), addr.getOffset()) == 0)
		throw DecompError("No bytes could be loaded");
}

// -------------------------------
//
// These are the classes/routines relevant to printing a pcode translation

// Here is a simple class for emitting pcode. We simply dump an appropriate string representation
// straight to standard out.
class PackedPcodeRawOut : public PcodeEmit {
public:
	virtual void dump(const Address& addr, OpCode opc,
		VarnodeData* outvar, VarnodeData* vars, int4 isize);
	std::string packedPcodes;
};

/*uchar unimpl_tag = 0x20, inst_tag = 0x21, op_tag = 0x22, void_tag = 0x23,
	spaceid_tag = 0x24, addrsz_tag = 0x25, end_tag = 0x60;*/

std::string dumpOffset(unsigned long long val) {
	std::string packed;
	while (val != 0) {
		uchar chunk = (int)(val & 0x3f);
		val >>= 6;
		packed += (chunk + 0x20);
	}
	packed += PcodeEmit::end_tag;
	return packed;
}

std::string dumpVarnodeData(VarnodeData* v) {
	std::string packed;
	packed += PcodeEmit::addrsz_tag;
	int spcindex = v->space->getIndex();
	packed += (spcindex + 0x20);
	packed += dumpOffset(v->offset);
	packed += (v->size + 0x20);
	return packed;
}

//const int ID_UNIQUE_SHIFT = 7;

std::string dumpSpaceId(VarnodeData* v) {
	std::string packed;
	packed += PcodeEmit::spaceid_tag;
	//int spcindex = ((int)v->offset >> ID_UNIQUE_SHIFT);
	//v->getAddr()->getSpaceFromConst()->getIndex();
	int spcindex = ((AddrSpace*)v->offset)->getIndex();
	packed += (spcindex + 0x20);
	return packed;
}

/*struct LabelRef
{
	int opIndex;		// Index of operation referencing the label
	int labelIndex;	// Index of label being referenced
	int labelSize;	// Number of bytes in the label
	int streampos;	// Position in byte stream where label is getting encoded
};*/

//std::vector<LabelRef> labelrefs;
//int numOps = 0;
//int labelBase = 0;
//int labelCount = 0;
//std::vector<int> labeldefs;

void PackedPcodeRawOut::dump(const Address& addr, OpCode opc,
	VarnodeData* outvar, VarnodeData* vars, int4 isize)
{
	//int oldbase = labelBase;
	//labelBase = labelCount;
	//labelCount += construct.getNumLabels();
	//PcodeCacher/PcodeBuilder seems to be already resolving all of this in C - so these issues probably never occur
	/*if (opc == CPUI_MULTIEQUAL) { throw DecompError("CPUI_MULTIEQUAL"); }
	else if (opc == CPUI_INDIRECT) { throw DecompError("CPUI_INDIRECT"); }
	else if (opc == CPUI_PTRADD) {
		throw DecompError("CPUI_PTRADD");
		int labelindex = (int)vars[0].offset + labelBase;
		while (labeldefs.size() <= labelindex) {
			labeldefs.push_back(-1);
		}
		labeldefs[labelindex] = numOps;
	}
	else if (opc == CPUI_PTRSUB) { throw DecompError("CPUI_PTRSUB"); }*/
	// Some spaces are "virtual", like the stack spaces, where addresses are really relative to a
	// base pointer stored in a register, like the stackpointer.  This routine will return non-zero
	// if \b this space is virtual and there is 1 (or more) associated pointer registers
	if ((isize > 0) && (vars[0].space->numSpacebase() != 0)) {
		//int labelIndex = (int)vars[0].offset + labelBase;
		//int labelSize = vars[0].size;
		//labelrefs.push_back(LabelRef{ numOps, labelIndex, labelSize, (int)packedPcodes.size() });
		// Force the emitter to write out a maximum length encoding (12 bytes) of a long
		// so that we have space to insert whatever value we need to when this relative is resolved
		vars[0].offset = -1;
	}
	//numOps++;
	//labelBase = oldbase;
	std::string packed;
	packed += PcodeEmit::op_tag;
	packed += (opc + 0x20);
	if (outvar != (VarnodeData*)0) {
		packed += dumpVarnodeData(outvar);
	} else
		packed += PcodeEmit::void_tag;
	int4 i = 0;
	if (opc == CPUI_LOAD || opc == CPUI_STORE) {
		packed += dumpSpaceId(&vars[0]);
		i = 1;
	}
	// Possibly check for a code reference or a space reference
	for (; i < isize; ++i) {
		packed += dumpVarnodeData(&vars[i]);
	}
	packed += PcodeEmit::end_tag;
	packedPcodes += packed;
}

//PcodeCacher/PcodeBuilder is already resolving all of this on C side
/*void insertOffset(int streampos, long val, std::string& buf) {
	while (val != 0) {
		if (buf[streampos] == PcodeEmit::end_tag) {
			throw DecompError("Could not properly insert relative jump offset");
		}
		int chunk = (int)(val & 0x3f);
		val >>= 6;
		buf[streampos] = chunk + 0x20;
		streampos += 1;
	}
	for (int i = 0; i < 11; ++i) {
		if (buf[streampos] == PcodeEmit::end_tag) {
			return;
		}
		buf[streampos] = 0x20;		// Zero fill
		streampos += 1;
	}
	throw DecompError("Could not find terminator while inserting relative jump offset");
}
void resolveRelatives(std::string& buf) {
	for (int i = 0; i < labelrefs.size(); i++) {
		LabelRef ref = labelrefs[i];
		if ((ref.labelIndex >= labeldefs.size()) || (labeldefs[ref.labelIndex] == -1)) {
			throw DecompError("Reference to non-existent sleigh label");
		}
		long res = (long)labeldefs[ref.labelIndex] - (long)ref.opIndex;
		if (ref.labelSize < 8) {
			long mask = -1;
			mask >>= (8 - ref.labelSize) * 8;
			res &= mask;
		}
		// We need to skip over op_tag, op_code, void_tag, addrsz_tag, and spc bytes
		insertOffset(ref.streampos + 5, res, buf);		// Insert the final offset into the stream
	}
}*/

//Ghidra/Features/Decompiler/src/decompile/cpp/translate.cc
//Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/app/plugin/processors/sleigh/SleighInstructionPrototype.java
//Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/app/plugin/processors/sleigh/PcodeEmit.java
//Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/app/plugin/processors/sleigh/PcodeEmitPacked.java
static std::string getPackedPcode(Translate& trans, std::string space, uintb startaddr)

{ // Dump pcode translation of machine instructions
	PackedPcodeRawOut emit;		// Set up the pcode dumper
	int4 length;			// Number of bytes of each machine instruction

	//numOps = 0;
	//labelBase = 0;
	//labelCount = 0;
	//labelrefs.clear();
	//labeldefs.clear();
	Address addr(trans.getSpaceByName(space), startaddr); // First address to translate

	std::string packed;
	packed += PcodeEmit::inst_tag;
	length = trans.oneInstruction(emit, addr); // Translate instruction
	packed += dumpOffset(length);
	uchar spcindex = addr.getSpace()->getIndex();
	packed += (spcindex + 0x20);
	packed += dumpOffset(addr.getOffset());
	//resolveRelatives(emit.packedPcodes);
	packed += emit.packedPcodes;
	packed += PcodeEmit::end_tag;
	return packed;
	//if a failure occurs:
	//packed += unimpl_tag;
	//packed += dumpOffset(length);
}


//Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/app/plugin/processors/sleigh/PcodeEmitObjects.java
//Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/program/model/pcode/PcodeOp.java
class XmlPcodeEmit : public PcodeEmit {
public:
	//PcodeCacher/PcodeBuilder already resolve
	//void resolveRelatives() {
		//for (int i = 0; i < labelref.size(); i++) {
			/*int opindex = labelref.get(i);
			PcodeOp op = oplist.get(opindex);
			Varnode vn = op.getInput(0);
			int labelid = (int)vn.getOffset();
			if ((labelid >= labeldef.size()) || (labeldef.get(labelid) == null)) {
				throw DecompError("Reference to non-existant sleigh label");
			}
			long res = (long)labeldef.get(labelid) - (long)opindex;
			if (vn.getSize() < 8) {
				long mask = -1;
				mask >>>= (8 - vn.getSize()) * 8;
				res &= mask;
			}
			AddressSpace spc = vn.getAddress().getAddressSpace();
			vn = new Varnode(spc.getAddress(res), vn.getSize());
			op.setInput(vn, 0);*/
			//}
		//}
		//void addLabelRef() { labelref.push_back(numOps); }
	virtual void dump(const Address& addr, OpCode opc,
		VarnodeData* outvar, VarnodeData* vars, int4 isize)
	{
		std::string inpstr;
		for (int i = opc == CPUI_LOAD || opc == CPUI_STORE ? 1 : 0; i < isize; i++) {
			inpstr += "<addr space=\"" + vars[i].space->getName() + "\" offset=\"0x" +
				to_string(vars[i].offset, hex) + "\" size=\"" + std::to_string(vars[i].size) + "\"/>";
		}
		strs.push_back({ "<op code=\"" + std::to_string(opc) + "\">",
			(outvar != nullptr ? "<addr space=\"" + outvar->space->getName() + "\" offset=\"0x" +
				to_string(outvar->offset, hex) + "\" size=\"" + std::to_string(outvar->size) + "\"/>" : "<void/>") +
			(opc == CPUI_LOAD || opc == CPUI_STORE ? "<spaceid name=\"" + vars[0].space->getName() + "\"/>" : "") +
			inpstr + "</op>" });
	}
	std::string build(std::string space, uintb addr)
	{
		std::string str;
		for (int i = 0; i < strs.size(); i++) {
			str += strs[i].pre + "<seqnum space=\"" + space + "\" offset=\"0x" + to_string(addr, hex) +
				"\" uniq=\"0x" + to_string(i, hex) + "\"/>" + strs[i].post;
		}
		return str;
	}
	//std::string xmlPcodes; //XML serialized vector of PcodeOpRaw
	struct StrGroup { std::string pre; std::string post; };
	std::vector<StrGroup> strs;
	intb paramShift = 0; //only for callfixup
	std::vector<std::pair<std::string, int>> inputs; //only for dynamic - callotherfixup, executablepcode
	std::vector<std::pair<std::string, int>> outputs; //only for dynamic - callotherfixup, executablepcode
	//std::vector<int> labelref;
};

//Interacting with decompile/decompile.exe:
//ghidra\app\decompiler\DecompileProcess.java
static const unsigned char command_start[] = { 0, 0, 1, 2 };
static const unsigned char command_end[] = { 0, 0, 1, 3 };
static const unsigned char query_response_start[] = { 0, 0, 1, 8 };
static const unsigned char query_response_end[] = { 0, 0, 1, 9 };
static const unsigned char string_start[] = { 0, 0, 1, 14 };
static const unsigned char string_end[] = { 0, 0, 1, 15 };
static const unsigned char exception_start[] = { 0, 0, 1, 10 };
static const unsigned char exception_end[] = { 0, 0, 1, 11 };
static const unsigned char byte_start[] = { 0, 0, 1, 12 };
static const unsigned char byte_end[] = { 0, 0, 1, 13 };

DecompInterface::~DecompInterface() {
	statusGood = false;
	for (std::map<std::string, XmlPcodeEmit*>::iterator it = callFixupMap.begin();
		it != callFixupMap.end(); it++) {
		delete it->second;
	}
	for (std::map<std::string, XmlPcodeEmit*>::iterator it = callFixupOtherMap.begin();
		it != callFixupOtherMap.end(); it++) {
		delete it->second;
	}
	for (std::map<std::string, XmlPcodeEmit*>::iterator it = callMechMap.begin();
		it != callMechMap.end(); it++) {
		delete it->second;
	}
	for (std::map<std::string, XmlPcodeEmit*>::iterator it = callExecPcodeMap.begin();
		it != callExecPcodeMap.end(); it++) {
		delete it->second;
	}
	if (trans != nullptr) delete trans;
	if (context != nullptr) delete context;
	if (loader != nullptr) delete loader;
}

uchar DecompInterface::read()
{
	uchar cur;
	if (callback->readDec(&cur, 1) <= 0) throw DecompError("Read pipe is bad");
	return cur;
}

void DecompInterface::write(void const* Buf, size_t MaxCharCount)
{
	if (callback->writeDec(Buf, MaxCharCount) < 0) throw DecompError("Write pipe is bad");
}

uchar DecompInterface::readToBurst() {
	uchar cur;
	for (;;) {
		do {
			cur = read();
		} while (cur > 0);
		if (cur == -1) {
			break;
		}
		do {
			cur = read();
		} while (cur == 0);
		if (cur == 1) {
			cur = read();
			if (cur == -1) {
				break;
			}
			return cur;
		}
		if (cur == -1) {
			break;
		}
	}
	throw DecompError("Decompiler process died");
	return -1;
}

uchar DecompInterface::readToBuffer(std::vector<uchar>& buf) {
	uchar cur;
	for (;;) {
		cur = read();
		while (cur > 0) {
			if (buf.size() >= (maxResultSizeMBYtes << 20))
				throw DecompError("Maximum payload size exceeded");
			buf.push_back((uchar)cur);
			cur = read();
		}
		if (cur == -1) {
			break;
		}
		do {
			cur = read();
		} while (cur == 0);
		if (cur == 1) {
			cur = read();
			if (cur > 0) {
				return cur;
			}
		}
		if (cur == -1) {
			break;
		}
	}
	throw DecompError("Decompiler process died");
	return -1;
}

std::string DecompInterface::readQueryString() {
	int type = readToBurst();
	if (type != 14) {
		throw DecompError("GHIDRA/decompiler alignment error");
	}
	std::vector<uchar> buf; //new LimitedByteBuffer(16, 1 << 16);
	type = readToBuffer(buf);
	if (type != 15) {
		throw DecompError("GHIDRA/decompiler alignment error");
	}
	return std::string(buf.begin(), buf.end());
}

void DecompInterface::generateException() {
	std::string type = readQueryString();
	std::string message = readQueryString();
	readToBurst(); // Read exception terminator
	if (type == "alignment") {
		throw DecompError("Alignment error: " + message);
	}
	throw DecompError(type + " " + message);
}

void DecompInterface::readToResponse() {
	//device level descriptors are not buffered and do not need flushing
	//fflush(nativeOut); // Make sure decompiler has access to all the info it has been sent
	uchar type;
	do {
		type = readToBurst();
	} while ((type & 1) == 1);
	if (type == 10) {
		generateException();
	}
	if (type == 6) {
		return;
	}
	throw DecompError("Ghidra/decompiler alignment error");
}

void DecompInterface::writeString(std::string msg) {
	write(string_start, sizeof(string_start));
	write(msg.c_str(), (unsigned int)msg.size());
	write(string_end, sizeof(string_end));
}

/**
 * Transfer bytes written to -out- to decompiler process
 * @param out has the collected byte for this write
 */
void DecompInterface::writeBytes(const uchar out[], int outlen) {
	write(string_start, sizeof(string_start));
	int sz = outlen;
	uchar sz1 = (sz & 0x3f) + 0x20;
	sz >>= 6;
	uchar sz2 = (sz & 0x3f) + 0x20;
	sz >>= 6;
	uchar sz3 = (sz & 0x3f) + 0x20;
	sz >>= 6;
	uchar sz4 = (sz & 0x3f) + 0x20;
	write(&sz1, sizeof(uchar));
	write(&sz2, sizeof(uchar));
	write(&sz3, sizeof(uchar));
	write(&sz4, sizeof(uchar));
	write(out, outlen);
	write(string_end, sizeof(string_end));
}

//Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/program/model/pcode/PcodeDataTypeManager.java
std::string DecompInterface::buildTypeXml(std::vector<TypeInfo>& ti)
{
	std::string str;
	std::stack<int> s;
	s.push(0);
	while (!s.empty()) {
		int idx = s.top();
		s.pop();
		if (idx == -1) { //-1 used to indicate coming back up the stack for ptr and array
			str += "</type>\n";
			continue;
		}
		TypeInfo& typeInfo = ti.at(idx);
		//if (typeInfo == terminate) return ""; //termination happens at appropriate place implicitly
		if (typeInfo.size == -1) {
			/*int i;
			for (i = 0; i < numDefCoreTypes; i++) {
				if (defaultCoreTypes[i].name == typeInfo.typeName) break;
			} // - (i == numDefCoreTypes ? (1ull << 63) : 0) */
			str += "<typeref name=\"" + typeInfo.typeName + "\" id=\"0x" +
				to_string(hashName(typeInfo.typeName), hex) + "\"/>"; //terminates
			continue;
		}

		if (typeInfo.metaType == "ptr") {
			str += "<type name=\"" + typeInfo.typeName + /*"\" id=\"" + std::to_string(hashName(typeInfo.typeName))*/ +
				"\" metatype=\"" + typeInfo.metaType + "\" size=\"" + std::to_string(typeInfo.size) + "\">";
			s.push(-1);
			s.push(idx + 1);
		} else if (typeInfo.metaType == "struct") {
			std::string strct;
			for (int i = 0; i < typeInfo.structMembers.size(); i++) { //core types are not type referenced though
				strct += "<field name=\"" + typeInfo.structMembers[i].name +
					"\" offset=\"" + std::to_string(typeInfo.structMembers[i].offset) + "\">" +
					buildTypeXml(typeInfo.structMembers[i].ti) + "</field>\n";
			}
			str += "<type name=\"" + typeInfo.typeName + "\" id=\"" + std::to_string(hashName(typeInfo.typeName)) +
				"\" metatype=\"" + typeInfo.metaType + "\" size=\"" + std::to_string(typeInfo.size) + "\">" + strct + "</type>\n"; //terminates
		} else if (typeInfo.metaType == "array") {
			str += "<type name=\"" + typeInfo.typeName + "\" metatype=\"" + typeInfo.metaType + "\" size=\"" +
				std::to_string(typeInfo.size) + "\" arraysize=\"" + std::to_string(typeInfo.arraySize) + "\">";
			s.push(-1);
			s.push(idx + 1);
		} else if (typeInfo.metaType == "code") {
			str += "<type name=\"" + typeInfo.typeName + "\" id=\"" + std::to_string(hashName(typeInfo.typeName)) +
				"\" metatype=\"" + typeInfo.metaType + "\" size=\"" + std::to_string(typeInfo.size) + "\">" +
				writeFuncProto(typeInfo.funcInfo, "", true) +
				"</type>\n";
		} else if (typeInfo.metaType == "void") {
			str += "<void/>";
		} else {
			/*int i;
			for (i = 0; i < numDefCoreTypes; i++) {
				if (defaultCoreTypes[i].name == typeInfo.typeName) break;
			} // - (i == numDefCoreTypes ? (1ull << 63) : 0)*/
			str += "<type name=\"" + typeInfo.typeName + "\" id=\"" + to_string(hashName(typeInfo.typeName)) +
				"\" metatype=\"" + typeInfo.metaType + "\" size=\"" + std::to_string(typeInfo.size) + "\"" +
				std::string(typeInfo.isEnum ? " enum=\"true\"" : "") +
				std::string(typeInfo.isUtf ? " utf=\"true\"" : "") +
				std::string(typeInfo.isChar ? " char=\"true\"" : "") +
				">" "</type>\n";
		}
		//metaType == "uint" && callback->isEnum();
		//metaType == "code"...
	}
	return str;
}

enum comment_type {
	user1 = 1,			///< The first user defined property
	user2 = 2,			///< The second user defined property
	user3 = 4,			///< The third user defined property
	header = 8,			///< The comment should be displayed in the function header
	warning = 16,		///< The comment is auto-generated to alert the user
	warningheader = 32		///< The comment is auto-generated and should be in the header
};

void DecompInterface::adjustUniqueBase(VarnodeTpl* v) {
	if (v->getSpace().isUniqueSpace()) {
		ConstTpl c = v->getOffset();
		uintb offset = c.getReal();
		if (offset >= uniqueBase) uniqueBase = offset + 16;
	}
}

//Ghidra/Features/Decompiler/src/main/java/ghidra/app/decompiler/DecompileCallback.java
//Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/program/model/lang/PcodeInjectLibrary.java
XmlPcodeEmit* DecompInterface::getPcodeSnippet(std::string parsestring,
	const std::vector<std::pair<std::string, int>>& inputs,
	const std::vector<std::pair<std::string, int>>& outputs)
{
	PcodeSnippet compiler(trans);
	//  compiler.clear();			// Not necessary unless we reuse
	compiler.setUniqueBase(uniqueBase);
	istringstream s(parsestring);
	for (size_t i = 0; i < inputs.size(); i++)
		compiler.addOperand(inputs[i].first, inputs[i].second);
	for (size_t i = 0; i < outputs.size(); i++)
		compiler.addOperand(outputs[i].first, outputs[i].second);
	if (!compiler.parseStream(s))
		throw DecompError("Unable to compile pcode: " + compiler.getErrorMessage());
	//uintm tempbase = compiler.getUniqueBase();
	ConstructTpl* tpl = compiler.releaseResult();
	//parsestring = "";		// No longer need the memory
	//adjustUniqueBase
	for (int i = 0; i < tpl->getOpvec().size(); i++) {
		if (tpl->getOpvec()[i]->getOut() != nullptr)
			adjustUniqueBase(tpl->getOpvec()[i]->getOut());
		for (int j = 0; j < tpl->getOpvec()[i]->numInput(); j++)
			adjustUniqueBase(tpl->getOpvec()[i]->getIn(j));
	}
	//buildInstruction
	ContextInternal cdb;
	ContextCache cc(&cdb);
	//use PcodeCacher to resolve issues
	XmlPcodeEmit* emit = new XmlPcodeEmit();
	uint4 parser_cachesize = 2;
	uint4 parser_windowsize = 32;
	DisassemblyCache discache(&cc, trans->getConstantSpace(), parser_cachesize, parser_windowsize);
	ParserContext* pc = discache.getParserContext(Address());
	//ParserContext pc(&cc); //Java derives a SleighParserContext

	//Create state suitable for parsing a just a p-code semantics snippet
	ParserWalkerChange walker(pc);
	walker.baseState();
	Constructor c(nullptr);
	walker.setConstructor(&c);

	//int4 curstate = pos->getParserState();
	//if (curstate == ParserContext::uninitialized) resolve(*pos);
	// If we reach here,  state must be ParserContext::pcode
	//resolveHandles(*pos);
	PcodeCacher pcode_cache;
	//walker.snippetState();
	//the whole task here is to glue the construction template to the walker through the context
	//trans->oneInstruction(emit, Address(trans->getSpaceByName(space), addr)); //Sleigh not sufficient for ConstructTpl...
	SleighBuilder builder(&walker, &discache, &pcode_cache,
		trans->getConstantSpace(), trans->getUniqueSpace(), 0); //unique_allocatemask not used and always 0
	//now tie input and output parameters for all callfixupother, callmechanism, executablepcode
	try {
		builder.build(tpl, -1);
		pcode_cache.resolveRelatives();
		pcode_cache.emit(Address(), emit);
	} catch (SleighError& err) {
		delete emit;
		throw DecompError(err.explain.c_str());
	} catch (BadDataError& err) {
		delete emit;
		throw DecompError(err.explain.c_str());
	} catch (UnimplError& /*err*/) {
		ostringstream s;
		s << "Instruction not implemented in pcode:\n ";
		ParserWalker* cur = builder.getCurrentWalker();
		cur->baseState();
		Constructor* ct = cur->getConstructor();
		cur->getAddr().printRaw(s);
		s << ": ";
		ct->printMnemonic(s, *cur);
		s << "  ";
		ct->printBody(s, *cur);
		//err.explain = s.str();
		//err.instruction_length = fallOffset;
		delete emit;
		throw DecompError(s.str());
	} catch (LowlevelError& err) {
		delete emit;
		throw DecompError(err.explain.c_str());
	}
	return emit;
}

std::string DecompInterface::compilePcodeSnippet(std::string sleighfilename,
	std::string parsestring,
	const std::vector<std::pair<std::string, int>>& inputs,
	const std::vector<std::pair<std::string, int>>& outputs)
{
	DecompInterface* di = new DecompInterface();
	di->setupTranslator(nullptr, sleighfilename);
	XmlPcodeEmit* emit = di->getPcodeSnippet(parsestring, inputs, outputs);
	std::string str = emit->build("ram", 0x10000000);
	delete di;
	return str;
}

void getAddrFromString(std::string addrstring, std::string & space, uint8 & idx, int8* size)
{
	istringstream str(addrstring);
	Document* doc;
	try {
		doc = xml_tree(str);
	} catch (XmlError&) {
		throw DecompError("Received bad XML Address from decompiler");
	}
	Element* el = doc->getRoot();
	space = el->getAttributeValue("space");
	idx = strtoull(el->getAttributeValue("offset").c_str(), nullptr, 16);
	if (size)* size = strtoll(el->getAttributeValue("size").c_str(), nullptr, 10);
	delete doc;
}

void DecompInterface::processPcodeInject(int type, std::map<std::string, XmlPcodeEmit*> &fixupmap)
{
	std::string name = readQueryString(); //inject_sleigh.cc
	std::string context = readQueryString();
	istringstream str(context);
	Document* doc;
	try {
		doc = xml_tree(str);
	} catch (XmlError&) {
		throw DecompError("Received bad XML inject context from decompiler");
	}
	Element* el = doc->getRoot();
	const List& list(el->getChildren());
	List::const_iterator iter;
	bool bFirst = true;
	std::string space, offset, fixupspace, fixupoffset; //first address for current address, next for fixup
	for (iter = list.begin(); iter != list.end(); ++iter) {
		el = *iter;
		if (el->getName() == "addr") { //should only be 2 of them
			for (int i = 0; i < el->getNumAttributes(); i++) {
				if (el->getAttributeName(i) == "space")
					(bFirst ? space : fixupspace) = el->getAttributeValue(i);
				else if (el->getAttributeName(i) == "offset")
					(bFirst ? offset : fixupoffset) = el->getAttributeValue(i);
			}
			bFirst = !bFirst;
		}
	}
	delete doc;
	uint8 idx = strtoull(offset.c_str(), nullptr, 16);
	uint8 fixupidx = strtoull(fixupoffset.c_str(), nullptr, 16);
	XmlPcodeEmit* emitter = fixupmap[name];
	if (fixupmap[name]->strs.size() == 0) //dynamic, has no body
		emitter = getPcodeSnippet(callback->getPcodeInject(type, name, space, idx, fixupspace, fixupidx),
			emitter->inputs, emitter->outputs);
	write(query_response_start, sizeof(query_response_start));
	writeString("<inst" " offset=\"" + std::to_string(trans->instructionLength(Address(trans->getSpaceByName(space), idx))) + "\"" +
		std::string(emitter->paramShift != 0 ? "paramshift=\"" + std::to_string(emitter->paramShift) + "\"" : "") + ">" +
		emitter->build(space, idx) + "</inst>"); //" paramshift=\"\"" 
	write(query_response_end, sizeof(query_response_end));
	if (fixupmap[name] == nullptr) delete emitter;
}

std::string DecompInterface::writeFuncProto(FuncProtoInfo func, std::string injectstr, bool bUseInternalList)
{
	std::string symbols;
	std::string joinString;
	if (callStyles.find(func.model) == callStyles.end()) func.model = "unknown";
	if (bUseInternalList) {
		for (std::vector<SymInfo>::iterator it = func.syminfo.begin(); it != func.syminfo.end(); it++) {
			bool readonly = false;
			symbols += "<param name=\"" + it->pi.name + "\" typelock=\"" "true" "\" namelock=\"" + std::string(it->pi.name.size() != 0 ? "true" : "false") + "\">\n"
				"  <addr/>\n" +
				buildTypeXml(it->pi.ti) +
				"</param>\n"; //"<type name=\"\" metatype=\"" + "\" size=\"" "\"><typeref name=\"" + it->typeRef + "\" id=\"" + std::string(buf) "\"/></type>"
		}
	} else {
		if (func.retType.space == "join") {
			for (int i = 0; i < func.retType.joins.size(); i++) {
				if (i != 0) joinString += " ";
				joinString += "piece" + std::to_string(i + 1) + "=\"" + func.retType.joins[i].space + ":0x" +
					to_string(func.retType.joins[i].offset) + ":" + std::to_string(func.retType.joins[i].size) + "\"";
			}
		}
	}
	return "<prototype extrapop=\"" + (func.extraPop != -1 ? std::to_string(func.extraPop) : std::string("unknown")) +
		"\" model=\"" + func.model + "\" modellock=\"" +
		(func.model != "default" && func.model != "unknown" ? "true" : "false") +
		(func.model != "default" && func.model != "unknown" && func.syminfo.size() == 0 ? "\" voidlock=\"true" : "") +
		(func.isInline ? "\" inline=\"true" : "") +
		(func.isNoReturn ? "\" noreturn=\"true" : "") +
		(func.hasThis ? "\" hasthis=\"true" : "") +
		(func.customStorage ? "\" custom=\"true" : "") +
		(func.isConstruct ? "\" constructor=\"true" : "") +
		(func.isDestruct ? "\" destructor=\"true" : "") +
		(func.dotdotdot ? "\" dotdotdot=\"true" : "") +
		"\">\n"
		"  <returnsym" + std::string(func.retType.pi.ti.begin()->metaType != "unknown" ? " typelock=\"true\"" : "") + ">\n" +
		std::string(func.retType.size == 0 ? "   <addr/>\n" "   <void/>\n" :
			((bUseInternalList ? "   <addr/>\n" : 
			"   <addr space=\"" + func.retType.space + "\" " + (func.retType.space != "join" ? "offset=\"0x" + to_string(func.retType.offset, hex) +
				"\" size=\"" + std::to_string(func.retType.size) + "\"" : joinString) + "/>\n") +
			buildTypeXml(func.retType.pi.ti))) +
		"  </returnsym>\n" +
		injectstr + //Pcode injection: "<inject>" + "</inject>\n"
		(bUseInternalList ? "  <internallist>\n" + symbols + "  </internallist>\n" : "") +  
		"</prototype>\n";
}

std::string DecompInterface::writeFunc(std::string space, unsigned long long offset, unsigned long long size, std::string funcname, std::string parentname, FuncProtoInfo func)
{//mapsym can have type: type="dynamic"/"equate" as well as booleans volatile, indirectstorage, hiddenretparm
							//valid symbol tags are: symbol/dynsymbol, equatesymbol, function/functionshell, labelsym, externrefsymbol
							//prototype can contain elements for <unaffected>..., <killedbycall>..., <returnaddress>..., <likelytrash>...
							//Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/program/model/pcode/HighFunction.java
							//Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/program/model/pcode/LocalSymbolMap.java
	//"<mapsym><symbol name=\"" "\" typelock=\"" "\" namelock=\"" "\" readonly=\"" "\" volatile=\"" "\" cat=\"" "\" index=\"" "\">" "<typeref name=\"" "\"/>" "</symbol>"
	//"<addr space=\"" + space + "\" offset=\"0x" + std::string() + "\" size=\"" + "\"/>"
	//"<rangelist/></mapsym>"
	//callback->status("Function name: " + funcname);
	//int index = 0;
	std::string res;
	ostringstream nameesc;
	xml_escape(nameesc, funcname.c_str());
	ostringstream parentnameesc;
	xml_escape(parentnameesc, parentname.c_str());
	std::string symbols;
	for (std::vector<SymInfo>::iterator it = func.syminfo.begin(); it != func.syminfo.end(); it++) {
		//must bitmask any negative offsets to the current addressing sizes, for stack, likely pointers and others too
		int addrSize = 0;
		if (it->space == "stack") {
			if (trans->getStackSpace() != nullptr)
				addrSize = trans->getStackSpace()->getAddrSize();
			else
				addrSize = trans->getRegister(stackPointerReg).size;
		} else addrSize = trans->getSpaceByName(it->space)->getAddrSize();
		it->offset &= ((~0ull) >> (64 - addrSize * 8));
		bool readonly = false;
		std::string joinString;
		if (it->space == "join") {
			for (int i = 0; i < it->joins.size(); i++) {
				if (i != 0) joinString += " ";
				joinString += "piece" + std::to_string(i + 1) + "=\"" + it->joins[i].space + ":0x" +
					to_string(it->joins[i].offset) + ":" + std::to_string(it->joins[i].size) + "\"";
			}
		}
		//current function arguments must be added to the category 0 index # in order of function definition parameters
		//this should be part of the symbol info and not done in here as nuances like type inference effect this
		//bool bIsCat = idx == startOffs && it->space == "stack" && it->offset != 0 && (it->offset & (1ull << (addrSize * 8 - 1))) == 0;
		//could assume positive offsets are args but indexes now assigned by callback
		symbols += "<mapsym><symbol name=\"" + it->pi.name + "\" typelock=\"" +
			(it->pi.ti.begin()->metaType == "unknown" ? "false" : "true") + //startOffs == idx && it->argIndex == -1
			"\" namelock=\"" + std::string(it->pi.name.size() != 0 ? "true" : "false") + "\" readonly=\"" + (readonly ? "true" : "false") +
			"\" cat=\"" + std::string(it->argIndex != -1 ? "0\" index=\"" + std::to_string(it->argIndex) : "-1") + "\">" +
			buildTypeXml(it->pi.ti) +
			"</symbol>"
			"<addr space=\"" + it->space + "\" " + (it->space != "join" ? "offset=\"0x" + to_string(it->offset, hex) +
				"\" size=\"" + std::to_string(it->size) + "\"" : joinString) + "/>" +
			std::string(it->space == "register" ? "<rangelist>\n<range space=\"" + (it->argIndex != -1 ? space : it->range.space) +
				"\" first=\"0x" + to_string(it->argIndex != -1 ? offset - 1 : it->range.beginoffset, hex) +
				"\" last=\"0x" + to_string(it->argIndex != -1 ? offset - 1 : it->range.endoffset, hex) + "\"/></rangelist>\n" : "<rangelist/>") +
			"</mapsym>";
	}
	//callback->status("Extern name: " + externname);
	//prototype has uponentry and uponreturn tags to inject pcode for call mechanism fixup
	res = "<result>\n<parent>\n<val/>\n" + std::string(parentname.size() != 0 ? "<val>" + parentnameesc.str() + "</val>" : "") + "</parent>\n<mapsym>\n"
		"<function name=\"" + nameesc.str() + "\" size=\"" + std::to_string(size) + "\">\n"
		"<addr space=\"" + space + "\" offset=\"0x" + to_string(offset, hex) + "\"/>"
		"<localdb lock=\"false\" main=\"" "stack" "\">\n<scope name=\"" + nameesc.str() + "\">\n"
		"<parent>\n<val/>\n" + std::string(parentname.size() != 0 ? "<val>" + parentnameesc.str() + "</val>" : "") + "</parent>\n<rangelist/>\n"
		"<symbollist>" + symbols + "\n</symbollist>\n</scope>\n</localdb>\n" +
		writeFuncProto(func, (fixupTargetMap.find(funcname) != fixupTargetMap.end() ?
			"<inject>" + fixupTargetMap[funcname] + "</inject>" :
			(callFixupMap.find(funcname) != callFixupMap.end() ? "<inject>" + nameesc.str() + "</inject>" : "")), false) +
		"</function>\n"
		"<addr space=\"" + space + "\" offset=\"0x" + to_string(offset, hex) + "\"/>"
		"<rangelist/></mapsym>\n</result>\n";
	return res;
}

std::string buildHoleXml(std::string space, unsigned long long beginoffset,
	unsigned long long endoffset, bool readOnly, bool volatil)
{
	return "<hole readonly=\"" + std::string(readOnly ? "true" : "false") + "\" volatile=\"" +
		std::string(volatil ? "true" : "false") + "\" space=\"" +
		space + "\" first=\"0x" + to_string(beginoffset, hex) +
		"\" last=\"0x" + to_string(endoffset, hex) + "\"/>";
}

std::vector<uchar> DecompInterface::readResponse() {
	readToResponse();
	int type = readToBurst();
	std::string name;
	std::vector<uchar> retbuf;
	std::vector<uchar> buf;
	std::string addrstring;
	while (type != 7) {
		switch (type) {
		case 4:
			name = readQueryString();
			try {
				//if (name != "getUserOpName" && name != "getRegister") throw DecompError(name);
				if (name.length() < 4) {
					throw DecompError("Bad decompiler query: " + name);
				}
				switch (name[3]) {
				case 'B':
				{
					addrstring = readQueryString();
					std::string space;
					uint8 idx;
					int8 sz;
					getAddrFromString(addrstring, space, idx, &sz);
					//callback->status("getBytes " + addrstring);
					std::vector<uchar> res;
					res.resize(sz);
					callback->getBytes(res.data(), sz, space, idx);
					std::vector<uchar> dblres;
					dblres.resize(res.size() * 2);
					for (int i = 0; i < res.size(); i++) {
						dblres[i * 2] = (uchar)(((res[i] >> 4) & 0xf) + 65);
						dblres[i * 2 + 1] = (uchar)((res[i] & 0xf) + 65);
					}
					write(query_response_start, sizeof(query_response_start));
					write(byte_start, sizeof(byte_start));
					write(dblres.data(), dblres.size());
					write(byte_end, sizeof(byte_end));
					write(query_response_end, sizeof(query_response_end));
					//getBytes();						// getBytes
					break;
				}
				case 'C':
					if (name == "getComments") {
						addrstring = readQueryString();
						//flags from Ghidra/Features/Decompiler/src/decompile/cpp/comment.hh based on initialization options
						std::string flags = readQueryString();
						std::string space;
						uint8 idx;
						getAddrFromString(addrstring, space, idx, nullptr);
						uint8 f = strtoull(flags.c_str(), nullptr, 10);
						//callback->status("getComments " + addr + " " + flags);
						//addr = Varnode.readXMLAddress(addrstring, addrfactory, funcEntry.getAddressSpace());
						//flags = SpecXmlUtils.decodeInt(types);
						//Function func = getFunctionAt(addr);
						//AddressSetView addrset = func.getBody();
						/*if ((flags & 8) != 0) {
							generateHeaderCommentXML(func, buf);
						}
						if ((flags & 1) != 0) {
							generateCommentXML(addrset, addr, buf, CodeUnit.EOL_COMMENT);
						}
						if ((flags & 2) != 0) {
							generateCommentXML(addrset, addr, buf, CodeUnit.PRE_COMMENT);
						}
						if ((flags & 4) != 0) {
							generateCommentXML(addrset, addr, buf, CodeUnit.POST_COMMENT);
						}
						if ((flags & 8) != 0) {
							generateCommentXML(addrset, addr, buf, CodeUnit.PLATE_COMMENT);
						}*/
						std::string commentStr;
						std::vector<CommentInfo> comments;
						callback->getComments(space, idx, comments);
						for (int i = 0; i < comments.size(); i++) {
							if (comments[i].type == "header" && (f & comment_type::header) != 0 ||
								comments[i].type == "warning" && (f & comment_type::warning) != 0 ||
								comments[i].type == "warningheader" && (f & comment_type::warningheader) != 0 ||
								comments[i].type == "user1" && (f & comment_type::user1) != 0 ||
								comments[i].type == "user2" && (f & comment_type::user2) != 0 ||
								comments[i].type == "user3" && (f & comment_type::user3) != 0) {
								ostringstream commentesc;
								xml_escape(commentesc, comments[i].text.c_str());
								commentStr += "<comment type=\"" + comments[i].type + "\">\n"
									"<addr space=\"" + space + "\" offset=\"0x" + to_string(idx, hex) + "\"/>"
									"<addr space=\"" + comments[i].space + "\" offset=\"0x" + to_string(comments[i].offset, hex) + "\"/>"
									"\n<text>" + commentesc.str() + "</text>\n"
									"</comment>";
							}
						}
						write(query_response_start, sizeof(query_response_start));
						writeString("<commentdb>\n" + commentStr + "</commentdb>");
						write(query_response_end, sizeof(query_response_end));
						//getComments();
					} else if (name == "getCallFixup") {
						processPcodeInject(CALLFIXUP_TYPE, callFixupMap);
					} else if (name == "getCallotherFixup") {
						processPcodeInject(CALLOTHERFIXUP_TYPE, callFixupOtherMap);
					} else if (name == "getCallMech") {
						processPcodeInject(CALLMECHANISM_TYPE, callMechMap);
					} else {
						std::string liststring = readQueryString();
						std::vector<unsigned long long> refs;
						std::stringstream ss(liststring);
						std::string item;
						while (getline(ss, item, ',')) refs.push_back(strtoull(item.c_str(), nullptr, 16));
						CPoolRecord rec;
						callback->getCPoolRef(refs, rec);
						std::string dblres;
						ostringstream commentesc;
						if (rec.data.size() != 0) {
							int wrap = 0;
							for (int i = 0; i < rec.data.size(); i++) {
								int val = (rec.data[i] >> 4) & 0xf;
								dblres.push_back(val > 9 ? val - 10 + 'a' : (val + '0'));
								val = rec.data[i] & 0xf;
								dblres.push_back(val > 9 ? val - 10 + 'a' : (val + '0'));
								dblres.push_back(' ');
								wrap++;
								if (wrap > 15) {
									dblres.push_back('\n');
									wrap = 0;
								}
							}
						} else {
							xml_escape(commentesc, rec.token.c_str());
						}
						write(query_response_start, sizeof(query_response_start));
						writeString("<cpoolrec ref=\"" + std::to_string(refs[0]) + "\" tag=\"" + cpoolreftags[rec.tag] + "\"" +
							std::string(rec.hasThis ? " hasthis=\"true\"" : "") +
							std::string(rec.constructor ? " constructor=\"true\"" : "") + ">\n" +
							(rec.tag == PRIMITIVE ? "<value>" + std::to_string(rec.value) + "</value>" : "") +
							(rec.data.size() != 0 ? "<data length=\"" + std::to_string(rec.data.size()) + "\">\n" + dblres + "</data>\n" :
								"<token>" + commentesc.str() + "</token>\n") +
							"</cpoolrec>");
						write(query_response_end, sizeof(query_response_end));
						//getCPoolRef(); //constant pool only implemented currently for JVM/Dalvik
						//Ghidra/Processors/JVM/src/main/java/ghidra/app/util/pcodeInject/ConstantPoolJava.java
						//hard coded constants and some sort of record format with tag, token value and type returned for the comman deliminated query
					}
					break;
				case 'E':
				{
					addrstring = readQueryString();
					//callback->status("getExternalRefXML " + addrstring);
					std::string space;
					uint8 idx;
					getAddrFromString(addrstring, space, idx, nullptr);
					std::string externname;
					std::string modName;
					FuncProtoInfo func = {};
					callback->getExternInfo(space, idx, externname, modName, func);
					std::string res = writeFunc(space, idx, 1, externname, modName, func);
					write(query_response_start, sizeof(query_response_start));
					writeString(res);
					write(query_response_end, sizeof(query_response_end));
					//getExternalRefXML();			// getExternalRefXML
					break;
				}
				case 'M':
				{
					addrstring = readQueryString();
					//callback->status("getMappedSymbolsXML " + addrstring);
					istringstream str(addrstring);
					std::string space;
					uint8 idx;
					getAddrFromString(addrstring, space, idx, nullptr);
					if (space == "register") {
						//trans->getRegisterName(trans->getSpaceByName(space), idx, sz).c_str()
					}
					MappedSymbolInfo msi = { KIND_HOLE };
					callback->getMappedSymbol(space, idx, msi);
					std::string res;
					if (msi.kind == KIND_FUNCTION) {
						if (msi.entryPoint != idx) {
							std::vector<RangeInfo>::iterator iter = msi.ranges.begin();
							for (; iter != msi.ranges.end(); iter++) {
								if (idx >= iter->beginoffset && idx <= iter->endoffset) {
									res = buildHoleXml(space, iter->beginoffset, iter->endoffset, true, false);
									break;
								}
							}
							if (iter == msi.ranges.end())
								res = buildHoleXml(space, idx, idx, true, false);
						} else {
							res = writeFunc(space, idx, msi.size, msi.name, "", msi.func);
						}
					} else if (msi.kind == KIND_DATA) {
						ostringstream datanameesc;
						xml_escape(datanameesc, msi.name.c_str());
						//typelock, namelock, readonly, volatile also cat (szCoreTypes), index
						//callback->status("Data name: " + dataname);
						res = "<result>\n<parent>\n<val/>\n</parent>\n<mapsym>\n" //type=\"dynamic\"/\"equate"
							"<symbol name=\"" + datanameesc.str() + "\" typelock=\"" +
							(msi.typeChain.begin()->metaType == "unknown" ? "false" : "true") +
							"\" namelock=\"" + std::string(msi.name.size() != 0 ? "true" : "false") + "\" readonly=\"" + (msi.readonly ? "true" : "false") +
							"\" volatile=\"" + std::string(msi.volatil ? "true" : "false") + "\" cat=\"" "-1" "\">" +
							buildTypeXml(msi.typeChain) +
							"</symbol>"
							"<addr space=\"" + space + "\" offset=\"0x" + to_string(idx, hex) + "\"/>"
							"<rangelist/></mapsym>\n</result>\n";
					} else if (msi.kind == KIND_EXTERNALREFERENCE) {
						ostringstream nameesc;
						xml_escape(nameesc, msi.name.c_str());
						//callback->status("Extern name: " + externname);
						res = "<result>\n<parent>\n<val/>\n</parent>\n<mapsym>\n"
							"<externrefsymbol name=\"" + nameesc.str() + "\">"
							"<addr space=\"" + space + "\" offset=\"0x" + to_string(idx, hex) + "\"/>"
							"</externrefsymbol>"
							"<addr space=\"" + space + "\" offset=\"0x" + to_string(idx, hex) + "\"/>"
							"<rangelist/></mapsym>\n</result>\n";
					} else if (msi.kind == KIND_LABEL) {
						ostringstream nameesc;
						xml_escape(nameesc, msi.name.c_str());
						res = "<result>\n<parent>\n<val/>\n</parent>\n<mapsym>\n"
							"<labelsym name=\"" + nameesc.str() + "\" namelock=\"true\" typelock=\"true\" readonly=\"" +
							std::string(msi.readonly ? "true" : "false") + "\" volatile=\"" +
							std::string(msi.volatil ? "true" : "false") + "\" cat=\"" "-1" "\"/>\n"
							"<addr space=\"" + space + "\" offset=\"0x" + to_string(idx, hex) + "\"/>"
							"<rangelist/></mapsym>\n</result>\n";
					}
					if (res.size() == 0) { //KIND_HOLE
						//get the readonly status
						res = buildHoleXml(space, idx, idx, msi.readonly, msi.volatil);
					} //get info about hole
					//addr = Varnode.readXMLAddress(addrstring, addrfactory, funcEntry.getAddressSpace());
					//Object obj = lookupSymbol(addr);
					/*if (obj instanceof Function) {
						boolean includeDefaults = addr.equals(funcEntry);
						res = buildFunctionXML((Function) obj, addr, includeDefaults);
					}
					else if (obj instanceof Data) {
						res = buildData((Data) obj);
					}
					else if (obj instanceof ExternalReference) {
						res = buildExternalRef(addr, (ExternalReference) obj);
					}
					else if (obj instanceof Symbol) {
						res = buildLabel((Symbol) obj, addr);
					}
					if (res == nullptr) { // There is a hole, describe the extent of the hole
						res = buildHole(addr).toString();
					}
					*/
					write(query_response_start, sizeof(query_response_start));
					writeString(res);
					write(query_response_end, sizeof(query_response_end));
					//getMappedSymbolsXML();			// getMappedSymbolsXML
					break;
				}
				case 'P':
				{
					addrstring = readQueryString();
					//callback->status("getPcodePacked " + addrstring);
					istringstream str(addrstring);
					std::string space;
					uint8 idx;
					getAddrFromString(addrstring, space, idx, nullptr);
					//addr = Varnode.readXMLAddress(addrstring, addrfactory, funcEntry.getAddressSpace());
					//Instruction instr = getInstruction(addr);
					//PackedBytes pcode = instr.getPrototype().getPcodePacked(instr.getInstructionContext(),
					//	new InstructionPcodeOverride(instr), uniqueFactory);
					std::string packed;
					try {
						packed = getPackedPcode(*trans, space, idx);
					} catch (SleighError&) {
					} catch (BadDataError&) {
					} catch (UnimplError&) {
					} catch (LowlevelError&) {
					} catch (DecompError&) {
						//no byte to load send empty response to decompiler
					}
					//get instruction bytes from IDA and use the Sleigh interface to translate to P-code
					write(query_response_start, sizeof(query_response_start));
					if (packed.size() != 0) writeBytes((const uchar*)packed.data(), packed.size());
					write(query_response_end, sizeof(query_response_end));
					//getPcodePacked();				// getPacked
					break;
				}
				case 'R':
					if (name == "getRegister") {
						std::string nm = readQueryString();
						//in .sla file: <varnode_sym name="REG" id="0xID" scope="0x0" space="register" offset="0xOFFSET" size="SIZE"></varnode_sym>
						//std::map<std::string, std::string>::const_iterator it = registers.find(nm);
						//if (it != registers.end()) writeString(it->second + "\n");
						VarnodeData vd = trans->getRegister(nm);
						std::string res = "<addr space=\"" + vd.space->getName() + "\" offset=\"0x" +
							to_string(vd.offset, hex) + "\" size=\"" + std::to_string(vd.size) + "\"/>\n";
						write(query_response_start, sizeof(query_response_start));
						writeString(res);
						write(query_response_end, sizeof(query_response_end));
						//getRegister();
					} else { //getRegisterName mainly because not all registers enumerated initially with getRegister
						//however this is probably never a problem, but it could be a problem if there is some highly specialized code with uneven alignment on registers?
						addrstring = readQueryString(); //is addr the same as offset?
						std::string space;
						uint8 idx;
						int8 sz;
						getAddrFromString(addrstring, space, idx, &sz);
						//Address addr = Varnode.readXMLAddress(addrstring, addrfactory, nullptr);
						//int size = readXMLSize(addrstring);
						//Register reg = pcodelanguage.getRegister(addr, size);
						//std::map<std::string, std::string>::iterator it;
						//for (it = registers.begin(); it != registers.end(); it++) {
						//	if (it->second == addrstring) break;
						//}
						//writeString(it == registers.end() ? "" : it->first);
						write(query_response_start, sizeof(query_response_start));
						writeString(trans->getRegisterName(trans->getSpaceByName(space), idx, sz).c_str());
						write(query_response_end, sizeof(query_response_end));
					}
					break;
				case 'S':
				{
					addrstring = readQueryString();
					std::string space;
					uint8 idx;
					getAddrFromString(addrstring, space, idx, nullptr);
					//callback->status("getSymbol " + addrstring);
					write(query_response_start, sizeof(query_response_start));
					writeString(callback->getSymbol(space, idx));
					write(query_response_end, sizeof(query_response_end));
					//getSymbol();					// getSymbol
					break;
				}
				case 'T':
					if (name == "getType") {
						std::string name = readQueryString();
						std::string id = readQueryString();
						//Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/program/model/pcode/PcodeDataTypeManager.java
						//getType();
						//Pointer
						//"<type name=\"" "\" metatype=\"ptr\" size=\"" "\" wordsize=\"" "\">" "<typeref />" "</type>";
						//Array, String, TerminatedString, StringUTF8, Unicode, TerminatedUnicode, Unicode32, TerminatedUnicode32
						//"<type name=\"" "\" metatype=\"array\" size=\"" "\" arraysize=\"" "\">" "<typeref />" "</type>";
						//Structure
						//"<type name=\"" "\" metatype=\"struct\" size=\"" "\">"
						//"<field name=\"" "\" offset=\"" "\">" "<typeref />" "</field>"
						//"</type>";
						//Enum
						//"<type name=\"" "\" metatype=\"uint\" size=\"" "\" enum=\"true\">"
						//"<val name=\"" "\" value=\"" "\"/>"
						//"</type>";
						//Function
						//"<type name=\"" "\" metatype=\"code\" size=\"1\">" "</type>"; //new FunctionPrototype(fdef, cspec, voidInputIsVarargs).buildPrototypeXML(resBuf, this) from function definition and compiler spec
						//Regular: WideChar, WideChar16, WideChar32, AbstractInteger, Boolean, AbstractFloat
						//can use coreTypes
						//Otherwise < 16: unknown, > 16: Array
						std::vector<TypeInfo> typeChain;
						callback->getMetaType(name, typeChain);
						write(query_response_start, sizeof(query_response_start));
						writeString(typeChain.size() != 0 ? buildTypeXml(typeChain) : "");
						write(query_response_end, sizeof(query_response_end));
					} else {
						//getTrackedRegisters();
						addrstring = readQueryString();
						//callback->status("getTrackedRegisters " + addrstring);
						std::string space;
						uint8 idx;
						getAddrFromString(addrstring, space, idx, nullptr);
						//.pspec context_data -> tracked_set -> <set name="DF" val="0"/>
						//addr = Varnode.readXMLAddress(addrstring, addrfactory, funcEntry.getAddressSpace());
						//ProgramContext context = program.getProgramContext();
						//Register[] regs = context.getRegisters();
						//Varnode.appendSpaceOffset(stringBuf, addr);
						/*for (Register reg : regs) {
							if (reg.isProcessorContext()) {
								continue;
							}
							BigInteger val = context.getValue(reg, addr, false);
							if (val != nullptr) {
								buildTrackSet(stringBuf, reg, val.longValue());
							}
						}*/
						//"<set space=\"register\" offset=\"0x106\" size=\"2\" val=\"0x90c\"/>\n"
						//"<set space=\"register\" offset=\"0x20a\" size=\"1\" val=\"0x0\"/>\n"
						//only the DF direction control flag on x86 is reported by default here for string operations from the pspec
						TrackedSet ts = context->getTrackedSet(Address(trans->getSpaceByName(space), idx));
						std::string track;
						for (TrackedSet::iterator it = ts.begin(); it != ts.end(); it++) {
							track += "<set space=\"" + it->loc.space->getName() + "\" offset=\"0x" +
								to_string(it->loc.offset, hex) + "\" size=\"" + std::to_string(it->loc.size) +
								"\" val=\"0x" + to_string(it->val, hex) + "\"/>\n";
						}
						//register values upon entry
						write(query_response_start, sizeof(query_response_start));
						writeString(std::string("<tracked_pointset space=\"" + space +
							"\" offset=\"0x") + to_string(idx, hex) +
							"\">\n" + track +
							"</tracked_pointset>\n");
						write(query_response_end, sizeof(query_response_end));
					}
					break;
				case 'U':
				{
					std::string indexStr = readQueryString();
					//in .sla file: <userop_head name = "UserOpName" id = "0xID" scope = "0x0" / >
					int index = strtol(indexStr.c_str(), nullptr, 10);
					//std::map<int, std::string>::const_iterator it = userOpNames.find(index);
					//std::string nm = it == userOpNames.end() ? "" : it->second;
					std::vector<std::string> userOpNames;
					trans->getUserOpNames(userOpNames);
					std::string nm = index >= userOpNames.size() ? "" : userOpNames[index];
					write(query_response_start, sizeof(query_response_start));
					writeString(nm);
					write(query_response_end, sizeof(query_response_end));
					//getUserOpName();				// getUserOpName
				}
				break;
				case 'X':
					processPcodeInject(EXECUTABLEPCODE_TYPE, callExecPcodeMap);
					break;
				default:
					throw DecompError("Unsupported decompiler query '" + name + "'");
				}
			} catch (DecompError& e) { // Catch ANY exception query generates
				// and pass it down to decompiler
				std::string extype = "DecompError"; // e.getClass().getName();
				//std::string msg = e.getMessage();
				write(exception_start, sizeof(exception_start));
				writeString(extype.c_str());
				writeString(e.explain.c_str());
				write(exception_end, sizeof(exception_end));
			}
			//fflush(nativeOut); // Make sure decompiler receives response
			readToBurst(); // Read query terminator
			break;
		case 6:
			throw DecompError("GHIDRA/decompiler out of alignment");
		case 10:
			generateException();
			break;
		case 14:			// Start of the main decompiler output
			if (buf.capacity() != 0) {
				throw DecompError("Nested decompiler output");
			}
			// Allocate storage buffer for the result, which is generally not tiny. So we
			// start with any initial allocation of 1024 bytes, also give an absolute upper bound
			// determined by maxResultSizeMBYtes
			//buf = LimitedByteBuffer(1024, maxResultSizeMBYtes << 20);
			buf.reserve(1024);
			break;
		case 15:			// This is the end of the main decompiler output
			if (buf.capacity() == 0) { //== nullptr...
				throw DecompError("Mismatched string header");
			}
			retbuf = buf;
			buf.clear();		// Reset the main buffer as a native message may follow
			buf.shrink_to_fit();
			break;
		case 16:			// Beginning of any native message from the decompiler
			//if (buf.capacity() != 0)
				//throw DecompError(("Nested decompiler output");
					// if buf is non-null, then res was interrupted
					// so we just throw out the partial result
					//buf = LimitedByteBuffer(64, 1 << 20);
			buf.reserve(64);
			break;
		case 17:			// End of the native message from the decompiler
			if (buf.capacity() == 0) { //== nullptr...
				throw DecompError("Mismatched message header");
			}
			//callback.setNativeMessage(buf.toString());
			//callback->status("Decompiler response: " + std::string(buf.begin(), buf.end()));
			//this is the one case where the process does not need to be relaunched...
			if (buf.size() != 0) { //can enhance if resbuf contains partial output?
				//always "\r\n" at start of message?
				//need to replace all new lines with "//"
				std::replace_if(buf.begin() + 2, buf.end(), [](uchar it) { return it == '\n'; }, ' ');
				std::string str = "<doc><function><comment color=\"comment\">\n"
					"//Decompiler native message: " + std::string(buf.begin()+2, buf.end()) +
					"\n</comment></function></doc>";
				retbuf = std::vector<uchar>(str.begin(), str.end());
			}
			buf.clear();
			buf.shrink_to_fit();
			break;
		default:
			throw DecompError("GHIDRA/decompiler alignment error");

		}
		if (buf.capacity() == 0) { //== nullptr...
			type = readToBurst();
		}
		else {
			type = readToBuffer(buf);
		}
	}
	return retbuf;
}

std::string readFileAsString(std::string filename)
{
	FILE* fp = fopen(filename.c_str(), "rb");
	if (fp == nullptr) return std::string();
	std::vector<uchar> buf;
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	buf.resize(size);
	fseek(fp, 0, SEEK_SET);
	fread(buf.data(), size, 1, fp);
	fclose(fp);
	return std::string(buf.begin(), buf.end());
}

std::string DecompInterface::getOptions(Options opt)
{
	return "<optionslist>\n"
		"\t<currentaction>\n"
		"\t\t<param1>conditionalexe</param1>\n"
		"\t\t<param2>" + std::string(opt.conditionalexe ? "on" : "off") + "</param2>\n"
		"\t</currentaction>\n"
		"\t<readonly>" + std::string(opt.readonly ? "on" : "off") + "</readonly>\n"
		"\t<currentaction>\n"
		"\t\t<param1>decompile</param1>\n"
		"\t\t<param2>unreachable</param2>\n"
		"\t\t<param3>" + std::string(opt.decompileUnreachable ? "on" : "off") + "</param3>\n"
		"\t</currentaction>\n"
		"\t<currentaction>\n"
		"\t\t<param1>decompile</param1>\n"
		"\t\t<param2>doubleprecis</param2>\n"
		"\t\t<param3>" + std::string(opt.decompileDoublePrecis ? "on" : "off") + "</param3>\n"
		"\t</currentaction>\n"
		"\t<ignoreunimplemented>" + std::string(opt.ignoreUnimplemented ? "on" : "off") +
		"</ignoreunimplemented>\n"
		"\t<inferconstptr>" + std::string(opt.inferConstPtr ? "on" : "off") + "</inferconstptr>\n"
		"\t<nullprinting>" + std::string(opt.nullPrinting ? "on" : "off") + "</nullprinting>\n"
		"\t<inplaceops>" + std::string(opt.inPlaceOps ? "on" : "off") + "</inplaceops>\n"
		"\t<conventionprinting>" + std::string(opt.conventionPrinting ? "on" : "off") +
		"</conventionprinting>\n"
		"\t<nocastprinting>" + std::string(opt.noCastPrinting ? "on" : "off") +
		"</nocastprinting>\n"
		"\t<maxlinewidth>" + std::to_string(opt.maxLineWidth) + "</maxlinewidth>\n"
		"\t<indentincrement>" + std::to_string(opt.indentIncrement) + "</indentincrement>\n"
		"\t<commentindent>" + std::to_string(opt.commentIndent) + "</commentindent>\n"
		"\t<commentstyle>" + opt.commentStyle + "</commentstyle>\n"
		"\t<commentinstruction>\n"
		"\t\t<param1>header</param1>\n"
		"\t\t<param2>" + std::string(opt.commentInstructionHeader ? "on" : "off") + "</param2>\n"
		"\t</commentinstruction>\n"
		"\t<commentinstruction>\n"
		"\t\t<param1>user2</param1>\n"
		"\t\t<param2>" + std::string(opt.commentUser2 ? "on" : "off") + "</param2>\n"
		"\t</commentinstruction>\n"
		"\t<commentinstruction>\n"
		"\t\t<param1>user1</param1>\n"
		"\t\t<param2>" + std::string(opt.commentUser1 ? "on" : "off") + "</param2>\n"
		"\t</commentinstruction>\n"
		"\t<commentinstruction>\n"
		"\t\t<param1>user3</param1>\n"
		"\t\t<param2>" + std::string(opt.commentUser3 ? "on" : "off") + "</param2>\n"
		"\t</commentinstruction>\n"
		"\t<commentinstruction>\n"
		"\t\t<param1>warning</param1>\n"
		"\t\t<param2>" + std::string(opt.commentWarning ? "on" : "off") + "</param2>\n"
		"\t</commentinstruction>\n"
		"\t<commentheader>\n"
		"\t\t<param1>header</param1>\n"
		"\t\t<param2>" + std::string(opt.commentHeader ? "on" : "off") + "</param2>\n"
		"\t</commentheader>\n"
		"\t<commentheader>\n"
		"\t\t<param1>warningheader</param1>\n"
		"\t\t<param2>" + std::string(opt.commentWarningHeader ? "on" : "off") + "</param2>\n"
		"\t</commentheader>\n"
		"\t<integerformat>" + opt.integerFormat + "</integerformat>\n"
		"\t<setlanguage>" + opt.setLanguage + "</setlanguage>\n"
		"\t<protoeval>" +
		((callStyles.find(opt.protoEval) == callStyles.end()) ? callStyle : opt.protoEval) +
		"</protoeval>\n"
		"</optionslist>"; //protoeval must be set to one allowed in the cspec or setOptions fails;
}

void DecompInterface::setupTranslator(DecompileCallback* cb, std::string sleighfilename)
{
	callback = cb;
	/*ifstream s(sleighfilename);
	Document* doc = xml_tree(s);
	s.close();
	Element* el = doc->getRoot();
	const List& list(el->getChildren());
	List::const_iterator iter;
	//uniqBase = strtoull(el->getAttributeValue("uniqbase").c_str(), nullptr, 16);
	//uniqBase = 0x10000000; //hard coded per Ghidra/Features/Decompiler/src/main/java/ghidra/app/decompiler/DecompInterface.java
	//bigEndian = el->getAttributeValue("bigendian") == "true";
	for (iter = list.begin(); iter != list.end(); ++iter) {
		el = *iter;
		if (el->getName() == "spaces") {
			//global defined here:
			//Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/app/plugin/processors/sleigh/SleighLanguage.java
			procSpaces = "<" + el->getName();
			for (int i = 0; i < el->getNumAttributes(); i++) {
				procSpaces += " " + el->getAttributeName(i) + "=\"" + el->getAttributeValue(i) + "\"";
			}
			procSpaces += ">";
			const List& lst(el->getChildren());
			List::const_iterator it;
			for (it = lst.begin(); it != lst.end(); ++it) {
				Element* e = *it;
				procSpaces += "<" + e->getName();
				for (int i = 0; i < e->getNumAttributes(); i++) {
					procSpaces += " " + e->getAttributeName(i) + "=\"" + e->getAttributeValue(i) + "\"";
				}
				if (e->getAttributeValue("name") == "ram") procSpaces += " global=\"true\""; //tag=space
				else if (e->getAttributeValue("name") == "register") procSpaces += " global=\"false\""; //tag=space
				else if (e->getAttributeValue("name") == "unique") procSpaces += " global=\"false\""; //tag=space_unique
				procSpaces += "/>";
			}
			procSpaces += "</spaces>";
			//need to add global attributes: false for unique and register, true for ram e.g. global="true/false" on the space_unique and space children though
		} /*else if (el->getName() == "symbol_table") {
			const List& lst(el->getChildren());
			List::const_iterator it;
			for (it = lst.begin(); it != lst.end(); ++it) {
				Element* e = *it;
				if (e->getName() == "userop_head") {
					//uint8 idx = strtoull(e->getAttributeValue("id").c_str(), nullptr, 16);
					//int idx = userOpNames.size();
					//userOpNames[idx] = e->getAttributeValue("name");
				} else if (e->getName() == "varnode_sym" && e->getAttributeValue("space") == "register") {
					//registers[e->getAttributeValue("name").c_str()] = "<addr space=\"register\" offset=\"" + e->getAttributeValue("offset") + "\" size=\"" + e->getAttributeValue("size") + "\"/>";
				}
			}
		}*/
		//}
		//delete doc;
		//  loader->open();
		//    loader->adjustVma(adjustvma);
		// Set up the context object
	if (context != nullptr) delete context;
	if (loader != nullptr) delete loader;
	context = new ContextInternal();
	// Set up the assembler/pcode-translator
	//if (trans != nullptr)
		//trans->reset(&loader, context);
	//else
	loader = new CallbackLoadImage(callback);

	DocumentStorage docstorage;
	if (lastsleighfile != sleighfilename) {
		lastsleighfile = sleighfilename;
		if (trans != nullptr) delete trans;
		trans = new Sleigh(loader, context);
		// Read sleigh file into DOM
		Element* sleighroot;
		try {
			sleighroot = docstorage.openDocument(sleighfilename)->getRoot();
		} catch (XmlError& err) {
			throw DecompError("Sleigh xml file not found or could not be parsed " + err.explain);
		}
		docstorage.registerTag(sleighroot);
		trans->initialize(docstorage); // Initialize the translator
	} else {
		trans->reset(loader, context);
		trans->initialize(docstorage);
	}

	for (std::map<std::string, XmlPcodeEmit*>::iterator it = callFixupMap.begin();
		it != callFixupMap.end(); it++) {
		delete it->second;
	}
	callFixupMap.clear();
	fixupTargetMap.clear();
	for (std::map<std::string, XmlPcodeEmit*>::iterator it = callFixupOtherMap.begin();
		it != callFixupOtherMap.end(); it++) {
		delete it->second;
	}
	callFixupOtherMap.clear();
	for (std::map<std::string, XmlPcodeEmit*>::iterator it = callMechMap.begin();
		it != callMechMap.end(); it++) {
		delete it->second;
	}
	callMechMap.clear();
	for (std::map<std::string, XmlPcodeEmit*>::iterator it = callExecPcodeMap.begin();
		it != callExecPcodeMap.end(); it++) {
		delete it->second;
	}
	callExecPcodeMap.clear();

	uniqueBase = trans->getUniqueBase(); //for pcode injections
}

void DecompInterface::setup(DecompileCallback* cb, std::string sleighfilename,
	std::string pspecfilename, std::string cspecfilename, std::vector<CoreType>& coreTypes,
	Options opt, int timeout, int maxpload)
{
	setupTranslator(cb, sleighfilename);
	std::string szCoreTypes = "    <coretypes>\n";
	for (std::vector<CoreType>::iterator it = coreTypes.begin(); it != coreTypes.end(); it++) {
		it->id = hashName(it->name);
		if (it->name == "void") szCoreTypes += "        <void/>\n";
		else szCoreTypes += "        <type name=\"" + it->name + "\" size=\"" +
			std::to_string(it->size) + "\" metatype=\"" + it->metaType +
			std::string(it->isChar ? "\" char=\"true" : "") +
			std::string(it->isUtf ? "\" utf=\"true" : "") +
			"\" id=\"" + std::to_string(it->id) + "\"/>\n";
	}
	szCoreTypes += "    </coretypes>";
	coretypesxml = szCoreTypes;
	cspecxml = readFileAsString(cspecfilename);
	pspecxml = readFileAsString(pspecfilename);
	
	std::vector<InitStateItem> inits;
	cb->getInits(inits);
	//no good way to do this in a future-proof way as no protected members, no good accessor methods, XML looks the best
	partmap<Address, TrackedSet> newtrackmap;
	std::string str;
	for (int i = 0; i < inits.size(); i++) {
		/*str += "<tracked_pointset space=\"" + inits[i].space1 + "\" offset=\"" + std::to_string(inits[i].offset1) + "\" size=\"" +
			std::to_string(inits[i].offset2 - inits[i].offset1) + "\">" "<set space=\"" + inits[i].space + "\" offset=\"" +
			std::to_string(inits[i].offset) + "\" val=\"" + std::to_string(inits[i].val) + "\"/>"
			"</tracked_pointset>";*/
		Address addr1(trans->getSpaceByName(inits[i].space1), inits[i].offset1);
		Address addr2(trans->getSpaceByName(inits[i].space2), inits[i].offset2);
		newtrackmap.split(addr1);
		newtrackmap.split(addr2);
		for (partmap<Address, TrackedSet>::iterator it = newtrackmap.begin(addr1);
			it != newtrackmap.begin(addr2); it++) {
			/*const TrackedSet oldts = context->getTrackedSet(it->first); //make copy as the temporary hole cleared will destroy anything in here presumably
			for (int i = 0; i < oldts.size(); i++) {
				it->second.push_back(oldts[i]);
			}*/
			it->second.push_back(TrackedContext({ VarnodeData{ trans->getSpaceByName(inits[i].space),
				inits[i].offset, inits[i].size}, inits[i].val }));
		}
	}

	// Now that context symbol names are loaded by the translator
	// we can set the default context
	//Ghidra/Processors/x86/data/languages/x86-64.pspec context_set:
	istringstream ss(pspecxml);
	Document* doc;
	try {
		doc = xml_tree(ss);
	} catch (XmlError& err) {
		throw DecompError("Processor spec file not found or could not be parsed: " + err.explain);
	}
	Element* el = doc->getRoot();
	const List& list(el->getChildren());
	List::const_iterator iter;
	for (iter = list.begin(); iter != list.end(); ++iter) {
		el = *iter;
		if (el->getName() == "context_data") {
			context->restoreFromSpec(el, trans); //although tracked set need not be done here since its merged with other inits later, context needed still
			/*const List& lst(el->getChildren());
			for (iter = lst.begin(); iter != lst.end(); ++iter) {
				el = *iter;
				if (el->getName() == "context_set" || el->getName() == "tracked_set") {
					bool bContext = el->getName() == "context_set";
					const List& lt(el->getChildren());
					for (iter = lt.begin(); iter != lt.end(); ++iter) {
						el = *iter;
						if (bContext && el->getName() == "set") {
							context->setVariableDefault(el->getAttributeValue("name"), strtoull(el->getAttributeValue("val").c_str(), nullptr, 10));
						}
					}
					//break;
				}
			}*/
			const List& lst(el->getChildren());
			for (iter = lst.begin(); iter != lst.end(); ++iter) {
				el = *iter;
				int4 size;
				Range range;
				range.restoreXml(el, trans); // There MUST be a range
				Address addr1, addr2;
				addr1 = range.getFirstAddr();
				addr2 = range.getLastAddrOpen(trans);
				if (el->getName() == "context_set" || el->getName() == "tracked_set") {
					bool bContext = el->getName() == "context_set";
					const List& lt(el->getChildren());
					for (List::const_iterator itr = lt.begin(); itr != lt.end(); ++itr) {
						el = *itr;
						if (!bContext && el->getName() == "set") {
							//context->setVariableDefault(el->getAttributeValue("name"), strtoull(el->getAttributeValue("val").c_str(), nullptr, 10));
							Address addrinner = Address::restoreXml(el, trans, size);
							for (partmap<Address, TrackedSet>::iterator it = newtrackmap.begin(addr1);
								it != newtrackmap.begin(addr2); it++) {
								it->second.push_back(TrackedContext({ VarnodeData{ addrinner.getSpace(),
									addrinner.getOffset(), (uint4)size},
									strtoull(el->getAttributeValue("val").c_str(), nullptr, 10) }));
							}
						}
					}
					//break;
				}
			}
			break;
		} else if (el->getName() == "jumpassist") {
			//EXECUTABLEPCODE_TYPE is for Dalvik/JVM
			//pspec has optional jumpassist -> with optional case_pcode, addr_pcode, default_pcode, size_pcode each with <input name="" size=""/>...<output name="" size=""/><body>[CDATA]
			std::string name = el->getAttributeValue("name");
			const List& lst(el->getChildren());
			for (iter = lst.begin(); iter != lst.end(); ++iter) {
				el = *iter;
				std::string finalname;
				if (el->getName() == "case_pcode") {
					finalname = name + "_index2case";
				} else if (el->getName() == "addr_pcode") {
					finalname = name + "_index2addr";
				} else if (el->getName() == "default_pcode") {
					finalname = name + "_defaultaddr";
				} else if (el->getName() == "size_pcode") {
					finalname = name + "_calcsize";
				}
				List::const_iterator t;
				const List& l(el->getChildren());
				std::string body;
				std::vector<std::pair<std::string, int>> inputs;
				std::vector<std::pair<std::string, int>> outputs;
				for (t = l.begin(); t != l.end(); ++t) {
					if ((*t)->getName() == "input") { //name, size
						inputs.push_back(std::pair<std::string, int>((*t)->getAttributeValue("name"), strtoull((*t)->getAttributeValue("size").c_str(), nullptr, 10)));
					} else if ((*t)->getName() == "output") { //name, size
						outputs.push_back(std::pair<std::string, int>((*t)->getAttributeValue("name"), strtoull((*t)->getAttributeValue("size").c_str(), nullptr, 10)));
					} else if ((*t)->getName() == "body") {
						body = (*t)->getContent();
					}
				}
				if (finalname.size() != 0 && body.size() != 0) callExecPcodeMap[finalname] = getPcodeSnippet(body, inputs, outputs);
			}
		}
	}
	delete doc;

	for (partmap<Address, TrackedSet>::iterator it = newtrackmap.begin();
		it != newtrackmap.end(); it++) {
		str += "<tracked_pointset space=\"" + it->first.getSpace()->getName() +
			"\" offset=\"" + std::to_string(it->first.getOffset()) +
			//"\" size=\"" + std::to_string(it->first.getOffset()) +
			"\">";
		for (int i = 0; i < it->second.size(); i++) {
			str += "<set space=\"" + it->second[i].loc.space->getName() + "\" offset=\"" +
				std::to_string(it->second[i].loc.offset) + "\" size=\"" +
				std::to_string(it->second[i].loc.size) + "\" val=\"" +
				std::to_string(it->second[i].val) + "\"/>";
		}
		str += "</tracked_pointset>";
	}
	istringstream st("<context_data>" + str + "</context_data>");
	try {
		doc = xml_tree(st);
	} catch (XmlError& err) {
		throw DecompError("Newly built initial tracking data could not be parsed: " + err.explain);
	}
	context->restoreXml(doc->getRoot(), trans);
	delete doc;
	/*for (partmap<Address, TrackedSet>::iterator it = newtrackmap.begin(); it != newtrackmap.end(); it++) {
		TrackedSet& ts(context->createSet(it->first, it->first));
		for (int i = 0; i < it->second.size(); i++) {
			ts.push_back(it->second[i]);
		}
	}*/

	toutSecs = timeout;
	maxResultSizeMBYtes = maxpload;
	//setShowNamespace(xmlOptions.isDisplayNamespaces());

	istringstream s(cspecxml);
	try {
		doc = xml_tree(s);
	} catch (XmlError& err) {
		throw DecompError("Compiler spec file not found or could not be parsed: " + err.explain);
	}
	callStyles.clear();
	std::map<std::string, bool> globalSpaces;
	el = doc->getRoot();
	const List& l(el->getChildren());
	for (iter = l.begin(); iter != l.end(); ++iter) {
		el = *iter;
		if (el->getName() == "default_proto") {
			const List& lst(el->getChildren());
			List::const_iterator it;
			for (it = lst.begin(); it != lst.end(); ++it) {
				el = *it; //el->getName() == "prototype"
				callStyle = el->getAttributeValue("name");
				callStyles[el->getAttributeValue("name")] =
					strtoull(el->getAttributeValue("extrapop").c_str(), nullptr, 10);
				//<pcode inject="uponentry"/"uponreturn" dynamic="true"/"false"><body></pcode>
				//break;
			}
			//break;
		} else if (el->getName() == "prototype") {
			//callStyle = el->getAttributeValue("name");
			//CALLMECHANISM_TYPE is for Dalvik/JVM all so far are dynamic
			//input, output, pcode inject="uponentry/uponreturn" dynamic="true/false" then use el->getAttributeValue("name") + "@@inject_uponentry" or "@@inject_uponreturn"; -> body
			callStyles[el->getAttributeValue("name")] =
				strtoull(el->getAttributeValue("extrapop").c_str(), nullptr, 10);
			const List& lst(el->getChildren());
			List::const_iterator it;
			for (it = lst.begin(); it != lst.end(); ++it) {
				Element* e = *it;
				if (e->getName() == "pcode") {
					for (size_t i = 0; i < e->getNumAttributes(); i++) {
						if (e->getAttributeName(i) == "dynamic") {
							if (e->getAttributeValue(i) == "true")
								callMechMap[el->getAttributeValue("name") + "@@inject_" + e->getAttributeValue("inject")] = new XmlPcodeEmit;
							break;
						}
					}
					List::const_iterator t;
					const List& l(e->getChildren());
					for (t = l.begin(); t != l.end(); ++t) {
						if ((*t)->getName() == "body") {
							callMechMap[el->getAttributeValue("name") + "@@inject_" + e->getAttributeValue("inject")] =
								getPcodeSnippet((*t)->getContent(),
									std::vector<std::pair<std::string, int>>(),
									std::vector<std::pair<std::string, int>>());
						}
					}
				}
			}
			//<pcode inject="uponentry"/"uponreturn" dynamic="true"/"false"><body></pcode>
		} else if (el->getName() == "resolveprototype") {
			callStyles[el->getAttributeValue("name")] = -1;
		} else if (el->getName() == "stackpointer") {
			stackPointerReg = el->getAttributeValue("register");
			//stackPointerSpace = el->getAttributeValue("space");
		} else if (el->getName() == "callfixup") { //CALLFIXUP_TYPE - wider use
			//target name=..., pcode -> body[CDATA]
			const List& lst(el->getChildren());
			List::const_iterator it;
			intb paramShift = 0;
			for (int i = 0; i < el->getNumAttributes(); i++) {
				if (el->getAttributeName(i) == "paramshift")
					paramShift = strtoll(el->getAttributeValue(i).c_str(), nullptr, 10);
			}
			for (it = lst.begin(); it != lst.end(); ++it) {
				Element* e = *it;
				if (e->getName() == "target") {
					fixupTargetMap[e->getAttributeValue("name")] = el->getAttributeValue("name");
				} else if (e->getName() == "pcode") {
					List::const_iterator t;
					const List& l(e->getChildren());
					for (t = l.begin(); t != l.end(); ++t) {
						if ((*t)->getName() == "body") {
							callFixupMap[el->getAttributeValue("name")] =
								getPcodeSnippet((*t)->getContent(),
									std::vector<std::pair<std::string, int>>(),
									std::vector<std::pair<std::string, int>>());
							callFixupMap[el->getAttributeValue("name")]->paramShift = paramShift;
						}
					}
				}
			}
		} else if (el->getName() == "callotherfixup") { //CALLOTHERFIXUP_TYPE is for Dalvik/JVM all so far are dynamic
			//targetop="" -> pcode -> body[CDATA]
			const List& lst(el->getChildren());
			List::const_iterator it;
			for (it = lst.begin(); it != lst.end(); ++it) {
				Element* e = *it;
				if (e->getName() == "pcode") { //dynamic="true" means no body will appear only input/output!
					for (size_t i = 0; i < e->getNumAttributes(); i++) {
						if (e->getAttributeName(i) == "dynamic") {
							if (e->getAttributeValue(i) == "true")
								callFixupOtherMap[el->getAttributeValue("name")] = new XmlPcodeEmit;
							break;
						}
					}
					List::const_iterator t;
					const List& l(e->getChildren());
					std::string body;
					std::vector<std::pair<std::string, int>> inputs;
					std::vector<std::pair<std::string, int>> outputs;
					for (t = l.begin(); t != l.end(); ++t) {
						if ((*t)->getName() == "input") { //name, size
							inputs.push_back(std::pair<std::string, int>((*t)->getAttributeValue("name"), strtoull((*t)->getAttributeValue("size").c_str(), nullptr, 10)));
						} else if ((*t)->getName() == "output") { //name, size
							outputs.push_back(std::pair<std::string, int>((*t)->getAttributeValue("name"), strtoull((*t)->getAttributeValue("size").c_str(), nullptr, 10)));
						} else if ((*t)->getName() == "body") {
							body = (*t)->getContent();
						}
					}
					if (body.size() != 0) {
						callFixupOtherMap[el->getAttributeValue("targetop")] = getPcodeSnippet(body, inputs, outputs);
					} else {
						callFixupOtherMap[el->getAttributeValue("targetop")]->inputs = inputs;
						callFixupOtherMap[el->getAttributeValue("targetop")]->outputs = outputs;
					}
				}
			}
		} else if (el->getName() == "global") {
			const List& lst(el->getChildren());
			List::const_iterator it;
			for (it = lst.begin(); it != lst.end(); ++it) {
				Element* e = *it;
				if (e->getName() == "range") {
					size_t i;
					for (i = 0; i < e->getNumAttributes(); i++) {
						if (e->getAttributeName(i) == "first" || e->getAttributeName(i) == "last") break;
					}
					if (i == e->getNumAttributes()) globalSpaces[e->getAttributeValue("space")] = true;
				}
			}
		}
	}
	std::string procSpaces = "<spaces defaultspace=\"" + trans->getDefaultSpace()->getName() + "\">";
	for (int i = 0; i < trans->numSpaces(); i++) {
		//trans->saveXml(os);
		AddrSpace* as = trans->getSpace(i);
		if (as->getName() == "const") continue; //const space is special and cannot be registered again
		//deadcodedelay if delay != deadcodedelay
		//wordsize if wordsize > 1
		//global is specified in the cspec file e.g. -> <global> -> <range space="ram"/>
		procSpaces += "<" + std::string(as->getName() == "unique" ? "space_unique" : "space") +
			" name=\"" + as->getName() + "\" index=\"" + std::to_string(i) +//as->getIndex();
			"\" bigendian=\"" + std::string(as->isBigEndian() ? "true" : "false") +
			"\" delay=\"" + std::to_string(as->getDelay()) +
			std::string(as->getDelay() != as->getDeadcodeDelay() ? "\" deadcodedelay=\"" + std::to_string(as->getDeadcodeDelay()) : "") +
			"\" size=\"" + std::to_string(as->getAddrSize()) +
			std::string(as->getWordSize() > 1 ? "\" wordsize=\"" + std::to_string(as->getWordSize()) : "") +
			"\" physical=\"" + std::string(as->hasPhysical() ? "true" : "false") +
			"\" global=\"" + std::string(globalSpaces.find(as->getName()) != globalSpaces.end() ? "true" : "false") + "\"/>"; //as->getName() == "ram"
	}
	procSpaces += "</spaces>";
	uintm uniqBase = 0x10000000; //trans->getUniqueBase()
	//the problem is the unique base at trans->getUniqueBase() will be used for pcode injections so 0x10000000 is considered a safe distance to allow for an arbitrary amount of them
	tspecxml = "<sleigh bigendian=\"" + std::string(trans->isBigEndian() ? "true" : "false") +
		"\" uniqbase=\"0x" + to_string(uniqBase, hex) + "\">\n" + procSpaces + "</sleigh>";

	delete doc;
	xmlOptions = getOptions(opt);
}

/**
 * Initialize decompiler for a particular platform
 * @param cback = callback object for decompiler
 * @param pspecxml = string containing .pspec xml
 * @param cspecxml = string containing .cspec xml
 * @param tspecxml = XML string containing translator spec
 */
void DecompInterface::registerProgram()
{
	//callback.setShowNamespace(showNamespace);
	//callback->status("setup");
	callback->launchDecompiler();
	statusGood = true;
	std::vector<uchar> revec;
	try {
		write(command_start, sizeof(command_start));
		writeString("registerProgram");
		writeString(pspecxml); //*.pspec
		writeString(cspecxml); //*.cspec
		writeString(tspecxml); //Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/app/plugin/processors/sleigh/SleighLanguage.java
		writeString(coretypesxml); //Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/program/model/pcode/PcodeDataTypeManager.java
		write(command_end, sizeof(command_end));
		revec = readResponse();
	}
	catch (DecompError& /*e*/) {
		statusGood = false;
		throw;
	}
	archId = strtol(std::string(revec.begin(), revec.end()).c_str(), nullptr, 10);
	std::vector<uchar> buf = sendCommand1Param("setOptions", xmlOptions);
	//cb->status("setOptions " + std::string(buf.begin(), buf.end()));
	if (std::string(buf.begin(), buf.end()) != "t") {
		statusGood = false;
		throw DecompError("Did not accept decompiler options");
	}
}

/**
 * Free decompiler resources
 */
int DecompInterface::deregisterProgram() {
	if (!statusGood) {
		throw DecompError("deregisterProgram called on bad process");
	}
	statusGood = false;
	// Once a program is deregistered, the process is never
	// used again
	write(command_start, sizeof(command_start));
	writeString("deregisterProgram");
	writeString(std::to_string(archId));
	write(command_end, sizeof(command_end));
	std::vector<uchar> revec = readResponse();
	return strtol(std::string(revec.begin(), revec.end()).c_str(), nullptr, 10);
}

/**
 * Send a single command to the decompiler with no parameters and return response
 * @param command is the name of the command to execute
 * @return the response String
 */
std::vector<uchar> DecompInterface::sendCommand(std::string command) {
	if (!statusGood) {
		throw DecompError(command + " called on bad process");
	}
	std::vector<uchar> resbuf;
	try {
		write(command_start, sizeof(command_start));
		writeString(command);
		writeString(std::to_string(archId));
		write(command_end, sizeof(command_end));
		resbuf = readResponse();
	}
	catch (DecompError& /*e*/) {
		statusGood = false;
		throw;
	}
	return resbuf;
}

/**
 * @param command the decompiler should execute
 * @param param an additional parameter for the command
 * @param timeoutSecs the number of seconds to run before timing out
 * @return the response string
 */
std::vector<uchar> DecompInterface::sendCommand1ParamTimeout(std::string command,
	std::string param, int timeoutSecs) {
	if (!statusGood) {
		throw DecompError(command + " called on bad process");
	}
	std::vector<uchar> resbuf;
	std::string err;
	std::packaged_task<std::vector<uchar>(std::string, std::string)>
		task([&err, this] (std::string command, std::string param) -> std::vector<uchar> {
		try {
			write(command_start, sizeof(command_start));
			writeString(command);
			writeString(std::to_string(archId));
			writeString(param);
			write(command_end, sizeof(command_end));
			return readResponse();
		} catch (BadDataError& e) {
			err = e.explain;
			statusGood = false;
			return std::vector<uchar>();
		} catch (SleighError& e) { //register lookup error
			err = e.explain;
			statusGood = false;
			return std::vector<uchar>();
		} catch (DecompError& e) {
			err = e.explain;
			statusGood = false;
			return std::vector<uchar>();
		}
	});
	std::future<std::vector<uchar>> future = task.get_future(); //crash occurs here
	std::thread t(std::move(task), command, param);
	//GetProcessMemoryInfo?
	std::future_status status = future.wait_for(std::chrono::seconds(timeoutSecs));
	if (status != std::future_status::ready) {
		callback->terminate();
		//does not work the process must close its end, not the local end
		//_close(nativeIn); //close read handle to cause thread to exit, closing again will just return -1
		//further after closed, read and write will cause assertion failures - correct only to terminate the process then after thread close, close the handles
		//*nativeIn = -1;
	}
	resbuf = future.get();
	t.join();
	if (status != std::future_status::ready) {
		statusGood = false;
		throw DecompError("process timeout");
	}
	if (err.size() != 0) throw DecompError(err);
	return resbuf;
}

/**
 * Send a command with 2 parameters to the decompiler and read the result
 * @param command string to send
 * @param param1  is the first parameter string
 * @param param2  is the second parameter string
 * @return the result string
 */
std::vector<uchar> DecompInterface::sendCommand2Params(std::string command,
	std::string param1, std::string param2)
{
	if (!statusGood) {
		throw DecompError(command + " called on bad process");
	}
	std::vector<uchar> resbuf;
	try {
		write(command_start, sizeof(command_start));
		writeString(command);
		writeString(std::to_string(archId));
		writeString(param1);
		writeString(param2);
		write(command_end, sizeof(command_end));
		resbuf = readResponse();
	}
	catch (DecompError& /*e*/) {
		statusGood = false;
		throw;
	}
	return resbuf;
}

void DecompInterface::setMaxResultSize(int maxResultSizeMBytes) {
	maxResultSizeMBYtes = maxResultSizeMBytes;
}

void DecompInterface::setShowNamespace(bool showNamespace) {
	showNamespace = showNamespace;
	//callback.setShowNamespace(showNamespace);
}

/**
 * Send a command to the decompiler with one parameter and return the result
 * @param command is the command string
 * @param param1 is the parameter as a string
 * @return the result string
 */
std::vector<uchar> DecompInterface::sendCommand1Param(std::string command, std::string param1)
{
	if (!statusGood) {
		throw DecompError(command + " called on bad process");
	}
	std::vector<uchar> resbuf;
	try {
		write(command_start, sizeof(command_start));
		writeString(command);
		writeString(std::to_string(archId));
		writeString(param1);
		write(command_end, sizeof(command_end));
		resbuf = readResponse();
	} catch (DecompError& /*e*/) {
		statusGood = false;
		throw;
	}
	return resbuf;
}

/**
* This allows the application to the type of analysis
* performed by the decompiler, by giving the name of
* an analysis class. Right now, there are a few
* predefined classes. But there soon may be support
* for applications to define their own class and
* tailoring the decompiler's behaviour for that class.
* <p>
* The current predefined analysis class are:
* <ul>
*   <li>"decompile" - this is the default, and performs all
*      analysis steps suitable for producing C code.
*   <li>"normalize" - omits type recovery from the analysis
*      and some of the final clean-up steps involved in
*      making valid C code.  It is suitable for creating
*      normalized pcode syntax trees of the dataflow.
*   <li>"firstpass" - does no analysis, but produces an
*      unmodified syntax tree of the dataflow from the
*   <li>"register" - does ???.
*   <li>"paramid" - does required amount of decompilation
*      followed by analysis steps that send parameter
*      measure information for parameter id analysis.
*      raw pcode.
* </ul>
*
* <p>
* This property should ideally be set once before the
* openProgram call is made, but it can be used repeatedly
* if the application needs to change analysis style in the
* middle of a sequence of decompiles.  Unless the style
* changes, the method does NOT need to be called repeatedly.
* Even after a crash, the new decompiler process will
* automatically configured with the cached style value.
*
* @param actionstring "decompile"|"normalize"|"register"|"firstpass"|"paramid"
* @return true - if the decompiler process was successfully configured
*/
bool DecompInterface::setSimplificationStyle(std::string actionstring) {
	//actionname = actionstring;
	// Property can be set before process exists
	//if (decompProcess == nullptr) {
	//	return true;
	//}
	try {
		//verifyProcess();
		std::vector<uchar> buf = sendCommand2Params("setAction", actionstring, "");
		return std::string(buf.begin(), buf.end()).c_str() == "t";
	}
	catch (DecompError& /*e*/) {
		// don't care
		throw;
	}
	//stopProcess();
	return false;
}

class TreeHandlerWhitespace : public ContentHandler {
	Element* root;
	Element* cur;
	string error;
public:
	TreeHandlerWhitespace(Element* rt) { root = rt; cur = root; }
	virtual ~TreeHandlerWhitespace(void) {}
	virtual void setDocumentLocator(Locator locator) {}
	virtual void startDocument(void) {}
	virtual void endDocument(void) {}
	virtual void startPrefixMapping(const string& prefix, const string& uri) {}
	virtual void endPrefixMapping(const string& prefix) {}
	virtual void startElement(const string& namespaceURI, const string& localName,
		const string& qualifiedName, const Attributes& atts)
	{
		Element* newel = new Element(cur);
		cur->addChild(newel);
		cur = newel;
		newel->setName(localName);
		for (int4 i = 0; i < atts.getLength(); ++i)
			newel->addAttribute(atts.getLocalName(i), atts.getValue(i));
	}
	virtual void endElement(const string& namespaceURI, const string& localName,
		const string& qualifiedName)
	{
		cur = cur->getParent();
	}
	virtual void characters(const char* text, int4 start, int4 length)
	{
		cur->addContent(text, start, length);
	}
	virtual void ignorableWhitespace(const char* text, int4 start, int4 length)
	{
		cur->addContent(text, start, length);
	}
	virtual void processingInstruction(const string& target, const string& data) {}
	virtual void setVersion(const string& val) {}
	virtual void setEncoding(const string& val) {}
	virtual void skippedEntity(const string& name) {}
	virtual void setError(const string& errmsg) { error = errmsg; }
	const string& getError(void) const { return error; }
};

Document* xml_tree_whitespace(istream& i)
{
	Document* doc = new Document();
	TreeHandlerWhitespace handle(doc);
	if (0 != xml_parse(i, &handle)) {
		delete doc;
		throw XmlError(handle.getError());
	}
	return doc;
}

string getHasAttributeValue(Element* pEl, const string& nm)
{
	for (uint4 i = 0; i < pEl->getNumAttributes(); ++i)
		if (pEl->getAttributeName(i) == nm)
			return pEl->getAttributeValue(i);
	return "";
}

std::string DecompInterface::convertSourceDoc(Element* el,
	std::string & displayXml, std::string& funcProto)
{
	//Ghidra/Features/Decompiler/src/decompile/cpp/prettyprint.cc
	//colors are keyword, comment, type, funcname, var, const, param and global
	std::string s;
	const List& list(el->getChildren());
	List::const_iterator iter;
	for (iter = list.begin(); iter != list.end(); ++iter) {
		el = *iter;
		//"clang_document"
		if (el->getName() == "function") { //syntax tree is followed by source code
			s += convertSourceDoc(el, displayXml, funcProto);
		} else if (el->getName() == "block") {
			s += convertSourceDoc(el, displayXml, funcProto);
		} else if (el->getName() == "statement") {
			s += convertSourceDoc(el, displayXml, funcProto);
		} else if (el->getName() == "funcproto") {
			funcProto = convertSourceDoc(el, displayXml, funcProto);
			s += funcProto;
		} else if (el->getName() == "return_type") {
			s += convertSourceDoc(el, displayXml, funcProto);
		} else if (el->getName() == "vardecl") {
			s += convertSourceDoc(el, displayXml, funcProto);
		} else if (el->getName() == "syntax") {
			//el->getAttributeValue("open");
			//el->getAttributeValue("close");
			//"keyword"
			s += el->getContent();
			displayXml += callback->emit(el->getName(), getHasAttributeValue(el, "color"), el->getContent());
		} else if (el->getName() == "break") {
			uint8 idx = strtoull(el->getAttributeValue("indent").c_str(), nullptr, 16);
			s += "\n";
			displayXml += "\n";
			while (idx--) {
				s += " "; displayXml += " ";
			}
		} else if (el->getName() == "comment") {
			//"comment"
			//el->getAttributeValue("space"); //e.g. "ram"
			//el->getAttributeValue("ram"); //e.g. "0x140001010"
			s += el->getContent();
			displayXml += callback->emit(el->getName(), getHasAttributeValue(el, "color"), el->getContent());
		} else if (el->getName() == "variable") {
			//"param", "const", "var"
			s += el->getContent();
			displayXml += callback->emit(el->getName(), getHasAttributeValue(el, "color"), el->getContent());
		} else if (el->getName() == "type") {
			//"type"
			//el->getAttributeValue("id"); //szCoreTypes ids given if non-zero only
			s += el->getContent();
			displayXml += callback->emit(el->getName(), getHasAttributeValue(el, "color"), el->getContent());
		} else if (el->getName() == "op") {
			//"keyword"
			s += el->getContent();
			displayXml += callback->emit(el->getName(), getHasAttributeValue(el, "color"), el->getContent());
		} else if (el->getName() == "funcname") {
			//"funcname"
			s += el->getContent();
			displayXml += callback->emit(el->getName(), getHasAttributeValue(el, "color"), el->getContent());
		} else if (el->getName() == "label") {
			s += el->getContent();
			displayXml += callback->emit(el->getName(), getHasAttributeValue(el, "color"), el->getContent());
		} else if (el->getName() == "field") {
			s += el->getContent();
			displayXml += callback->emit(el->getName(), getHasAttributeValue(el, "color"), el->getContent());
		} else {

		}
	}
	return s;
}

int DecompInterface::coreTypeLookup(size_t size, std::string metatype)
{
	//special size handling for void=0, char=1/2, bool=1, code=1???
	for (int i = 0; i < numDefCoreTypes; i++) {
		if (defaultCoreTypes[i].metaType == metatype) {
			if (defaultCoreTypes[i].size == size) return i;
		}
	}
	return -1;
}

int DecompInterface::regNameToIndex(std::string regName)
{
//	try {
		VarnodeSymbol* sym = (VarnodeSymbol*)trans->findSymbol(regName);
		if (sym != nullptr && sym->getType() == SleighSymbol::varnode_symbol)
			return sym->getFixedVarnode().offset;
		//return trans->getRegister(regName).offset;
	//} catch (SleighError&) {
		transform(regName.begin(), regName.end(), regName.begin(),
			islower(regName[0]) ? ::toupper : ::tolower);
		sym = (VarnodeSymbol*)trans->findSymbol(regName);
		if (sym != nullptr && sym->getType() == SleighSymbol::varnode_symbol)
			return sym->getFixedVarnode().offset;
		return -1; //trans->getRegister(regName).offset; //case-sensitive
	//}
}

std::string DecompInterface::getRegisterFromIndex(unsigned long long offs, int size)
{
	return trans->getRegisterName(trans->getSpaceByName("register"), offs, size);
}

//language id and compiler id from ldefs can be used for loading the patterns (for byte searching to find functions though - not a decompiler level issue):
//data/patterns/patternconstraints.xml
//patternconstraints -> language id="" -> compiler id="" -> <patternfile>name</patternfile>
std::string DecompInterface::doDecompile(DecMode dm, std::string base,
	unsigned long long DecompAddress, std::string & displayXml, std::string& funcProto)
{
	std::vector<uchar> buf;
	setSimplificationStyle(dm.actionname);
	if (dm.actionname.empty()) {
		throw DecompError("Decompile action not specified");
	}
	//Ghidra/Features/Decompiler/src/decompile/cpp/ghidra_process.hh
	//"normalize", "jumptable", "paramid", "register", "firstpass"
	if (dm.actionname != "decompile") {
		buf = sendCommand2Params("setAction", dm.actionname, "");
		if (std::string(buf.begin(), buf.end()) != "t") {
			throw DecompError("Could not set decompile action");
		}
	}
	//run with noc and notree to just get a document with a function prototype doc -> function -> addr/localdb/prototype as used for queries
	if (!dm.printSyntaxTree) {
		buf = sendCommand2Params("setAction", "", "notree"); //"true"
		if (std::string(buf.begin(), buf.end()) != "t") {
			throw DecompError("Could not turn off syntax tree");
		}
	}
	if (!dm.printCCode) {
		buf = sendCommand2Params("setAction", "", "noc"); //"c"
		if (std::string(buf.begin(), buf.end()) != "t") {
			throw DecompError("Could not turn off C printing");
		}
	}
	if (dm.sendParamMeasures) {
		buf = sendCommand2Params("setAction", "", "parammeasures"); //"noparammeasures"
		if (std::string(buf.begin(), buf.end()) != "t") {
			throw DecompError("Could not turn on sending of parameter measures");
		}
	}
	if (dm.jumpLoad) {
		buf = sendCommand2Params("setAction", "", "jumpload"); //"nojumpload"
		if (std::string(buf.begin(), buf.end()) != "t") {
			throw DecompError("Could not turn on jumptable loads");
		}
	}
	lastdm = dm;
	startOffs = DecompAddress;
	buf = sendCommand1ParamTimeout("decompileAt", "<addr space=\"" + base +
		"\" offset=\"0x" + to_string(DecompAddress, hex) + "\"/>", toutSecs);
	std::string decompXml = std::string(buf.begin(), buf.end());
	if (decompXml.size() == 0) throw DecompError("Empty decompiler response due to error");
	istringstream str(decompXml);
	//doc -> function ->
	//<ast>
		//<varnodes> -> <addr... ref="#"> ...
		//<block index="0..n"> -> <rangelist> -> <range>...
		//					   -> <op code="#"><seqnum space="" offset="" uniq="#"> -> <addr ref="#"/>... </seqnum></op>...
		//<blockedge index="1..n"><edge end="0" rev="0"/><edge end="11" rev="0"/></blockedge>...
	//</ast>
	//<highlist> -> <high repref="#" class="local" symref="#">... -> <type... ref="#" core="true/false"> -> <addr>
	//<prototype ...>
	//if bad XML was returned due to a problem, should not crash here
	Document* doc = nullptr;
	try {
		doc = xml_tree_whitespace(str);
	} catch (XmlError& /*err*/) {
		throw DecompError("Unable to parse XML: " + decompXml);
	}
	decompXml = convertSourceDoc(doc->getRoot(), displayXml, funcProto);
	delete doc;
	/**
	* Tell the decompiler to clear any function and symbol
	* information it gathered from the database.  Its a good
	* idea to call this after any decompileFunction call,
	* as the decompile process caches and reuses this kind
	* of data, and there is no explicit method for keeping
	* the cache in sync with the data base. Currently the
	* return value has no meaning.
	* @return -1
	*/
	buf = sendCommand("flushNative");
	strtol(std::string(buf.begin(), buf.end()).c_str(), nullptr, 10);

	return decompXml;
}

bool DecompInterface::toggleSyntaxTree(bool val)
{
	lastdm.printSyntaxTree = val;
	std::vector<uchar> buf = sendCommand2Params("setAction", "", val ? "tree" : "notree");
	return std::string(buf.begin(), buf.end()) == "t";
}

bool DecompInterface::toggleCCode(bool val)
{
	lastdm.printCCode = val;
	std::vector<uchar> buf = sendCommand2Params("setAction", "", val ? "c" : "noc");
	return std::string(buf.begin(), buf.end()) == "t";
}

bool DecompInterface::toggleParamMeasures(bool val)
{
	lastdm.sendParamMeasures = val;
	std::vector<uchar> buf = sendCommand2Params("setAction", "", val ? "parammeasures" : "noparammeasures");
	return std::string(buf.begin(), buf.end()) == "t";
}

bool DecompInterface::toggleJumpLoads(bool val)
{
	lastdm.jumpLoad = val;
	std::vector<uchar> buf = sendCommand2Params("setAction", "", val ? "jumpload" : "nojumpload");
	return std::string(buf.begin(), buf.end()) == "t";
}

bool DecompInterface::setOptions(Options opt)
{
	xmlOptions = getOptions(opt);
	std::vector<uchar> buf = sendCommand1Param("setOptions", xmlOptions);
	//cb->status("setOptions " + std::string(buf.begin(), buf.end()));
	if (std::string(buf.begin(), buf.end()) != "t") {
		statusGood = false;
		throw DecompError("Did not accept decompiler options");
	}
	return true;
}

void DecompInterface::getProtoEvals(std::string cspec,
	std::vector<std::string>& vec, int* defaultIdx)
{
	ifstream s(cspec.c_str());
	Document* doc;
	try {
		doc = xml_tree(s);
	} catch (XmlError& /*err*/) {
		//throw DecompError("Compiler spec file not found or could not be parsed: " + err.explain);
		return;
	}
	Element* el = doc->getRoot();
	const List& list(el->getChildren());
	List::const_iterator iter;
	for (iter = list.begin(); iter != list.end(); ++iter) {
		el = *iter;
		if (el->getName() == "default_proto") {
			const List& lst(el->getChildren());
			List::const_iterator it;
			for (it = lst.begin(); it != lst.end(); ++it) {
				el = *it; //el->getName() == "prototype"
				*defaultIdx = vec.size();
				vec.push_back(el->getAttributeValue("name"));
				//break;
			}
			//break;
		} else if (el->getName() == "prototype") {
			//callStyle = el->getAttributeValue("name");
			//CALLMECHANISM_TYPE is for Dalvik/JVM all so far are dynamic
			//input, output, pcode inject="uponentry/uponreturn" dynamic="true/false" then use el->getAttributeValue("name") + "@@inject_uponentry" or "@@inject_uponreturn"; -> body
			vec.push_back(el->getAttributeValue("name"));
		} else if (el->getName() == "resolveprototype") {
			vec.push_back(el->getAttributeValue("name"));
		}
	}
}

//Ghidra/Processors/*/data/languages/*.ldefs
//ghidra_9.0.4\Ghidra\Processors>dir *.ldefs/s/b
//could narrow down with external tool, followed by bit size and endianness
void DecompInterface::getLangFiles(std::string processorpath, std::string externaltool,
	std::map<std::string, std::vector<int>>& toolMap,
	std::vector<LangInfo> & li) //tools such as "gnu", "IDA-PRO", "DWARF.register.mapping.file"
{
	std::vector<string> res;
	FileManage::directoryList(res, processorpath, false);
	for (std::vector<string>::iterator it = res.begin(); it != res.end(); it++) {
		std::vector<string> ldefs;
		FileManage::matchListDir(ldefs, ".ldefs", true, *it + "/data/languages/", false);
		for (std::vector<string>::iterator its = ldefs.begin(); its != ldefs.end(); its++) {
			ifstream s(its->c_str());
			Document* doc;
			try {
				doc = xml_tree(s);
			} catch (XmlError& /*err*/) {
				continue;
			}
			Element* el = doc->getRoot();
			const List& list(el->getChildren());
			List::const_iterator iter;
			for (iter = list.begin(); iter != list.end(); ++iter) {
				el = *iter;
				if (el->getName() == "language") {
					std::map<std::string, std::string> compilers;
					const List& lst(el->getChildren());
					List::const_iterator t;
					for (t = lst.begin(); t != lst.end(); ++t) {
						Element* e = *t;
						if (e->getName() == "compiler") {
							compilers[e->getAttributeValue("id")] = e->getAttributeValue("spec");
						} else if (e->getName() == "external_name") {
							if (e->getAttributeValue("tool") == externaltool) {
								std::string str = e->getAttributeValue("name");
								//if (toolMap.find(str) == toolMap.end()) toolMap[str].clear();// = std::vector<int>();
								toolMap[str].push_back(li.size());
							}
						}
					}
					li.push_back(LangInfo{ *it + "/data/languages/",
						el->getAttributeValue("processor"),
						el->getAttributeValue("endian") == "big",
						(int)strtol(el->getAttributeValue("size").c_str(), nullptr, 10),
						el->getAttributeValue("slafile"),
						el->getAttributeValue("processorspec"), compilers });
				}
			}
			delete doc;
		}
	}
}