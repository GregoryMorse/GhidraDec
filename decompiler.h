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
			std::string model;
			SymInfo retType;
			std::vector<SymInfo> syminfo;
		};
		RdGlobalInfo* di;
		int regNameToIndexIda(std::string regstr);
		std::string arglocToAddr(argloc_t al, unsigned long long* offset, std::vector<JoinEntryInfo>& joins, bool noResolveReg);
	public:
		DecompInterface* decInt = nullptr;

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
		IdaCallback(RdGlobalInfo* rdgi) : di(rdgi) {}
		virtual ~IdaCallback();
		void executeOnMainThread(std::function<void()> fun);

		void launchDecompiler();
		size_t readDec(void* Buf, size_t MaxCharCount);
		size_t writeDec(void const* Buf, size_t MaxCharCount);
		void terminate();
		void getInits(std::vector<InitStateItem>& inits);
		std::string getPcodeInject(int type, std::string name, std::string base, unsigned long long offset, std::string fixupbase, unsigned long long fixupoffset);
		void getCPoolRef(const std::vector<unsigned long long>& refs, CPoolRecord& rec);
		int getBytes(unsigned char* ptr, int size, std::string base, unsigned long long offset);
		void getMappedSymbol(std::string base, unsigned long long offset, MappedSymbolInfo& msi);
		void getExternInfo(std::string base, unsigned long long offset, std::string& callName, std::string& modName, FuncProtoInfo& func);
		void getMetaType(std::string typeName, std::vector<TypeInfo>& typeChain);
		//colors are Clang-based: "keyword", "comment", "type", "funcname", "var", "const", "param" and "global"
		std::string emit(std::string type, std::string color, std::string str);
		void getComments(std::string base, unsigned long long offset, std::vector<CommentInfo>& comments);
		std::string getSymbol(std::string base, unsigned long long offset);

		void getFuncInfo(std::string base, ea_t offset, func_t* f, std::string & name, FuncProtoInfo & func);
		void getFuncTypeInfo(tinfo_t& ti, bool paramOnly, unsigned long long* extraPop, bool* isNoReturn, bool* dotdotdot, bool* hasThis, std::string& model, SymInfo* retType, std::vector<SymInfo>& syminfo);
		bool getFuncTypeInfoByAddr(ea_t ea, unsigned long long* extraPop, bool* isNoReturn, bool* dotdotdot, bool* hasThis, std::string& model, SymInfo* retType, std::vector<SymInfo>& syminfo);
		bool checkPointer(unsigned long long offset, std::vector<TypeInfo>& typeChain, std::vector<ea_t>& deps);
		void consumeTypeInfo(int idx, std::vector<TypeInfo>& tc, unsigned long long ea, std::vector<ea_t>& deps);
		std::string lookupDataInfo(unsigned long long offset, bool* readonly, bool* volatil, std::vector<TypeInfo>& typeChain);
		void getType(tinfo_t ti, std::vector<TypeInfo>& typeChain, bool bOuterMost = true);

		void analysisDump(std::string& definitions, std::string& idaInfo);
		std::string dumpIdaInfo();
		std::string initForType(const std::vector<TypeInfo>& types/*tinfo_t ti*/, ea_t addr);
		std::string getHeaderDefFromAnalysis(bool allImports);

		void init(std::string sleighfilename, std::string pspec, std::string cspec);
		std::string tryDecomp(DecMode dec, ea_t ea, std::string funcName, std::string& display, std::string& err);
	};

	bool detectProcCompiler(RdGlobalInfo* di, std::string& pspec, std::string& cspec, std::string& sleighfilename);
	std::string ccToStr(cm_t c, int callMethod, bool bInit = false);

	void createRangesFromSelectedFunction(RdGlobalInfo& decompInfo, func_t* fnc);
	void decompileInput(RdGlobalInfo &decompInfo);

} // namespace idaplugin

#endif
