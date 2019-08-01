#pragma once

#include <string>
#include <vector>
#include <map>

typedef unsigned char  uchar;   ///< unsigned 8 bit value

struct DecompError {
	std::string explain;		///< Explanatory string
	/// Initialize the error with an explanatory string
	DecompError(const std::string& s) { explain = s; }
};

struct LangInfo
{
	std::string basePath;
	std::string processor;
	bool bigEndian;
	int size;
	std::string slafile;
	std::string processorspec;
	std::map<std::string, std::string> compilers;
};

//Ghidra/Features/Decompiler/src/decompile/cpp/options.hh
//top level: "extrapop", "defaultprototype", "inline", "noreturn", "structalign"
//"warning", "hideextensions", "allowcontextset", "errorunimplemented"
//"errorreinterpreted", "errortoomanyinstructions", "jumprule", "togglerule"

//Ghidra/Features/Decompiler/src/decompile/cpp/options.cc
//decompile currentaction options: "base", "protorecovery", "protorecovery_a", "deindirect", "localrecovery",
//"deadcode", "typerecovery", "stackptrflow",
//"blockrecovery", "stackvars", "deadcontrolflow", "switchnorm",
//"cleanup", "merge", "dynamic", "casts", "analysis",
//"fixateglobals", "fixateproto",
//"segment", "returnsplit", "nodejoin", "doubleload", "doubleprecis",
//"unreachable", "subvar", "floatprecision",
//"conditionalexe"
//jumptable currentaction options: "base", "noproto", "localrecovery", "deadcode", "stackptrflow",
//"stackvars", "analysis", "segment", "subvar", "conditionalexe"
//normalize currentaction options:  "base", "protorecovery", "protorecovery_b", "deindirect", "localrecovery",
//"deadcode", "stackptrflow", "normalanalysis",
//"stackvars", "deadcontrolflow", "analysis", "fixateproto", "nodejoin",
//"unreachable", "subvar", "floatprecision", "normalizebranches",
//"conditionalexe"
//paramid currentaction options: "base", "protorecovery", "protorecovery_b", "deindirect", "localrecovery",
//"deadcode", "typerecovery", "stackptrflow", "siganalysis",
//"stackvars", "deadcontrolflow", "analysis", "fixateproto",
//"unreachable", "subvar", "floatprecision",
//"conditionalexe"
//register currentaction options: "base", "analysis", "subvar"
//firstpass currentaction options: "base"
struct Options
{
	bool conditionalexe;
	bool readonly;
	bool decompileUnreachable;
	bool decompileDoublePrecis;
	bool ignoreUnimplemented;
	bool inferConstPtr;
	bool nullPrinting;
	bool inPlaceOps;
	bool conventionPrinting;
	bool noCastPrinting;
	int maxLineWidth;
	int indentIncrement;
	int commentIndent;
	std::string commentStyle; //"c"
	bool commentInstructionHeader;
	bool commentUser2; //PRE
	bool commentUser1; //EOL
	bool commentUser3; //POST
	bool commentWarning;
	bool commentHeader;
	bool commentWarningHeader;
	std::string integerFormat; //"best"
	std::string setLanguage; //"c-language" - also has "java-language"
	std::string protoEval; //calling convention
};
struct AddrInfo
{
	std::string space;
	unsigned long long offset;
};
struct SizedAddrInfo
{
	AddrInfo addr;
	unsigned long long size;
	//std::string regName;
};
struct InitStateItem
{
	AddrInfo addr1;
	AddrInfo addr2;
	SizedAddrInfo addr;
	unsigned long long val;
};

struct DecMode
{
	std::string actionname; // Name of simplification action - "decompile"
	bool printSyntaxTree; // Whether syntax tree is returned
	bool printCCode; // Whether C code is returned
	bool sendParamMeasures; // Whether Parameter Measures are returned
	bool jumpLoad; // Whether jumptable load information is returned
};

struct CoreType
{
	std::string name;
	int size;
	std::string metaType;
	bool isChar;
	bool isUtf;
	long long id; //Datatype::hashName
};

extern const Options defaultOptions;
extern const DecMode defaultDecMode;
extern CoreType defaultCoreTypes[];
extern const int numDefCoreTypes;
extern const char* szCoreTypeDefs[];

struct RangeInfo
{
	std::string space;
	unsigned long long beginoffset;
	unsigned long long endoffset;
};

struct TypeInfo;

//name and argIndex not used for function return values
struct ParamInfo
{
	std::string name;
	std::vector<TypeInfo> ti;
};
struct SymInfo
{
	ParamInfo pi;
	SizedAddrInfo addr; //"ram", "stack", "join" but then need offsets for something like piece1="register:0x8:4" piece2="register:0x0:4"
	int argIndex; //-1 if local variable
	RangeInfo range; //for register space only with locals (for arguments, will automatically specify one less than the start address of the function)
	std::vector<SizedAddrInfo> joins; //for join space only
};
struct FuncProtoInfo
{
	bool isInline;
	bool isNoReturn;
	unsigned long long extraPop; //-1 for unknown
	std::string model; //unknown, default, __fastcall, __stdcall, __cdecl, __thiscall, __vectorcall, __pascal
	bool dotdotdot;
	bool customStorage;
	bool hasThis;
	bool isConstruct;
	bool isDestruct;
	SymInfo retType;
	std::vector<SymInfo> syminfo;
	std::vector<SizedAddrInfo> killedByCall;
};
struct StructMemberInfo;
struct ParamInfo;

