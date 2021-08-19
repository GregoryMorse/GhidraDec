/**
 * @file idaplugin/decompiler.h
 * @brief Module contains classes/methods dealing with program decompilation.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#ifndef IDAPLUGIN_DECOMPILER_H
#define IDAPLUGIN_DECOMPILER_H

#include <string>
#include <functional>
#ifdef __X64__
#include <segregs.hpp>
#else
#include <srarea.hpp>
#endif
#include "defs.h"
#include "sleighinterface.h"

namespace idaplugin {

	class IdaCallback : public DecompileCallback 
	{
		static int idaapi EnumImportNames(ea_t ea, const char* name, uval_t ord, void* param);
		struct ImportInfo
		{
			std::string name;
			uval_t ord;
			int idx;
		};
		struct ImportParam
		{
			int cur;
			std::map<ea_t, ImportInfo>* pImports;
		};
		struct FuncInfo
		{
			std::string name;
			bool bFromParamId;
			FuncProtoInfo fpi;
			FuncProtoInfo oldfpi;
		};
		RdGlobalInfo* di;
		int regNameToIndexIda(std::string regstr);
		std::string arglocToAddr(argloc_t al, unsigned long long* offset, std::vector<SizedAddrInfo>& joins, bool noResolveReg);
	public:
		DecompInterface* decInt = nullptr;
		FILE* sendfp = nullptr;
		FILE* recvfp = nullptr;
		FILE* recfp = nullptr;

		unsigned long long cacheCount = 0;
		std::map<ea_t, std::pair<unsigned long long, std::vector<unsigned short>>> byteCache;

		std::string sleighfilename;
		std::string pspec;
		std::string cspec;
		std::vector<ea_t> allFuncs;
		std::map<ea_t, std::string> allFuncNames;
		std::vector<std::vector<sreg_range_t>> sregRanges;
		std::map<ea_t, bool> exportData;
		std::map<std::string, std::vector<TypeInfo>> typeDatabase;
		std::vector<std::string> modNames;
		std::map<std::string, bool> importNames;
		std::map<ea_t, ImportInfo> imports;
		std::map<std::string, std::vector<ea_t>> typeToAddress;
		std::map<ea_t, bool> usedImports;
		std::map<std::string, bool> usedTypes;
		std::vector<bool> coreTypeUsed;
		std::map<ea_t, std::vector<TypeInfo>> usedData;
		std::map<ea_t, bool> definedFuncs;
		std::map<ea_t, bool> usedFuncs;

		std::map<ea_t, std::vector<ea_t>> dependentData;

		std::map<std::string, bool> finalFuncs;
		std::map<std::string, bool> finalData;
		std::map<std::string, bool> finalTypes;

		std::map<ea_t, FuncInfo> funcProtoInfos;
		std::map<ea_t, std::string> funcProtos;
		std::map<ea_t, std::string> funcColorProtos;
		IdaCallback(RdGlobalInfo* rdgi) : di(rdgi) {}
		virtual ~IdaCallback();
		void executeOnMainThread(std::function<void()> fun);

		void launchDecompiler();
		size_t readDec(void* Buf, size_t MaxCharCount);
		size_t writeDec(void const* Buf, size_t MaxCharCount);
		void protocolRecorder(std::string data, bool bWrite);
		void terminate();
		void getInits(std::vector<InitStateItem>& inits);
		std::string getPcodeInject(int type, std::string name, AddrInfo addr, std::string fixupbase, unsigned long long fixupoffset);
		void getCPoolRef(const std::vector<unsigned long long>& refs, CPoolRecord& rec);
		int getBytes(unsigned char* ptr, int size, AddrInfo addr);
		void getMappedSymbol(AddrInfo addr, MappedSymbolInfo& msi);
		void getExternInfo(AddrInfo addr, std::string& callName, std::string& modName, FuncProtoInfo& func);
		void getMetaType(std::string typeName, std::vector<TypeInfo>& typeChain);
		//colors are Clang-based: "keyword", "comment", "type", "funcname", "var", "const", "param" and "global"
		std::string emit(std::string type, std::string color, std::string str);
		void getComments(AddrInfo addr, std::vector<CommentInfo>& comments);
		std::string getSymbol(AddrInfo addr);
		std::vector<uchar> getStringData(AddrInfo addr);

		void getFuncInfo(AddrInfo addr, func_t* f, std::string & name, FuncProtoInfo & func);
		void getFuncTypeInfo(const tinfo_t& ti, bool paramOnly, FuncProtoInfo& func);
		bool getFuncTypeInfoByAddr(ea_t ea, FuncProtoInfo& func);
		bool checkPointer(unsigned long long offset, std::vector<TypeInfo>& typeChain, std::vector<ea_t>& deps);
		void consumeTypeInfo(int idx, std::vector<TypeInfo>& tc, unsigned long long ea, std::vector<ea_t>& deps);
		std::string lookupDataInfo(unsigned long long offset, bool* readonly, bool* volatil, std::vector<TypeInfo>& typeChain);
		unsigned long long getTypeSize(const tinfo_t & ti);
		void getType(const tinfo_t & ti, std::vector<TypeInfo>& typeChain, bool bOuterMost = true);

		void analysisDump(std::string& definitions, std::string& forDisplay, std::string& idaInfo);
		std::string dumpIdaInfo();
		std::string print_func(ea_t ea, std::string& forDisplay, char NameColor);
		std::string initForType(const tinfo_t& t, ea_t addr, bool bColor);
		std::string initForType(const std::vector<TypeInfo>& types, ea_t addr, bool bColor);
		std::string getHeaderDefFromAnalysis(bool allImports, std::string& forDisplay);

		void init(std::string sleighfilename, std::string pspec, std::string cspec);
		void addrToArgLoc(SizedAddrInfo addr, argloc_t& al);
		void funcInfoToIDA(FuncProtoInfo& paramInfo, tinfo_t & ti);
		void typeInfoToIDA(int idx, std::vector<TypeInfo>& type, tinfo_t& ti);
		Options getOpts();
		void identParams(ea_t ea);
		void updateDatabaseFromParams(std::vector<ea_t> eas);
		std::string tryDecomp(DecMode dec, ea_t ea, std::string funcName, std::string& display, std::string& err,
			std::vector<std::tuple<std::vector<unsigned int>, std::string, unsigned int>>& blockGraph, bool paramOnly = false);
	};

	bool detectProcCompiler(RdGlobalInfo* di, std::string& pspec, std::string& cspec, std::string& sleighfilename);
	std::string ccToStr(cm_t c, int callMethod, bool bInit = false);

	void createRangesFromSelectedFunction(RdGlobalInfo& decompInfo, func_t* fnc);
	void decompileInput(RdGlobalInfo &decompInfo);

} // namespace idaplugin

#endif