//either only a typeName is present for reference or if size != -1 then all fields are present
struct TypeInfo
{
	std::string typeName;
	unsigned long long size = 0;
	std::string metaType;
	//following are mutually exclusive fields - union?
	bool isEnum = false; //uint
	bool isUtf = false; //wchar*
	bool isChar = false; //char
	unsigned long long arraySize = 0; //array
	std::vector<StructMemberInfo> structMembers; //struct
	FuncProtoInfo funcInfo; //code

	bool isReadOnly; //external use only for all except functions, structures, array - decompiler does not need - it tracks symbols not types better to create derived struct
};
struct StructMemberInfo
{
	std::string name;
	unsigned long long offset;
	std::vector<TypeInfo> ti;
};
struct CommentInfo
{
	AddrInfo addr;
	std::string type;
	std::string text;
};
enum MappedSymbolKinds
{
	KIND_HOLE = -1,
	KIND_FUNCTION = 0,
	KIND_DATA = 1,
	KIND_EXTERNALREFERENCE = 2,
	KIND_LABEL = 3
};
struct MappedSymbolInfo
{
	MappedSymbolKinds kind;
	
	bool readonly; //data, label, hole
	bool volatil; //data, label, hole
	
	std::string name; //function, data, extref, symbol
	
	std::vector<TypeInfo> typeChain; //data

	unsigned long long entryPoint; //function

	std::vector<RangeInfo> ranges; //function not at entry point

	//function at entry point:
	unsigned long long size; //minimally 1 or maximally must be contiguous block from entry point
	FuncProtoInfo func;
};
enum CPoolTag
{
	PRIMITIVE = 0,			// Constant -value- of datatype -type-
	STRING_LITERAL = 1,		// Constant reference to string in -token-
	CLASS_REFERENCE = 2,	// Reference to (system level) class object
	POINTER_METHOD = 3,		// Pointer to a method, name in -token-, signature in -type-
	POINTER_FIELD = 4,		// Pointer to a field, name in -token-, datatype in -type-
	ARRAY_LENGTH = 5,		// Integer length, -token- is language specific indicator, -type- is integral type
	INSTANCE_OF = 6,		// boolean value, -token- is language specific indicator, -type- is boolean type
	CHECK_CAST = 7			// Pointer to object, new name in -token-, new datatype in -type-
};
struct CPoolRecord
{
	CPoolTag tag;
	bool hasThis;
	bool constructor;
	unsigned long long value; //for PRIMITIVE
	std::vector<uchar> data;
	std::string token; //if and only if data empty
};
class DecompileCallback
{
public:
	virtual int getBytes(unsigned char* ptr, int size, AddrInfo addr) = 0;

	virtual void getMappedSymbol(AddrInfo addr, MappedSymbolInfo& msi) = 0;
	virtual void getExternInfo(AddrInfo addr, std::string& callName, std::string& modName, FuncProtoInfo& func) = 0;

	//type functions only needed if returned from the symbol queries
	//metatypes are ptr, array and struct beyond the core ones: void, int, uint, bool, code, float, unknown
	virtual void getMetaType(std::string typeName, std::vector<TypeInfo>& typeChain) = 0;

	//4 P-code injection callbacks, along with C Pool Ref callbacks are not yet implemented
	
	//valid types are "user1", "user2", "user3", "header", "warning", "warningheader"
	virtual void getComments(AddrInfo addr, std::vector<CommentInfo> & comments) = 0;
	virtual std::string getSymbol(AddrInfo addr) = 0;

	virtual std::string getPcodeInject(int type, std::string name, AddrInfo addr, std::string fixupbase, unsigned long long fixupoffset) = 0;
	virtual void getCPoolRef(const std::vector<unsigned long long>& refs, CPoolRecord& rec) = 0;

	//colors are Clang-based: "keyword", "comment", "type", "funcname", "var", "const", "param" and "global"
	virtual std::string emit(std::string type, std::string color, std::string str) = 0;

	virtual void getInits(std::vector<InitStateItem>& inits) = 0;
	virtual void launchDecompiler() = 0;
	virtual size_t readDec(void* Buf, size_t MaxCharCount) = 0;
	virtual size_t writeDec(void const* Buf, size_t MaxCharCount) = 0;
	virtual void terminate() = 0;
};

class Sleigh;
class ContextInternal;
class Element;
class VarnodeTpl;
class XmlPcodeEmit;
class CallbackLoadImage;

class DecompInterface
{

	DecompileCallback* callback;
	bool statusGood;     // true if decompiler process is running

	int archId = -1;              // architecture id for decomp process
	int maxResultSizeMBYtes = 50; // maximum result size in MBytes to allow from decompiler
	bool showNamespace;        // whether to show namespaces for functions

	// Initialization state
	DecMode lastdm = { "decompile", true, true, false, false };
	std::string xmlOptions; // Current decompiler options
	std::string callStyle;
	std::map<std::string, unsigned long long> callStyles;
	std::string stackPointerReg;
	//std::string stackPointerSpace;
	//std::map<std::string, std::string> registers;
	//std::map<int, std::string> userOpNames;
	//std::string procSpaces;
	//uint8 uniqBase;
	//bool bigEndian;
	AddrInfo startOffs;
	CallbackLoadImage* loader = nullptr;
	ContextInternal* context = nullptr;
	Sleigh* trans = nullptr;
	std::string lastsleighfile;
	std::string pspecxml;
	std::string tspecxml;
	std::string cspecxml;
	std::string coretypesxml;
	int toutSecs;

	unsigned long long uniqueBase;
	std::map<std::string, XmlPcodeEmit*> callFixupMap;
	std::map<std::string, XmlPcodeEmit*> callFixupOtherMap;
	std::map<std::string, XmlPcodeEmit*> callMechMap;
	std::map<std::string, XmlPcodeEmit*> callExecPcodeMap;
	std::map<std::string, std::string> fixupTargetMap;

	void processPcodeInject(int type, std::map<std::string, XmlPcodeEmit*>& map);
	uchar read();
	void write(void const* Buf, size_t MaxCharCount);
	uchar readToBurst();
	uchar readToBuffer(std::vector<uchar>& buf);
	std::string readQueryString();
	void generateException();
	void readToResponse();
	void writeString(std::string msg);
	void writeBytes(const uchar out[], int outlen);
	std::vector<uchar> readResponse();
	std::vector<uchar> sendCommand(std::string command);
	std::vector<uchar> sendCommand1ParamTimeout(std::string command, std::string param,
		int timeoutSecs);
	std::vector<uchar> sendCommand2Params(std::string command, std::string param1, std::string param2);
	std::vector<uchar> sendCommand1Param(std::string command, std::string param1);
	std::string convertSourceDoc(Element* el, std::string& displayXml,
		std::string& funcProto, std::string& funcColorProto);
	std::string getOptions(Options opt);
	void adjustUniqueBase(VarnodeTpl* v);
	std::string buildTypeXml(std::vector<TypeInfo>& typeInfo);
	std::string writeFuncProto(FuncProtoInfo func, std::string injectstr, bool bUseInternalList);
	std::string writeFunc(SizedAddrInfo addr, std::string funcname, std::string parentname, FuncProtoInfo func);
protected:
	void setupTranslator(DecompileCallback* cb, std::string sleighfilename);
	XmlPcodeEmit* getPcodeSnippet(std::string parsestring,
		const std::vector<std::pair<std::string, int>>& inputs,
		const std::vector<std::pair<std::string, int>>& outputs);

public:
	enum {
		CALLFIXUP_TYPE = 1,		///< Injection that replaces a CALL
		CALLOTHERFIXUP_TYPE = 2,	///< Injection that replaces a user-defined p-code op, CALLOTHER
		CALLMECHANISM_TYPE = 3,	///< Injection to patch up data-flow around the caller/callee boundary
		EXECUTABLEPCODE_TYPE = 4	///< Injection running as a stand-alone p-code script
	};

	~DecompInterface();
	static int coreTypeLookup(size_t size, std::string metatype);
	static void getLangFiles(std::string processorpath, std::string externaltool,
		std::map<std::string, std::vector<int>> & toolMap, std::vector<LangInfo>& li);
	static void getProtoEvals(std::string cspec, std::vector<std::string>& vec, int* defaultIdx);
	static std::string compilePcodeSnippet(std::string sleighfilename, std::string parsestring,
		const std::vector<std::pair<std::string, int>>& inputs,
		const std::vector<std::pair<std::string, int>>& outputs);

	void setup(DecompileCallback* cb, std::string sleighfilename, std::string pspecfilename,
		std::string cspecfilename, std::vector<CoreType>& coreTypes, Options opt, int timeout, int maxpload);
	//coreTypes IDs will be automatically properly generated upon return
	void setMaxResultSize(int maxResultSizeMBytes);
	void setShowNamespace(bool showNamespace);
	bool setSimplificationStyle(std::string actionstring);
	bool toggleSyntaxTree(bool val);
	bool toggleCCode(bool val);
	bool toggleParamMeasures(bool val);
	bool toggleJumpLoads(bool val);
	bool setOptions(Options opt);

	//only after setup, and if decompilation process running and registered
	std::string doDecompile(DecMode dm, AddrInfo addr, std::string& displayXml,
		std::string& funcProto, std::string& funcColorProto);
	void registerProgram();
	int deregisterProgram();
	int regNameToIndex(std::string regName);
	std::string getRegisterFromIndex(unsigned long long offs, int size);
};