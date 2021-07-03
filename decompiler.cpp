/**
 * @file idaplugin/decompiler.cpp
 * @brief Module contains classes/methods dealing with program decompilation.
 * @copyright (c) 2019 Gregory Morse, licensed under the MIT license
 */

//linux build test - need C++14 however
//AWS: sudo yum -y groupinstall "Development Tools"
//g++ -std=c++1y -m64 -I include -I . -D__IDP__ -D__PLUGIN__ -DNO_OBSOLETE_FUNCS -D__X64__ -D__LINUX__ -fpermissive GhidraDecIface.cpp
//relative registers?
//graph AST view - need to return the full XML info or some custom graph data structure?

#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <memory>

#include "retdec/utils/os.h"
#include "code_viewer.h"
#include "decompiler.h"
#include "plugin_config.h"
#include <fixup.hpp>
#include <entry.hpp>
#include <segment.hpp>
#include "sleighinterface.h"
#if !defined(OS_WINDOWS)
//cpp -dM <<<'' | grep 'FLT_'
#define FLT_DECIMAL_DIG __FLT_DECIMAL_DIG__
#define DBL_DECIMAL_DIG __DBL_DECIMAL_DIG__
#endif

#define min(x, y) ((x <= y) ? (x) : (y))

using namespace retdec;

namespace idaplugin {
	//types depend on types - circular only through structure resolved by forward declarations
	//function declarations depend on types
	//data depends on types and other data or functions due to pointers - circular resolved by extern keyword
	//function definitions depend on types and data - circular resolved by function declaration
	//add core types 

	//problems in following table with calculation of pointer sizes - default/near/far
	;
	/*const size_t sizeTable[] = { //-1 means unknown/not allowed/need to parse for more information
-1,	0,  sizeof(__int8), sizeof(__int16), sizeof(__int32), sizeof(__int64), 16, inf.cc.size_i,
inf.cc.size_b,          sizeof(float),                      ph.max_ptr_size() - ph.segreg_size, -1, -1, -1,            -1, -1,

2,  1,  sizeof(__int8), sizeof(__int16), sizeof(__int32), sizeof(__int64), 16, inf.cc.size_i,
1,                      sizeof(double),                     ph.max_ptr_size() - ph.segreg_size, -1, -1, -1,            -1, -1,

8,  4,  sizeof(__int8), sizeof(__int16), sizeof(__int32), sizeof(__int64), 16, inf.cc.size_i,
inf_is_64bit() ? 8 : 2, inf.cc.size_ldbl,                   ph.max_ptr_size(),                  -1, -1, inf.cc.size_e, -1, -1,

-1, 16, sizeof(char),   -1,              -1,              -1,              -1, ph.segreg_size,
4,                      ph.use_tbyte() ? ph.tbyte_size : 2, -1,                                 -1, -1, -1,            -1, -1 };*/
	template <class T>
	std::string to_string(T t, std::ios_base& (__cdecl* f)(std::ios_base&))
	{
		std::ostringstream oss;
		oss << f << t;
		return oss.str();
	}

	unsigned long long addrToOffset(std::string space, unsigned long long offset)
	{
		if (space == "ram") return offset;
		//else if (space == "stack")
		//else if (space == "register")
		//else if (space == "join")
		else return 0;
	}

	std::string getMetaTypeInfo(const tinfo_t& ti)
	{
		if (ti.is_void()) return "void";
		else if (ti.is_char() || is_type_int(ti.get_realtype()) && ti.is_signed()) return "int";
		else if (is_type_int(ti.get_realtype())) return "uint"; //is_unsigned() || get_sign() == no_sign
		else if (ti.is_floating()) return "float";
		else if (ti.is_bool()) return "bool";
		else if (ti.is_udt()) return "struct"; //ti.is_struct() || ti.is_union()
		else if (ti.is_array()) return "array";
		else if (ti.is_func() || ti.is_funcptr()) return "code";
		else if (ti.is_ptr() /*|| ti.is_array() && ti.get_size() == 0*/) return "ptr";
		else if (ti.is_enum()) return ti.is_signed() ? "int" : "uint"; //"int" if BTE_SDEC?
		else return "unknown";
	}
	bool isX86()
	{
		std::string procName = inf_procname;
		return procName == "8086" ||
			procName == "8086r" ||
			procName == "8086p" ||
			procName == "k62" ||
			procName == "athlon" ||
			procName == "80386p"
			|| procName == "80386r"
			|| procName == "80486p"
			|| procName == "80486r"
			|| procName == "80586p"
			|| procName == "80586r"
			|| procName == "80686p"
			|| procName == "p2"
			|| procName == "p3"
			|| procName == "p4"
			|| procName == "metapc";
	}
	std::string ccToStr(cm_t c, int callMethod, bool bInit)
	{
		//if (!bInit && c == inf_cc_cm) return "default"; //&& ccToStr(inf_cc_cm, is_code_far(inf_cc_cm) ? FTI_FARCALL : FTI_NEARCALL, true) == modelDefault //if this default is not used its meaningless here
		if (!inf_is_32bit() && !inf_is_64bit() && isX86() && ((c & CM_CC_MASK) == CM_CC_STDCALL || (c & CM_CC_MASK) == CM_CC_PASCAL || (c & CM_CC_MASK) == CM_CC_CDECL)) {//16-bit x86 code needs right model
			std::string model;
			if ((c & CM_CC_MASK) == CM_CC_STDCALL || (c & CM_CC_MASK) == CM_CC_PASCAL) model = "__stdcall";
			else model = "__cdecl";
			model += "16"; // && is_code_far(inf_cc_cm)
			model += (callMethod == FTI_FARCALL || callMethod == FTI_DEFCALL && is_code_far(c)) ? "far" : "near"; //|| (c & (CM_MASK | CM_M_MASK)) == (CM_UNKNOWN | CM_M_NN)
			return model;
		}
		//if (!bInit && (c & CM_CC_MASK) == (inf_cc_cm & CM_CC_MASK)) return "default";
		switch (c & CM_CC_MASK) {
		case CM_CC_FASTCALL: return !inf_is_32bit() && !inf_is_64bit() ? "__regcall" : "__fastcall"; //return "__vectorcall"; - extension to fastcall
		case CM_CC_STDCALL: return "__stdcall";
		case CM_CC_VOIDARG: return "__cdecl";
		case CM_CC_CDECL: return "__cdecl";
		case CM_CC_THISCALL: return "__thiscall";
		case CM_CC_PASCAL: return "__stdcall"; // "__pascal";
		case CM_CC_ELLIPSIS: return "__cdecl";
		case CM_CC_SPECIAL: return "__cdecl";
		case CM_CC_SPECIALE: return "__cdecl";
		case CM_CC_SPECIALP: return "__stdcall";
		case CM_CC_UNKNOWN: default: return "unknown";
		}
	}
	bool getFuncByGuess(ea_t ea, tinfo_t& ti)
	{
		if (get_tinfo(&ti, ea) && ti.is_func()) return true;
		tinfo_t tryti;
		if (guess_tinfo(&tryti, ea) != GUESS_FUNC_OK) return false;
		ti = tryti;
		return true;
	}
	IdaCallback::~IdaCallback() {
		if (decInt != nullptr) delete decInt;
		if (sendfp != nullptr) qfclose(sendfp);
		if (recvfp != nullptr) qfclose(recvfp);
		if (recfp != nullptr) qfclose(recfp);
	}
	int IdaCallback::EnumImportNames(ea_t ea, const char* name, uval_t ord, void* param)
	{
		(*((ImportParam*)param)->pImports)[ea] = ImportInfo{ name == nullptr ? "" : name, ord, ((ImportParam*)param)->cur };
		return 1;
	}
	int IdaCallback::regNameToIndexIda(std::string regstr) {
		int res = decInt->regNameToIndex(regstr);
		if (res == -1 && *regstr.begin() == '$') {
			res = decInt->regNameToIndex(regstr.substr(1));
		}
		return res;
	}
	std::string IdaCallback::arglocToAddr(argloc_t al, unsigned long long* offset, std::vector<SizedAddrInfo>& joins, bool noResolveReg) {
		if (al.is_ea()) {
			*offset = al.get_ea();
			return "ram";
		} else if (al.is_stkoff()) {
			*offset = al.stkoff();
			return "stack";
		} else if (al.is_scattered()) {
			scattered_aloc_t scat;
			for (size_t i = scat.size() - 1; i != -1; i--) { //these appear to be high to low, so need to reverse order here
				unsigned long long offs;
				std::vector<SizedAddrInfo> j; //can have a register reg1() as the is_mixed_scattered indicates
				std::string spc = arglocToAddr(scat[i], &offs, j, false); //all "ram" - certainly join or reg2 would not make sense
				joins.push_back(SizedAddrInfo{ {spc, scat[i].off}, scat[i].size });
			}
			return "join";
		} else if (al.is_reg1()) {
			//qstring qs;
			//get_reg_name(&qs, al.reg1(), size);
			//al.regoff() == 0; //how to handle register bit offset?
			bitrange_t bits;
			const char* regnm = get_reg_info(ph.reg_names[al.reg1()], &bits); //use default processor name
			reg_info_t ri;
			parse_reg_name(&ri, regnm == nullptr ? ph.reg_names[al.reg1()] : regnm);
			qstring qs;
			get_reg_name(&qs, al.reg1(), ri.size);
			*offset = noResolveReg ? -1 : regNameToIndexIda(qs.c_str());
			return "register";
		} else if (al.is_reg2()) {
			//qstring qs1, qs2;
			//get_reg_name(&qs1, al.reg1(), size);
			//get_reg_name(&qs2, al.reg2(), size);
			//*offset = regNameToIndexIda(ph.reg_names[al.reg1()]);
			bitrange_t bits1, bits2;
			const char* regnm1 = get_reg_info(ph.reg_names[al.reg1()], &bits1),
				* regnm2 = get_reg_info(ph.reg_names[al.reg2()], &bits2); //use default processor name
			reg_info_t ri1, ri2;
			parse_reg_name(&ri1, regnm1 == nullptr ? ph.reg_names[al.reg1()] : regnm1);
			parse_reg_name(&ri2, regnm2 == nullptr ? ph.reg_names[al.reg2()] : regnm2);
			qstring qs1, qs2;
			get_reg_name(&qs1, al.reg1(), ri1.size);
			get_reg_name(&qs2, al.reg2(), ri2.size);
			//reg2 is the high value, as in dx for pair dx:ax, so it needs to be added to the join space first
			joins.push_back(SizedAddrInfo{ {"register", noResolveReg ? -1 : (unsigned long long)regNameToIndexIda(qs2.c_str())}, (unsigned long long)ri2.size });
			joins.push_back(SizedAddrInfo{ {"register", noResolveReg ? -1 : (unsigned long long)regNameToIndexIda(qs1.c_str())}, (unsigned long long)ri1.size });
			return "join";
		} else if (al.is_rrel()) {
			*offset = al.get_rrel().off; //some type of spacebase
			bitrange_t bits;
			const char* regnm = get_reg_info(ph.reg_names[al.get_rrel().reg], &bits); //use default processor name
			reg_info_t ri;
			parse_reg_name(&ri, regnm == nullptr ? ph.reg_names[al.get_rrel().reg] : regnm);
			qstring qs;
			get_reg_name(&qs, al.get_rrel().reg, ri.size);
			int regidx = regNameToIndexIda(ph.reg_names[al.get_rrel().reg]);			
			return decInt->regToSpacebase(regidx);
		}
		//al.is_custom() || al.is_badloc();
		*offset = 0;
		return "ram";
	}
	void IdaCallback::launchDecompiler()
	{
		if (PRINT_DEBUG) {
			sendfp = qfopen("d:/source/repos/ghidradec/tests/written", "wb");
			recvfp = qfopen("d:/source/repos/ghidradec/tests/received", "wb");
			recfp = qfopen("d:/source/repos/ghidradec/tests/protorec", "wb");
		}
		runCommand(di->decCmd, "",
			&di->decompPid, &di->hDecomp, &di->rdHandle, &di->wrHandle,
			true);
	}
	size_t IdaCallback::readDec(void* Buf, size_t MaxCharCount)
	{		
		size_t val = _read(di->rdHandle, Buf, (unsigned int)MaxCharCount);
		if (recvfp != nullptr) qfwrite(recvfp, Buf, MaxCharCount);
		return val;
		//return qpipe_read(di->rdHandle, Buf, MaxCharCount);
	}
	size_t IdaCallback::writeDec(void const* Buf, size_t MaxCharCount)
	{
		if (sendfp != nullptr) qfwrite(sendfp, Buf, MaxCharCount);
		return _write(di->wrHandle, Buf, (unsigned int)MaxCharCount);
		//return qpipe_write(di->wrHandle, Buf, MaxCharCount);
	}
	void IdaCallback::protocolRecorder(std::string data, bool bWrite)
	{
		if (recfp != nullptr) {
			std::string s = ((bWrite ? "Sent: " : "Received: ") + data + "\n");
			qfwrite(recfp, s.c_str(), s.size());
		}
	}
	void IdaCallback::terminate()
	{
		if (sendfp != nullptr) { qfclose(sendfp); sendfp = nullptr; }
		if (recvfp != nullptr) { qfclose(recvfp); recvfp = nullptr; }
		if (recfp != nullptr) { qfclose(recfp); recfp = nullptr; }
		stopDecompilation(di, false, false, false);
	}
	void IdaCallback::getInits(std::vector<InitStateItem>& inits)
	{
		//inf_start_ea == ea; inf.start_ss; inf.start_sp;
		if (ph.has_segregs()) { //initialize segment registers for tracking - vital for x86-16
			//code segment register although a pointless mapping helps track things for Ghidra
			unsigned long long regtrans = (unsigned long long)regNameToIndexIda(ph.reg_names[ph.reg_code_sreg]);
			for (int j = 0; j < get_segm_qty(); j++) {
				segment_t* s = getnseg(j);
				if (s != nullptr) {
					qstring qs;
					if (get_segm_class(&qs, s) != -1 && qs == "CODE") {
						inits.push_back(InitStateItem{ {"ram", s->start_ea}, {"ram", s->end_ea}, {{"register", regtrans}, (unsigned long)ph.segreg_size}, (unsigned long long)s->sel });
					}
				}
			}
			for (int i = 0; i <= ph.reg_last_sreg - ph.reg_first_sreg; i++) {
				regtrans = (unsigned long long)regNameToIndexIda(ph.reg_names[i + ph.reg_first_sreg]);
				if (regtrans == -1) continue;
				//this is a much more powerful analysis by the processor engine than the default segregs which now are not used
				for (size_t k = 0; k < sregRanges[i].size(); k++) {
					if (sregRanges[i][k].val == -1) continue;
					//if (out.val == ((1 << (ph.segreg_size * 8)) - 1)) continue;
					inits.push_back(InitStateItem{ {"ram", sregRanges[i][k].start_ea}, {"ram", sregRanges[i][k].end_ea}, {{"register", regtrans}, (unsigned long)ph.segreg_size}, (unsigned long long)sregRanges[i][k].val });
				}
				//if (s->defsr[i] != -1) {
					//inits.push_back(InitStateItem{ "ram", s->start_ea, "ram", s->end_ea, "register", (unsigned long long)regNameToIndexIda(ph.reg_names[i + ph.reg_first_sreg]), (unsigned long)ph.segreg_size, (unsigned long long)s->defsr[i] });
				//}
			}
		}
	}
	std::string IdaCallback::getPcodeInject(int type, std::string name, AddrInfo addr, std::string fixupbase, unsigned long long fixupoffset)
	{
		if (type == DecompInterface::CALLMECHANISM_TYPE) {
			if ((name == "__stdcall16far@@inject_uponreturn" || name == "__cdecl16far@@inject_uponreturn") && addr.space == "ram") {
				sel_t s;
				executeOnMainThread([&s, addr]() { s = getseg((ea_t)addr.offset)->sel; });
				return "CS = " + std::to_string(s) + ";";
			} else return "";
		} else return "";
	}
	void IdaCallback::getCPoolRef(const std::vector<unsigned long long>& refs, CPoolRecord& rec)
	{
	}
#define CACHEPAGESIZE 4096ull
#define CACHELIMIT 128
#define CACHECLEANUPAFTER 256
	int IdaCallback::getBytes(unsigned char* ptr, int size, AddrInfo addr)
	{
		int i = 0;
		if (addr.space == "ram") {
			if (byteCache.size() > CACHECLEANUPAFTER) {
				for (std::map<ea_t, std::pair<unsigned long long, std::vector<unsigned short>>>::iterator it = byteCache.begin(); it != byteCache.end();) {
					if (it->second.first < cacheCount - CACHELIMIT) it = byteCache.erase(it);
					else it++;
				}
			}
			//paging strategy for easy lookup - just like the processor utilizes
			bool bFetch = false;
			unsigned long long page = addr.offset & (~(CACHEPAGESIZE - 1)),
				pgoffs = addr.offset & (CACHEPAGESIZE - 1);
			do {
				if (byteCache.find((ea_t)page) == byteCache.end()) {
					bFetch = true;
					break;
				}
				page += CACHEPAGESIZE;
			} while (page < addr.offset + size);
			if (bFetch) executeOnMainThread([this, size, addr]() {
					unsigned long long page = addr.offset & (~(CACHEPAGESIZE - 1));
					do {
						for (int i = 0; i < CACHEPAGESIZE; i++) { //i = offset & (CACHEPAGESIZE - 1);
							byteCache[(ea_t)page].second.push_back(is_loaded((ea_t)page + i) ? get_byte((ea_t)page + i) : 256);
						}
						page += CACHEPAGESIZE;
					} while (page < addr.offset + size);
				});
			page = addr.offset & (~(CACHEPAGESIZE - 1));
			pgoffs = addr.offset & (CACHEPAGESIZE - 1);
			byteCache[(ea_t)page].first = cacheCount++;
			for (i = 0; i < size; i++) {
				if (pgoffs == CACHEPAGESIZE) {
					page += CACHEPAGESIZE;
					pgoffs = 0;
					byteCache[(ea_t)page].first = cacheCount++;
				}
				if ((byteCache[(ea_t)page].second[(size_t)pgoffs] & 256) == 0) ptr[i] = (unsigned char)byteCache[(ea_t)page].second[(size_t)pgoffs];
				else break; //could do something like 0x90 for NOP on x86 - but could be data request
				pgoffs++;
			}
		}
		if (i != size) memset(ptr + i, 0, size - i);
		//msg("%0X%0X%0X%0X %0X%0X%0X%0X\n", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7]);
		return i;
	}
	std::string getSymbolName(unsigned long long offset)
	{
		std::string str = get_name((ea_t)offset).c_str();
		if (str.size() == 0) {
			str = to_string(offset, std::hex);
			str = ("ram0x" + std::string((inf_is_64bit() ? 16 : 8) - str.size(), '0') + str);
		}
		return str;
	}
	void getMemoryInfo(unsigned long long offset, bool* readonly, bool* volatil)
	{
		*volatil = !is_mapped((ea_t)offset);
		segment_t* s = getseg((ea_t)offset);
		//(s->type & SEG_DATA) != 0;
		*readonly = s != nullptr && (s->perm & SEGPERM_WRITE) == 0 && (s->perm & SEGPERM_MAXVAL) != 0;
		xrefblk_t xr;
		if (xr.first_to((ea_t)offset, XREF_ALL)) {
			do {
				if ((xr.type & XREF_MASK) == dr_W) {
					*readonly = false;
				}
			} while (xr.next_to());
		}
	}
	void IdaCallback::getFuncInfo(AddrInfo addr, func_t* f, std::string& name, FuncProtoInfo& func)
	{
		//get_ea_name(&qs, offset, GN_VISIBLE | GN_DEMANGLED | GN_SHORT);
		if (imports.find((ea_t)addr.offset) != imports.end() && imports[(ea_t)addr.offset].name != "") {
			name = demangle_name(imports[(ea_t)addr.offset].name.c_str(), MNG_SHORT_FORM).c_str();
			if (name.size() == 0) name = imports[(ea_t)addr.offset].name;
		}
		if (name.size() == 0) name = get_short_name((ea_t)addr.offset, GN_STRICT).c_str();
		if (name.size() != 0) name = name.substr(0, name.find("(", 0));
		else name = getSymbolName(addr.offset);
		func.isInline = false;
		bool bFuncTinfo = getFuncTypeInfoByAddr(f != nullptr ? f->start_ea : (ea_t)addr.offset, func);
		func.isNoReturn = (f != nullptr) ? !f->does_return() : (bFuncTinfo ? func.isNoReturn : false);// f->flags& FUNC_NORET ? true : false;
		//(f->flags & FUNC_USERFAR) != 0
		//(f->flags & FUNC_LIB) != 0
		//(f->flags & FUNC_STATICDEF) != 0
		size_t curArgs = func.syminfo.size();
		if (f != nullptr) {
			if (func.model == "unknown") {
				if (f->argsize == 0) { //|| !f->does_return()
					func.model = ccToStr((f->flags & FUNC_PURGED_OK) != 0 ? CM_CC_CDECL : CM_CC_UNKNOWN, f->is_far() || (f->flags & FUNC_USERFAR) != 0 ? FTI_FARCALL : FTI_NEARCALL, false);
				} else { //CM_CC_UNKNOWN is likely better...
					func.model = ccToStr(CM_CC_STDCALL, f->is_far() || (f->flags & FUNC_USERFAR) != 0 ? FTI_FARCALL : FTI_NEARCALL, false);
				}
			}
			if (func.extraPop == -1 && (f->flags & FUNC_PURGED_OK) != 0) func.extraPop = (unsigned long long)f->argsize; //type unknown but have frame so now can calculate
			if (f->regargs == nullptr) read_regargs(f); //populates regargs, similar to how get_spd or the like with f specified populate stkpts
			//callregs_t cr;
			//cr.init_regs(inf_cc_cm);
			for (int i = 0; i < f->regargqty; i++) { //name can be a nullptr!
				std::string nm = f->regargs[i].name == nullptr ? "" : f->regargs[i].name;
				unsigned long long offset;
				std::vector<TypeInfo> typeChain;
				unsigned long long size;
				//func_type_data_t ftd;
				//if (ti.is_func() && ti.get_func_details(&ftd)) {
				//callregs_t cr;
				//ph.get_cc_regs(&cr, get_cc(ftd.get_cc()));
				if (f->regargs[i].type != nullptr) {
					tinfo_t ti; //The type information is internally kept as an array of bytes terminated by 0.
					qtype typeForDeser;
					typeForDeser.append(f->regargs[i].type);
					ti.deserialize(get_idati(), &typeForDeser); //will free buffer passed!
					getType(ti, typeChain, false);
					size = ti.get_size();
					qstring qs;
					if (get_reg_name(&qs, f->regargs[i].reg, (size_t)size) == -1) offset = -1;  //for example 1 byte of DS register will fail
					else offset = regNameToIndexIda(qs.c_str());
					if (offset == -1) {
						bitrange_t bits;
						const char* regnm = nullptr;
						regnm = get_reg_info(ph.reg_names[f->regargs[i].reg], &bits); //use default processor name
						offset = regNameToIndexIda(regnm == nullptr ? ph.reg_names[f->regargs[i].reg] : regnm);
					}
				} else {
					bitrange_t bits;
					const char* regnm = nullptr;
					regnm = get_reg_info(ph.reg_names[f->regargs[i].reg], &bits); //use default processor name
					reg_info_t ri;
					parse_reg_name(&ri, regnm == nullptr ? ph.reg_names[f->regargs[i].reg] : regnm);
					size = ri.size;
					qstring qs;
					get_reg_name(&qs, f->regargs[i].reg, ri.size);
					offset = regNameToIndexIda(qs.c_str());
				}
				if (offset == -1) {
					offset = regNameToIndexIda(ph.reg_names[f->regargs[i].reg]); //last try if nothing else works
					if (offset == -1) continue;
				}
				std::vector<SymInfo>::iterator it = nm.empty() ? func.syminfo.end() : std::find_if(func.syminfo.begin(), func.syminfo.end(), [&nm](SymInfo it) { return it.pi.name == nm; });
				if (it == func.syminfo.end()) {
					it = std::find_if(func.syminfo.begin(), func.syminfo.end(), [offset](SymInfo it) { return it.pi.name == "" && it.addr.addr.space == "register" && it.addr.addr.offset == offset; });
				}
				if (it != func.syminfo.end()) {
					if (it->pi.name == "") it->pi.name = nm;
					it->addr.addr.space = "register";
					it->addr.addr.offset = offset;
					continue;
				}
				if (f->regargs[i].type == nullptr) {
					std::string metaType = "unknown";
					int typ = DecompInterface::coreTypeLookup((int)size, metaType);
					if (typ == -1 && size != 1) {
						typeChain.push_back(TypeInfo{ "", (unsigned long long)size, "array", false, false, false, (unsigned long long)size });
						typeChain.push_back(TypeInfo{ "undefined", 1, metaType });
					} else {
						typeChain.push_back(TypeInfo{ typ == -1 ? "undefined" : defaultCoreTypes[typ].name, (unsigned long long)size, metaType });
					}
					coreTypeUsed[typ == -1 ? 1 : typ] = true;
				}
				func.syminfo.push_back(SymInfo{ { nm, typeChain }, {{"register", offset}, size}, (int)curArgs++ });
			}
		}
		if (f != nullptr && f->frame != BADNODE) { //f->analyzed_sp()
			struc_t* frame = get_frame(f);
			func.extraPop += get_frame_retsize(f);
			ea_t firstarg = frame_off_args(f);
			//generalized for any arguments not found in the frame
			for (size_t i = 0; i < func.syminfo.size(); i++) {
				if (func.syminfo[i].addr.addr.space == "stack") {
					func.syminfo[i].addr.addr.offset += firstarg - frame_off_retaddr(f);
				}
			}
			for (uint32 i = 0; i < frame->memqty; i++) {
				//if (frame->members[i].get_soff() >= f->frsize && bFuncTinfo) break; //args already read for type info but offsets not adjusted
				//" r" for return address and " s" for saved registers is a convention that seems to be used in IDA
				if (!(is_funcarg_off(f, frame->members[i].get_soff()) || lvar_off(f, frame->members[i].get_soff()))) continue;
				std::string nm = get_member_name(frame->members[i].id).c_str();
				if (frame_off_retaddr(f) == frame->members[i].get_soff()) continue;
				/*if (nm == " r") {
					retSize = get_member_size(&frame->members[i]);
					continue;
				}*/
				if (frame_off_savregs(f) == frame->members[i].get_soff()) continue;
				//if (nm == " s") continue;
				std::vector<SymInfo>::iterator it = nm.empty() ? func.syminfo.end() : std::find_if(func.syminfo.begin(), func.syminfo.end(), [&nm](SymInfo it) { return it.pi.name == nm; });
				unsigned long long offset = frame->members[i].soff - frame_off_retaddr(f);// - firstarg;
				if (it == func.syminfo.end()) {
					//if no name, offset is from 0, so use arg offset
					//msg("Offset: %llX\n", offset);
					it = std::find_if(func.syminfo.begin(), func.syminfo.end(), [offset](SymInfo it) { return it.pi.name == "" && it.addr.addr.space == "stack" && it.addr.addr.offset == offset; });
				}
				//unsigned long long reloffs = frame->members[i].get_soff() - frame_off_retaddr(f); // - f->frsize - f->frregs; //frame seems to always be relative to return address
				if (it != func.syminfo.end()) {
					if (it->pi.name == "") it->pi.name = nm;
					it->addr.addr.space = "stack";
					it->addr.addr.offset = offset;
					continue;
				}
				//if (frame->members[i].get_soff() >= f->frsize && frame->members[i].get_soff() < f->frsize + 1 + f->frregs) continue; //return address size is computed how - get_member_size() for " r"?
				tinfo_t ti;
				std::vector<TypeInfo> typeChain;
				if (frame->members[i].has_ti() && get_tinfo(&ti, frame->members[i].id)) {
					//int typ = coreTypeLookup((int)ti.get_size(), getMetaTypeInfo(ti));
					//msg("has struct member type info");
					getType(ti, typeChain, false);
				} else {
					std::string metaType = "unknown";
					int typ = DecompInterface::coreTypeLookup((int)get_member_size(&frame->members[i]), metaType);
					if (typ == -1 && get_member_size(&frame->members[i]) != 1) {
						typeChain.push_back(TypeInfo{ "", get_member_size(&frame->members[i]), "array", false, false, false, get_member_size(&frame->members[i]) });
						typeChain.push_back(TypeInfo{ "undefined", 1, metaType });
					} else {
						typeChain.push_back(TypeInfo{ typ == -1 ? "undefined" : defaultCoreTypes[typ].name,  get_member_size(&frame->members[i]), metaType });
					}
					coreTypeUsed[typ == -1 ? 1 : typ] = true;
				}
				func.syminfo.push_back(SymInfo{ {nm, typeChain}, {{"stack", frame->members[i].get_soff() - frame_off_retaddr(f)}, get_member_size(&frame->members[i])},
					is_funcarg_off(f, frame->members[i].get_soff()) ? (int)curArgs++ : -1 }); //frame->members[i].get_soff() >= firstarg or f->frsize - f->frregs
			}
		} else {
			unsigned long long retSize = 0;
			if (f != nullptr && (f->is_far() || (f->flags & FUNC_USERFAR) != 0)) retSize += ph.segreg_size;
			else if (f == nullptr && bFuncTinfo) {
				tinfo_t ti;
				func_type_data_t ftd;
				getFuncByGuess((ea_t)addr.offset, ti);
				ti.get_func_details(&ftd);
				if (ftd.get_call_method() == FTI_FARCALL || ftd.get_call_method() == FTI_DEFCALL && is_code_far(inf_cc_cm)) retSize += ph.segreg_size;
			}
			//bitrange_t bits;
			//const char* regnm = get_reg_info(ash.a_curip, &bits); //use default processor name
			//reg_info_t ri;
			//parse_reg_name(&ri, regnm == nullptr ? ash.a_curip : regnm);
			//retSize += ri.size;
			retSize += f == nullptr ? inf_is_64bit() ? 8 : (inf_is_32bit() ? 4 : 2) : get_func_bytes(f); //per documentation of get_func_bitness
			//type arguments need stack adjustment but there is no frame?
			for (size_t i = 0; i < func.syminfo.size(); i++) {
				if (func.syminfo[i].addr.addr.space == "stack") {
					func.syminfo[i].addr.addr.offset += retSize;
				}
			}
			//only need to figure out return address size and adjust extraPop which depending on type info or not may affect the model
			//it would be really nice to have context for which cref was used!
			if (func.extraPop == -1) {
				std::map<sval_t, int> histogram;
				for (ea_t ea = get_first_cref_to((ea_t)addr.offset); ea != BADADDR;) {
					//if points is nullptr, IDA wont retrieve and fill it unless get_spd or the like is called with non-null pointer func_t first
					func_t* ft = get_func(ea); //IDA will not retrieve spd information except for function head as per decompiling ida64.dll where after it uses ea2node, netnode_qgetblob and then unpack_dq to retrieve it
					//get_spd(f, f->start_ea); //passing null unlike API documentation will not yield useful results if not already populated
					/*if (f != nullptr && f->points != nullptr) {
						for (int i = 0; i < f->pntqty; i++) {
							if (f->points[i].ea == get_item_end(ea)) {
								sval_t s = f->points[i].spd - (i == 0 ? 0 : f->points[i - 1].spd);
							}
						}
					}*/
					histogram[get_spd(ft, get_item_end(ea)) - get_spd(ft, ea)]++;
					ea = get_next_cref_to((ea_t)addr.offset, ea);
				}
				std::map<sval_t, int>::iterator max =
					std::max_element(histogram.begin(), histogram.end(), [](std::pair<sval_t, int> p1, std::pair<sval_t, int> p2) { return p1.second < p2.second; });
				if (max != histogram.end()) func.extraPop = max->first;
			}
			func.extraPop = func.extraPop == -1 ? retSize : func.extraPop + retSize;
			/*for (ea_t ea = get_first_cref_to(f->start_ea); ea != BADADDR; ea = get_next_cref_to(f->start_ea, ea)) {
				func_t* fp = get_func(ea);
				if (fp != nullptr) {
					get_spd(fp, fp->start_ea);
					if (definedFuncs.find(fp->start_ea) == definedFuncs.end()) continue;
					if (fp->points != nullptr) {
						for (int i = 0; i < fp->pntqty; i++) { //can build a histogram and take the winner
							if (fp->points[i].ea == get_item_end(ea)) {
								//*extraPop = fp->points[i].spd - (i == 0 ? 0 : fp->points[i - 1].spd);
								//if ((*extraPop & (1ull << 63)) != 0)* extraPop = 2; //make it cdecl
							}
						}
					}
				}
			}*/
			//if (bFuncTinfo) {//adjust arguments
			//} else {
			//}
			//return address size based on model
			//x86-64 bit far/interupt calling aligns everything like flags, error codes, stack pointer and stack register to 64 bits each
			//if (ftd.get_call_method() == FTI_FARCALL || ftd.get_call_method() == FTI_DEFCALL && is_code_far(inf_cc_cm)) {
			//if (f->is_far() || (f->flags & FUNC_USERFAR) != 0 || imports.find(offset) != imports.end()) {
				//ph.max_ptr_size(); - for some reason is 48 bits on 64-bit systems?
				//ph.get_stkarg_offset();
				//ph.segreg_size
				//syminfo[i].offset += ph.segreg_size + (inf_is_64bit() ? 8 : inf_is_32bit() == 1 ? 4 : 2);
				//} else if (ftd.get_call_method() == FTI_NEARCALL || ftd.get_call_method() == FTI_DEFCALL && !is_code_far(inf_cc_cm)) {
			//} else {
				//syminfo[i].offset += (inf_is_64bit() ? 8 : inf_is_32bit() == 1 ? 4 : 2);
				//} else if (ftd.get_call_method() == FTI_INTCALL) {
				//syminfo[i].offset += ph.segreg_size + (inf_is_64bit() ? 8 : inf_is_32bit() == 1 ? 4 : 2); //far pointer to old stack, e/rflags, error code
			//}
		}
		if (f != nullptr) {
			/*for (int i = 0; i < ph.regs_num; i++) {
				bitrange_t bits;
				const char* regnm = nullptr;
				regvar_t* rv = find_regvar(f, BADADDR, ph.reg_names[i]); //all general registers
				regnm = get_reg_info(ph.reg_names[i], &bits); //use default processor name
				if (regnm != nullptr && strcmp(regnm, ph.reg_names[i]) != 0) rv = find_regvar(f, BADADDR, regnm); //all general registers
				if (rv != nullptr) {}
			}*/
			if (f->regvars == nullptr) find_regvar(f, f->start_ea, nullptr); //convience function to always load regvars since canon can be null and the load already occurred from the database
			if (f->regvarqty != -1 && f->regvars != nullptr) {
				for (int i = 0; i < f->regvarqty; i++) {
					//f->regvars[i].cmt; //can be nullptr
					reg_info_t ri;
					parse_reg_name(&ri, f->regvars[i].canon); //does it need to be translated to default via get_reg_info?
					std::vector<TypeInfo> typeChain;
					std::string metaType = "unknown";
					int typ = DecompInterface::coreTypeLookup((int)ri.size, metaType);
					if (typ == -1 && ri.size != 1) {
						typeChain.push_back(TypeInfo{ "", (unsigned long long)ri.size, "array", false, false, false, (unsigned long long)ri.size });
						typeChain.push_back(TypeInfo{ "undefined", 1, metaType });
					} else {
						typeChain.push_back(TypeInfo{ typ == -1 ? "undefined" : defaultCoreTypes[typ].name,  (unsigned long long)ri.size, metaType });
					}
					coreTypeUsed[typ == -1 ? 1 : typ] = true; //name base is p for pointer, a for array, other wise first char of type name
					func.syminfo.push_back(SymInfo{ {f->regvars[i].user == nullptr || f->regvars[i].user[0] == 0 ? std::string(1, typ == -1 ? 'u' : defaultCoreTypes[typ].name[0]) + "Var_" + std::string(f->regvars[i].canon) : f->regvars[i].user,typeChain },
						{{"register", (unsigned long long)regNameToIndexIda(f->regvars[i].canon)}, (unsigned long long)ri.size}, -1, RangeInfo{ addr.space, f->regvars[i].start_ea, f->regvars[i].end_ea } });
				}
			}
			/*syminfo.erase(std::remove_if(syminfo.begin(), syminfo.end(), [](SymInfo it) { return it.name == ""; }), syminfo.end()); //or should give names - but often not used or necessary - dont mark in argument category at least
			//need to remove argument indexes however if deleting arguments
			curArgs = 0;
			for (int i = 0; i < syminfo.size(); i++) {
				if (syminfo[i].argIndex == -1) continue;
				if (syminfo[i].argIndex == curArgs) curArgs++;
				else syminfo[i].argIndex = curArgs++;
			}*/
		}
	}
	void IdaCallback::getMappedSymbol(AddrInfo addr, MappedSymbolInfo& msi)
	{
		executeOnMainThread([this, &msi, addr]() {
			msi.kind = KIND_HOLE;
			if (addr.space == "register") {} else if (addr.space == "ram") {
				if (imports.find((ea_t)addr.offset) != imports.end()) {
					usedImports[(ea_t)addr.offset] = true;
					//Ghidra currently has 2 bugs one with restoring CS register if not simulating same segment fixups and the other calling back for external ref function info
						//16-bit apps have functions defined for imports also...
					if (isX86() && !inf_is_32bit() && !inf_is_64bit() && get_func((ea_t)addr.offset) != nullptr) msi.kind = KIND_FUNCTION;
					else
						msi.kind = KIND_EXTERNALREFERENCE;
				} else {
					if (get_func((ea_t)addr.offset) != nullptr) {
						msi.kind = KIND_FUNCTION;
					} else {
						flags_t fl = get_flags((ea_t)addr.offset);
						//if (is_func(fl)) msi.kind = KIND_FUNCTION; //function start only
						//if (exists_fixup(offset)) msi.kind = KIND_EXTERNALREFERENCE; //fixups is not really a determining criterion in exported functions
						if (is_data(fl) || is_unknown(fl) && is_mapped((ea_t)addr.offset)) msi.kind = KIND_DATA;
						else if (has_name(fl) || has_dummy_name(fl)) msi.kind = KIND_LABEL; //dummy name/label for code only is_code(fl)...
						//if (is_unknown(fl) && !is_mapped(offset))
					}
				}
			} else if (addr.space == "stack") {}
			if (msi.kind == KIND_HOLE) {
				//flags_t f = get_flags(offset);
				//no information about a segment should still assume writable
				//volatile for unmapped memory should be based on the life of the system
				//volatile for mapped memory should be based on the life of the program
				//volatile for stack and registers should be based on the life of a function
				//const and unique would not be volatile ever, though a join space could be comprised of members who are volatile
				if (addr.space == "ram") {
					//prev_addr(offset); prev_head(offset, inf_min_ea); prev_that(offset, inf_max_ea, [](flags_t f, void* ud) { return true; });
					//next_addr(offset); next_head(offset, inf_max_ea); next_that(offset, inf_max_ea, [](flags_t f, void* ud) { return true; });
					getMemoryInfo(addr.offset, &msi.readonly, &msi.volatil);
				} else {//if (base == "register" || base == "stack") {
					msi.readonly = false;
					msi.volatil = addr.space != "register"; //far more conservative to assume not volatile until figure out how to determine this - perhaps pspec must explicitly define
				}
			} else if (msi.kind == KIND_LABEL) {
				if (addr.space == "ram") {
					getMemoryInfo(addr.offset, &msi.readonly, &msi.volatil);
					msi.name = getSymbolName(addr.offset);
				} else {
					msi.readonly = false;
					msi.volatil = addr.space != "register";
				}
			} else if (msi.kind == KIND_DATA) {
				if (addr.space == "ram") {
					msi.name = lookupDataInfo(addr.offset, &msi.readonly, &msi.volatil, msi.typeChain);
				} else {
					msi.readonly = false;
					msi.volatil = addr.space != "register";
				}
			} else if (msi.kind == KIND_EXTERNALREFERENCE) {
				if (addr.space == "ram") {
					if (imports.find((ea_t)addr.offset) != imports.end() && imports[(ea_t)addr.offset].name != "") {
						msi.name = demangle_name(imports[(ea_t)addr.offset].name.c_str(), MNG_SHORT_FORM).c_str();
						if (msi.name.size() == 0) msi.name = imports[(ea_t)addr.offset].name;
					}
					if (msi.name.size() == 0) msi.name = get_short_name((ea_t)addr.offset, GN_STRICT).c_str();
					if (msi.name.size() != 0) msi.name = msi.name.substr(0, msi.name.find("(", 0));
					else msi.name = getSymbolName(addr.offset);
				}
				/*fixup_data_t fd;
				get_fixup(&fd, offset);
				qstring buf;
				get_fixup_desc(&buf, offset, fd);
				buf.c_str();*/
			} else if (msi.kind == KIND_FUNCTION) {
				//get_func_name(&name, f->start_ea);
				func_t* f = get_func((ea_t)addr.offset);
				msi.entryPoint = (f != nullptr) ? f->start_ea : addr.offset;
				if (msi.entryPoint != addr.offset) {
					if (f != nullptr) {
						rangeset_t rangeset;
						if (get_func_ranges(&rangeset, f) != BADADDR) {
							for (rangeset_t::iterator it = rangeset.begin(); it != rangeset.end(); it++) {
								msi.ranges.push_back(RangeInfo{ addr.space, it->start_ea, it->end_ea });
							}
						}
					}
				} else {
					if (f != nullptr) {
						if (addr.offset == f->start_ea && definedFuncs.find((ea_t)addr.offset) == definedFuncs.end() && imports.find((ea_t)addr.offset) == imports.end()) usedFuncs[(ea_t)addr.offset] = true;
						//find_func_bounds(f, FIND_FUNC_NORMAL); //size minimally 1 or maximally must be contiguous block from entry point
						//but func_t already contiguous and the tails account for the rest so end_ea and size are a valid way dont need to find this or first from func ranges should do
						//*size = f->size();
						rangeset_t rangeset;
						msi.size = get_func_ranges(&rangeset, f) == BADADDR ? f->size() : rangeset.begin()->size();
					} else msi.size = 1; //Ghidra uses 1 always despite its commentary
					if (funcProtoInfos.find((ea_t)msi.entryPoint) != funcProtoInfos.end()) {
						msi.name = funcProtoInfos[(ea_t)msi.entryPoint].name;
						msi.func = funcProtoInfos[(ea_t)msi.entryPoint].fpi;
					} else {
						getFuncInfo(addr, f, msi.name, msi.func);
						funcProtoInfos[(ea_t)msi.entryPoint] = FuncInfo{ msi.name.c_str(), false, msi.func };
					}
				}
			}
		});
	}
	void IdaCallback::getFuncTypeInfo(const tinfo_t & ti, bool paramOnly, FuncProtoInfo& func)
	{
		asize_t retSize = 0;
		func_type_data_t ftd;
		if (ti.is_func() && ti.get_func_details(&ftd)) { //GTD_NO_ARGLOCS
			//(ftd.flags & FTI_SPOILED) != 0
			for (size_t i = 0; i < ftd.spoiled.size(); i++) {
				qstring qs;
				get_reg_name(&qs, ftd.spoiled[i].reg, ftd.spoiled[i].size);
				unsigned long long offset = regNameToIndexIda(qs.empty() ? ph.reg_names[ftd.spoiled[i].reg] : qs.c_str());
				func.killedByCall.push_back(SizedAddrInfo{ {"register", offset}, (unsigned long long)ftd.spoiled[i].size });
			}
			func.dotdotdot = ftd.is_vararg_cc();
			func.hasThis = (ftd.get_cc() & CM_CC_MASK) == CM_CC_THISCALL;
			func.isNoReturn = (ftd.flags & FTI_NORET) != 0;
			func.customStorage = is_user_cc(ftd.get_cc());
			//switch (get_cc(inf_cc_cm)) {
			//switch (get_cc(guess_func_cc(fd, ti.calc_purged_bytes(), CC_CDECL_OK | CC_ALLOW_ARGPERM | CC_ALLOW_REGHOLES | CC_HAS_ELLIPSIS))) {
			func.model = ccToStr(ftd.cc, ftd.get_call_method());
			for (size_t i = 0; i < ftd.size(); i++) {
				unsigned long long offs;
				//int typ = coreTypeLookup((int)ftd[i].type.get_size(), getMetaTypeInfo(ftd[i].type));
				std::vector<SizedAddrInfo> joins;
				std::string space = arglocToAddr(ftd[i].argloc, &offs, joins, paramOnly);
				//argloc offset is relative to the frame and not properly adjusted for the stack
				//need to at least adjust for return address size or problems will occur in 16-bit exports!
				//if (qs.size() == 0) ftd[i].type.print(&qs, NULL, PRTYPE_1LINE);
				//core types must be resolved here since the printing will not be queryable later such as for multi token values like "unsigned int"
				//need to include a fixed core type typedef - it could be reduced only by scanning the file decompiled output
				std::vector<TypeInfo> typeChain;
				getType(ftd[i].type, typeChain, false);
				qstring qs;
				if (ftd[i].name.size() == 0 && space == "register") get_reg_name(&qs, ftd[i].argloc.reg1(), ftd[i].type.get_size());
				func.syminfo.push_back(SymInfo{ {ftd[i].name.size() == 0 && space == "register" ? "in_" + std::string(qs.c_str()) : ftd[i].name.c_str(), typeChain },
					{{space, offs, joins}, ftd[i].type.get_size()}, (int)i, {} });
			}
			func.retType.addr.addr.space = arglocToAddr(ftd.retloc, &func.retType.addr.addr.offset, func.retType.addr.addr.joins, paramOnly);
			func.retType.addr.size = ftd.rettype.get_size();
			if (func.retType.addr.size == BADSIZE) func.retType.addr.size = 0; //0 is void
			//qstring qs;
			//ftd.rettype.get_type_name(&qs);
			getType(ftd.rettype, func.retType.pi.ti, false);
			func.extraPop = ti.calc_purged_bytes();
			//ti.is_vararg_cc();
			//local variables still need to be processed
		} else {
			func.extraPop = -1; //need size of return address which is also popped
			argloc_t al;
			const tinfo_t t(inf_is_64bit() ? BT_INT64 : (inf_is_32bit() ? BT_INT32 : BT_INT16)); //or use BT_INT8 - 1 byte like Ghidra?
#ifdef __X64__
			if (ph.calc_retloc(&al, t, inf_cc_cm) == 1)
#else
			if (ph.notify(ph.calc_retloc3, &t, inf_cc_cm, &al) == 2)
#endif
				func.retType.addr.addr.space = arglocToAddr(al, &func.retType.addr.addr.offset, func.retType.addr.addr.joins, false);
			else {
				func.retType.addr.addr.space = "register"; // "ram";
				qstring qs;
				get_reg_name(&qs, 0, t.get_size());
				func.retType.addr.addr.offset = regNameToIndexIda(qs.empty() ? ph.reg_names[0] : qs.c_str());
				func.retType.addr.size = t.get_size();
			}
			std::string metaType = "unknown"; //still unknown really
			int typ = DecompInterface::coreTypeLookup((size_t)func.retType.addr.size, metaType);
			func.retType.pi.ti.push_back(TypeInfo{ typ == -1 ? "undefined" : defaultCoreTypes[typ].name.c_str(), func.retType.addr.size, metaType });
			coreTypeUsed[typ == -1 ? 1 : typ] = true;
			func.model = "unknown";
		}
	}
	bool IdaCallback::getFuncTypeInfoByAddr(ea_t ea, FuncProtoInfo& func)
	{
		tinfo_t ti;
		bool bFuncTinfo = getFuncByGuess(ea, ti);
		getFuncTypeInfo(ti, false, func);
		return bFuncTinfo;
	}
	bool IdaCallback::checkPointer(unsigned long long offset, std::vector<TypeInfo>& typeChain, std::vector<ea_t> & deps)
	{
		//ea_t ea = offset;
		//deref_ptr2(get_idati(), &ea, nullptr);
		tinfo_t ti;
		if (exists_fixup((ea_t)offset)) {//need to detect pointers
			fixup_data_t fd;
			get_fixup(&fd, (ea_t)offset);
			//uval_t u = get_fixup_value(offset, fd.get_type()); //terminate circular types with pointer chain situation
#ifdef __X64__
			//if (!fd.is_extdef()) return false;
			uval_t u = fd.get_base(); //get_fixup_extdef_ea
#else
			//if ((fd.type & FIXUP_EXTDEF) != 0) return false;
			uval_t u = get_fixup_base((ea_t)offset, &fd); // (fd.type & FIXUP_REL) != 0 ? 0 : (fd.sel != BADSEL ? sel2ea(fd.sel) : 0); //get_fixup_extdef_ea
#endif
			u += fd.off;
			if (is_mapped(u) && has_any_name(get_flags(u))) deps.push_back(u);
			if (usedData.find(u) != usedData.end()) return false;
			typeChain.push_back(TypeInfo{ "", (unsigned long long)calc_fixup_size(fd.get_type()), "ptr", false, false, false });
			if (is_mapped(u) && has_any_name(get_flags(u))) {
				std::vector<TypeInfo> nextType;
				if (is_func(get_flags(u))) {
					usedFuncs[u] = true;
					bool bFuncTinfo = getFuncByGuess(u, ti);
					getType(ti, nextType, false);
					if (funcProtoInfos.find(u) == funcProtoInfos.end()) {
						FuncProtoInfo innerFunc;
						getFuncTypeInfo(ti, false, innerFunc);
						qstring name;
						//get_func_name(&name, f->start_ea);
						name = get_short_name((ea_t)offset, GN_STRICT);
						if (name.size() != 0) name = name.substr(0, name.find("(", 0));
						else name = getSymbolName(offset).c_str();
						funcProtoInfos[u] = FuncInfo{ name.c_str(), false, innerFunc };
					}
				} else {
					bool rdonly, volat;
					lookupDataInfo(u, &rdonly, &volat, nextType);
				}
				typeChain.insert(typeChain.end(), nextType.begin(), nextType.end());
			} else {
				typeChain.push_back(TypeInfo{ "void", 0, "void" });
			}
			return true;
		} else {
			unsigned long long size = get_item_size((ea_t)offset);
			//if (is_unknown(offset)) size = next_head(offset, BADADDR) - offset;
			bool foundPtr = false;
			if ((inf_cc_cm & CM_MASK) == CM_N32_F48 && size == 4) {
				uval_t u;
				if (!get_data_value(&u, (ea_t)offset, (asize_t)size)) return false;
				if (is_mapped(u) && has_any_name(get_flags(u))) deps.push_back(u);
				if (usedData.find(u) != usedData.end()) return false;
				if (is_mapped(u) && has_any_name(get_flags(u))) {
					typeChain.push_back(TypeInfo{ "", size, "ptr", false, false, false });
					std::vector<TypeInfo> nextType;
					if (is_func(get_flags(u))) {
						usedFuncs[u] = true;
						bool bFuncTinfo = getFuncByGuess(u, ti);
						getType(ti, nextType, false);
						if (funcProtoInfos.find(u) == funcProtoInfos.end()) {
							FuncProtoInfo innerFunc;
							getFuncTypeInfo(ti, false, innerFunc);
							qstring name;
							//get_func_name(&name, f->start_ea);
							name = get_short_name((ea_t)offset, GN_STRICT);
							if (name.size() != 0) name = name.substr(0, name.find("(", 0));
							else name = getSymbolName(offset).c_str();
							funcProtoInfos[u] = FuncInfo{ name.c_str(), false, innerFunc };
						}
					} else {
						bool rdonly, volat;
						lookupDataInfo(u, &rdonly, &volat, nextType);
					}
					typeChain.insert(typeChain.end(), nextType.begin(), nextType.end());
					return true;
				}
			}
		}
		return false;
	}
	void IdaCallback::consumeTypeInfo(int idx, std::vector<TypeInfo>& tc, unsigned long long ea, std::vector<ea_t> & deps)
	{
		if (tc[idx].size == -1) {
			std::vector<TypeInfo> nextChain;
			tinfo_t ti;
			ti.get_named_type(get_idati(), tc[idx].typeName.c_str());
			qstring qs;
			ti.get_final_type_name(&qs);
			getType(ti, nextChain);
			size_t sz = nextChain.size();
			consumeTypeInfo(0, nextChain, ea, deps); //but if addition occurs from is_strlit... then its lost
			if (nextChain.size() != sz) tc.insert(tc.begin(), *nextChain.begin());
		} else if (tc[idx].metaType == "ptr") {
			std::vector<TypeInfo> nextChain;
			checkPointer(ea, nextChain, deps); //end recursion since pointer is only recursed through new data type if not already traced
		} else if (tc[idx].metaType == "array") { //unions and bitfields consumed in this way
			unsigned long long elSize = tc[idx + 1].size;
			if (elSize == -1) {
				tinfo_t ti;
				if (ti.get_named_type(get_idati(), tc[idx + 1].typeName.c_str())) {
					elSize = ti.get_size();
				}
			}
			if (tc[idx].size == 0) { //0 length array can only be alone or very last member of structure which recursively chains up to a top structure
				tc[idx].size = get_item_size((ea_t)ea);
				tc[idx].arraySize = tc[idx].size / elSize;
			} else {
				//could be an array of structures, arrays, or pointers
				if (!is_strlit(get_flags((ea_t)ea)) || (elSize != sizeof(char) && elSize != sizeof(wchar16_t) && elSize != sizeof(wchar32_t))) { //must prevent auto detection here
					for (int i = 0; i < tc[idx].arraySize; i++) {
						consumeTypeInfo(idx + 1, tc, ea + elSize * i, deps);
					}
				}
			}
		} else if (tc[idx].metaType == "struct") {
			for (size_t i = 0; i < tc[idx].structMembers.size(); i++) {
				consumeTypeInfo(0, tc[idx].structMembers[i].ti, ea + tc[idx].structMembers[i].offset, deps);
			}
		} else if (is_strlit(get_flags((ea_t)ea)) && tc[idx].metaType != "array" && (tc[idx].size == sizeof(char) || tc[idx].size == sizeof(wchar16_t) || tc[idx].size == sizeof(wchar32_t)) &&
			get_item_size((ea_t)ea) % tc[idx].size == 0) {
			//probably need more heuristics than just to verify the type is char, wchar_t or wchar16/32
			tc.insert(tc.begin() + idx, TypeInfo{ "", get_item_size((ea_t)ea), "array", false, false, false, get_item_size((ea_t)ea) / tc[idx].size });
			//msg("wrong string is not array\n");
		}
	}
	std::string IdaCallback::lookupDataInfo(unsigned long long offset, bool* readonly, bool* volatil, std::vector<TypeInfo>& typeChain)
	{
		unsigned long long size = (is_unknown(get_flags((ea_t)offset))) ? calc_max_item_end((ea_t)offset) - offset : get_item_size((ea_t)offset);
		tinfo_t ti;
		std::vector<ea_t> deps;
		//is_strlit(get_flags(offset))
		if (!is_unknown(get_flags((ea_t)offset)) && get_tinfo(&ti, (ea_t)offset)) {
			getType(ti, typeChain, false);
			//recursive pointer scan and also 0 length array fix, string identification fix
			consumeTypeInfo(0, typeChain, offset, deps);
			//0 length arrays cause a problem but only one should occur but could be at the end of a structure
			/*if (typeChain.begin()->metaType == "array" && typeChain.begin()->size == 0) {
				typeChain.begin()->size = size;
				unsigned long long elSize = (typeChain.begin() + 1)->size;
				if (elSize == -1) {
					tinfo_t ti;
					if (ti.get_named_type(get_idati(), (typeChain.begin() + 1)->typeName.c_str())) {
						elSize = ti.get_size();
					}
				}
				typeChain.begin()->arraySize = size / elSize;
			}*/
		} else if (!checkPointer(offset, typeChain, deps)) { //need to detect pointers
			std::string metaType = "unknown";
			int typ = DecompInterface::coreTypeLookup((size_t)size, metaType);
			if (typ == -1 && size != 1) {
				typeChain.push_back(TypeInfo{ "", size, "array", false, false, false, size });
				typeChain.push_back(TypeInfo{ "undefined", 1, metaType });
			} else {
				typeChain.push_back(TypeInfo{ typ == -1 ? "undefined" : defaultCoreTypes[typ].name, size, metaType });
			}
			coreTypeUsed[typ == -1 ? 1 : typ] = true;
		}
		//*size = ti.get_size();
		//qstring qs;
		//print_type(&qs, offset, PRTYPE_MULTI);
		//msg((qs + "\n").c_str());
		typeChain.begin()->isReadOnly = readonly;
		usedData[(ea_t)offset] = typeChain;
		dependentData[(ea_t)offset] = deps;
		getMemoryInfo(offset, readonly, volatil);
		return getSymbolName(offset);
	}

	void IdaCallback::getExternInfo(AddrInfo addr, std::string& callName, std::string& modName, FuncProtoInfo& func)
	{
		if (addr.space != "ram") return;
		modName = modNames[imports[(ea_t)addr.offset].idx];
		if (funcProtoInfos.find((ea_t)addr.offset) != funcProtoInfos.end()) {
			callName = funcProtoInfos[(ea_t)addr.offset].name;
			func = funcProtoInfos[(ea_t)addr.offset].fpi;
		} else {
			executeOnMainThread([this, addr, &callName, &func]() {
				//ids folder contains some basic information for common imports and its in comments in the disassembly but how to properly access the info?
				func_t* f = get_func((ea_t)addr.offset);
				getFuncInfo(addr, f, callName, func);
				});
			funcProtoInfos[(ea_t)addr.offset] = FuncInfo{ callName.c_str(), false, func };
		}
	}
	unsigned long long IdaCallback::getTypeSize(const tinfo_t & ti)
	{
		qstring typeName;
		ti.get_type_name(&typeName);
		int minSize = 0;
		if (typeToAddress.find(typeName.c_str()) != typeToAddress.end()) {
			const std::vector<ea_t>& vec = typeToAddress[typeName.c_str()];
			for (size_t i = 0; i < vec.size(); i++) {
				asize_t sz = get_item_size(vec[i]);
				if (minSize == 0) minSize = (int)sz;
				else minSize = min(minSize, (int)sz);
			}
		}
		return minSize;
	}
	void IdaCallback::getType(const tinfo_t & ti, std::vector<TypeInfo>& typeChain, bool bOuterMost)
	{
		TypeInfo typeinf;
		typeinf.isReadOnly = ti.is_const();
		if (!bOuterMost) { //need to terminate the chain as soon as possible if not outermost
			qstring qs;
			ti.get_type_name(&qs);
			if (qs.size() != 0) {
				typeinf.typeName = qs.c_str();
				typeinf.size = -1;
				typeChain.push_back(typeinf);
				return;
			}
		}
		if (ti.is_typeref()) {
			qstring qs;
			ti.get_next_type_name(&qs);
			if (qs.size() != 0) { //things like function pointers are typeref but dont have names and should return the code type
				typeinf.typeName = qs.c_str();
				typeinf.size = -1;
				typeChain.push_back(typeinf);
				return;
			} //if size is BADSZIE and realtype is unknown then IDA prints these as dummy structures mostly for structures/classes it does not have type info for
		} 
		typeinf.metaType = getMetaTypeInfo(ti);
		typeinf.size = ti.is_func() || ti.is_funcptr() ? 1 : ti.get_size();
		if (typeinf.size == BADSIZE) typeinf.size = ti.get_realtype() == BT_UNK ? getTypeSize(ti) : 0; //0 is void
		typeinf.isEnum = ti.is_enum();

		//get_str_type() != -1 && (get_str_type() & STRWIDTH_2B);
		typeinf.isUtf = false;
		typeinf.isChar = ti.is_char();
		qstring qs;
		//if (!is_anonymous_udt(ti) || !ti.is_union() && !(ti.is_struct() && hasBitFields)) { //type name needed for later lookup as anonymous type could end up being a union/bitfield structure which is converted to data type/array which will not have a name
			ti.get_type_name(&qs);
			typeinf.typeName = qs.c_str();
		//}
		//msg(typeinf.typeName + " " + typeinf.metaType + " " << typeChain.size());
		if (typeinf.metaType == "ptr") {
			ptr_type_data_t pt;
			ti.get_ptr_details(&pt);
			typeChain.push_back(typeinf);
			getType(pt.obj_type, typeChain, false);
		} else if (typeinf.metaType == "struct") {
			//ti.is_udt(); //struct or union
			udt_type_data_t udt; //structures with bitfields and unions are to be dealt with by using a character array
			ti.get_udt_details(&udt);
			bool bHasBitField = false;
			for (size_t i = 0; i < udt.size(); i++) {
				if (udt[i].type.is_bitfield() || (udt[i].offset & 7) != 0) {
					bHasBitField = true; break; //mark but scan for better decompiled type prolog
				}
				std::vector<TypeInfo> memberType;
				getType(udt[i].type, memberType, is_anonymous_udt(udt[i].type));
				typeinf.structMembers.push_back(StructMemberInfo{ udt[i].name.c_str(), udt[i].offset / 8, memberType }); //if not byte multiple bits, round down to nearest byte, what to do as Ghidra uses bytes?
			}
			if (udt.is_union || bHasBitField) {
				typeinf.structMembers.clear();
				if (udt.total_size <= 8) {
					typeinf.metaType = "unknown";
				} else {
					typeinf.metaType = "array";
					typeinf.arraySize = udt.total_size;
				}
			}
			typeChain.push_back(typeinf);
			if ((udt.is_union || bHasBitField) && udt.total_size > 8) typeChain.push_back(TypeInfo{ "byte", 1, "uint" });
		} else if (typeinf.metaType == "array") {
			array_type_data_t ai;
			ti.get_array_details(&ai);
			typeinf.arraySize = ai.nelems;
			typeChain.push_back(typeinf);
			getType(ai.elem_type, typeChain, false);
		} else if (typeinf.metaType == "code") {
			asize_t retSize = 0;
			func_type_data_t ftd;
			ptr_type_data_t pt;
			if (ti.is_funcptr()) {
				ti.get_ptr_details(&pt);
				pt.obj_type.get_func_details(&ftd);
			} else ti.get_func_details(&ftd);
			typeinf.size = (ftd.get_call_method() == FTI_FARCALL || ftd.get_call_method() == FTI_DEFCALL && is_code_far(inf_cc_cm)) ? ph.segreg_size : 0;
			typeinf.size += inf_is_64bit() ? 8 : (inf_is_32bit() ? 4 : 2);
			getFuncTypeInfo(ti.is_funcptr() ? pt.obj_type : ti, true, typeinf.funcInfo);
			typeChain.push_back(typeinf);
		} else { //size greater than 16 needs to be converted to array, really should be 8 as no core types exist
			int typ = DecompInterface::coreTypeLookup((size_t)typeinf.size, typeinf.metaType);
			if (typ == -1 && typeinf.size == 0) { //is_decl_* where there is a forward declaration but not any actual type yields this
				typeChain.push_back(TypeInfo{ "void", 0, "void" });
			} if (typ == -1 && typeinf.size != 1) {
				typeinf.metaType = "array";
				typeinf.arraySize = typeinf.size;
				typeChain.push_back(typeinf);
				typeChain.push_back(TypeInfo{ "undefined", 1, "unknown" });
			} else {
				if (typeinf.typeName.size() == 0) { //dont give type name if already have one
					qs = typ == -1 ? "undefined" : defaultCoreTypes[typ].name.c_str();
					typeinf.typeName = typeinf.isEnum ? "enum" : (typeinf.isChar ? "char" : qs.c_str());
				}
				if (typeinf.isEnum) {
					enum_type_data_t etd;
					ti.get_enum_details(&etd);
					for (size_t i = 0; i < etd.size(); i++) {
						typeinf.enumMembers.push_back(std::pair<std::string, unsigned long long>(etd[i].name.c_str(), etd[i].value));
					}
				}
				typeChain.push_back(typeinf);
			}
			coreTypeUsed[typ == -1 ? (typeinf.size == 0 ? 0 : 1) : typ] = true;
		}
	}
	void IdaCallback::getMetaType(std::string typeName, std::vector<TypeInfo>& typeChain)
	{
		if (typeDatabase.find(typeName) == typeDatabase.end()) {
			executeOnMainThread([this, &typeChain, typeName]() {
				tinfo_t ti;
				if (ti.get_named_type(get_idati(), typeName.c_str())) {
					usedTypes[typeName] = true;
					getType(ti, typeChain);
				} else {
					//msg("Voiding erroneous type: %s\n", typeName.c_str());
					typeChain.push_back(TypeInfo{ "void", 0, "void" });
					coreTypeUsed[0] = true;
				}
			});
			typeDatabase[typeName] = typeChain;
		} else {
			usedTypes[typeName] = true;
			typeChain.insert(typeChain.begin(), typeDatabase[typeName].begin(), typeDatabase[typeName].end());
		}
	}
	//colors are Clang-based: "keyword", "comment", "type", "funcname", "var", "const", "param" and "global"
	std::string IdaCallback::emit(std::string type, std::string color, std::string str)
	{
		//type == "variable" && color == "var" || color == "param"
		if (type == "type" && color == "type") {
			finalTypes[str] = true;
		} else if (type == "funcname" && color == "funcname") {
			finalFuncs[str] = true;
		} else if (type == "variable" && color == "global") {
			finalData[str] = true;
		}
		// else if (type == "variable" && color == "var") { //dont track locals
		//} else if (type == "variable" && color == "param") { //dont track arguments
		//}
		if (color == "comment") return std::string({ COLOR_ON, COLOR_AUTOCMT }) + str + std::string({ COLOR_OFF, COLOR_AUTOCMT });
		else if (color == "param") return std::string({ COLOR_ON, COLOR_LOCNAME }) + str + std::string({ COLOR_OFF, COLOR_LOCNAME });
		else if (color == "var") return std::string({ COLOR_ON, COLOR_LOCNAME }) + str + std::string({ COLOR_OFF, COLOR_LOCNAME });
		else if (color == "global") return std::string({ COLOR_ON, COLOR_DEFAULT }) + str + std::string({ COLOR_OFF, COLOR_DEFAULT });
		else if (color == "const") return std::string({ COLOR_ON, (char)(str.find('\"') == -1 ? COLOR_NUMBER : COLOR_STRING) }) + str + std::string({ COLOR_OFF, (char)(str.find('\"') == -1 ? COLOR_NUMBER : COLOR_STRING) });
		else if (color == "funcname") {
			if (importNames.find(str) != importNames.end()) {
				return std::string({ COLOR_ON, COLOR_IMPNAME }) + str + std::string({ COLOR_OFF, COLOR_IMPNAME });
			} else {
				return std::string({ COLOR_ON, COLOR_DEFAULT }) + str + std::string({ COLOR_OFF, COLOR_DEFAULT });
			}
		} else if (color == "type") return std::string({ COLOR_ON, COLOR_LOCNAME }) + str + std::string({ COLOR_OFF, COLOR_LOCNAME });
		else if (color == "keyword") return std::string({ COLOR_ON, COLOR_KEYWORD }) + str + std::string({ COLOR_OFF, COLOR_KEYWORD });
		else return std::string({ COLOR_ON, COLOR_DEFAULT }) + str + std::string({ COLOR_OFF, COLOR_DEFAULT });
		//COLOR_DREF
		//else return str;
	}
	void IdaCallback::getComments(AddrInfo addr, std::vector<CommentInfo>& comments)
	{
		if (addr.space != "ram") return;
		executeOnMainThread([&comments, addr]() {
			//default options only allow warning, warningheader and user2
			func_t* f = get_func((ea_t)addr.offset);
			if (f != nullptr) {
				qstring qs;
				if (get_func_cmt(&qs, f, false) != -1)
					comments.push_back(CommentInfo{ addr, "warningheader", qs.c_str() });
				if (get_func_cmt(&qs, f, true) != -1)
					comments.push_back(CommentInfo{ addr, "warningheader", qs.c_str() });
			}
			rangeset_t rangeset;
			get_func_ranges(&rangeset, f);
			for (rangeset_t::iterator it = rangeset.begin(); it != rangeset.end(); it++) { //there should be a better way to do this besides going through individual instructions?
				for (ea_t offs = it->start_ea; offs < it->end_ea; offs += get_item_size(offs)) { //is_code(offs) should always be true
					//color_t clr; //get_any_indented_cmt is missing from library
					qstring qs;
					if (get_cmt(&qs, offs, false) != -1)
						comments.push_back(CommentInfo{ AddrInfo{addr.space, offs}, "user1", qs.c_str() });
					if (get_cmt(&qs, offs, true) != -1)
						comments.push_back(CommentInfo{ AddrInfo{addr.space, offs}, "user1", qs.c_str() });
				}
			}
		});
	}
	std::string IdaCallback::getSymbol(AddrInfo addr)
	{
		std::string name;
		if (addr.space == "ram") {
			executeOnMainThread([&name, addr]() {
				name = getSymbolName(addr.offset);
			});
		}
		return name;
	}


	void checkForwardDecl(const tinfo_t & ti, const std::map<std::string, bool>& alreadyDefined, std::map<std::string, bool>& needDecl)
	{
		std::stack<tinfo_t> s;
		s.push(ti);
		while (!s.empty()) {
			tinfo_t t = s.top();
			s.pop();
			udt_type_data_t utd;
			t.get_udt_details(&utd);
			//prevent infinite recursion
			qstring qs;
			t.get_type_name(&qs);
			if (ti == t) {}
			else if (alreadyDefined.find(qs.c_str()) != alreadyDefined.end()) continue;
			else needDecl[qs.c_str()] = true;
			for (size_t i = 0; i < utd.size(); i++) {
				if (!utd[i].type.get_type_name(&qs) || qs.size() == 0) {
					if (utd[i].type.is_typeref()) {
						utd[i].type.get_next_type_name(&qs);
						if (qs.size() != 0) continue;
					}
					if (utd[i].type.is_ptr()) {
						ptr_type_data_t ptd;
						utd[i].type.get_ptr_details(&ptd);
						s.push(ptd.obj_type);
					} else if (utd[i].type.is_array()) {
						array_type_data_t atd;
						utd[i].type.get_array_details(&atd);
						s.push(atd.elem_type);
					} else if (utd[i].type.is_udt()) {
						s.push(utd[i].type);
					} else if (utd[i].type.is_func()) { //funcptr would be handled from is_ptr
						func_type_data_t ftd;
						utd[i].type.get_func_details(&ftd);
						for (size_t j = 0; j < ftd.size(); j++) {
							s.push(ftd[j].type);
						}
							s.push(ftd.rettype);
						}
					} else if (alreadyDefined.find(qs.c_str()) == alreadyDefined.end())
						needDecl[qs.c_str()] = true;
				}
			}
		}

	void buildDependents(std::string str, std::map<std::string, bool>& usedT, std::map<std::string, bool>& isDependee)
	{
		std::stack<std::string> s;
		s.push(str);
		while (!s.empty()) {
			std::string st = s.top();
			s.pop();
			if (usedT.find(st) == usedT.end()) usedT[st] = true;
			if (!usedT[st]) continue;
			usedT[st] = false;
			tinfo_t tinf;
			tinf.get_named_type(get_idati(), st.c_str());
			std::stack<tinfo_t> t;
			t.push(tinf);
			while (!t.empty()) {
				tinfo_t ti = t.top();
				t.pop();
				qstring qs;
				ti.get_type_name(&qs);
				if (qs.size() != 0 && usedT.find(qs.c_str()) != usedT.end() && !usedT[qs.c_str()] && ti != tinf) continue;
				else if (/*!is_anonymous_udt(ti) &&*/ qs.size() != 0) buildDependents(qs.c_str(), usedT, isDependee);
				if (ti.is_typeref()) {
					qs.clear();
					ti.get_next_type_name(&qs);
					if (qs.size() != 0) {
						isDependee[qs.c_str()] = true;
						buildDependents(qs.c_str(), usedT, isDependee);
						continue;
					}
				}
				if (ti.is_ptr()) {
					ptr_type_data_t ptd;
					ti.get_ptr_details(&ptd);
					t.push(ptd.obj_type);
				} else if (ti.is_array()) {
					array_type_data_t atd;
					ti.get_array_details(&atd);
					t.push(atd.elem_type);
				} else if (ti.is_udt()) {
					udt_type_data_t utd;
					ti.get_udt_details(&utd);
					for (size_t i = 0; i < utd.size(); i++) {
						//utd[i].is_anonymous_udm();
						t.push(utd[i].type);
					}
				} else if (ti.is_func()) { //funcptr would be handled from is_ptr
					func_type_data_t ftd;
					ti.get_func_details(&ftd);
					for (size_t j = 0; j < ftd.size(); j++)
						t.push(ftd[j].type);
					t.push(ftd.rettype);
				}
			}
		}
	}

	void refTypes(const std::vector<TypeInfo>& types/*const tinfo_t & ti*/, std::map<std::string, bool>& usedT)
	{
		std::stack<int> s;
		s.push(0);
		while (!s.empty()) {
			int idx = s.top();
			s.pop();
			const TypeInfo& ti = types.at(idx);
			//if (typeInfo == terminate) return ""; //termination happens at appropriate place implicitly
			if (ti.size == -1) {
				usedT[ti.typeName] = true;
				continue;
			}
			if (ti.metaType == "ptr") {
				if (ti.typeName.size() != 0) usedT[ti.typeName] = true;
				s.push(idx + 1);
			} else if (ti.metaType == "struct") {
				std::string strct;
				for (size_t i = 0; i < ti.structMembers.size(); i++) { //core types are not type referenced though
					refTypes(ti.structMembers[i].ti, usedT);
				}
				if (ti.typeName.size() != 0) usedT[ti.typeName] = true;
			} else if (ti.metaType == "array") {
				if (ti.typeName.size() != 0) usedT[ti.typeName] = true;
				s.push(idx + 1);
			} else if (ti.metaType == "code") {
				std::string symbols;
				for (std::vector<SymInfo>::const_iterator it = ti.funcInfo.syminfo.begin(); it != ti.funcInfo.syminfo.end(); it++) {
					refTypes(it->pi.ti, usedT);
				}
				if (ti.typeName.size() != 0) usedT[ti.typeName] = true;
				refTypes(ti.funcInfo.retType.pi.ti, usedT);
			} else if (ti.metaType == "void") {
			} else {
				if (ti.typeName.size() != 0) usedT[ti.typeName] = true;
			}
			//metaType == "uint" && callback->isEnum();
			//metaType == "code"...
		}
	}

	//void IdaCallback::dfsVisitType(const tinfo_t & ti, std::vector<std::string>& sortedTypes, bool firstScan)
	//{
	//	qstring qs;
	//	ti.get_type_name(&qs);
	//	if (qs.size() != 0 && !usedTypes[qs.c_str()] && !firstScan) return;
	//	else if (/*!is_anonymous_udt(ti) &&*/ qs.size() != 0) dfsVisit(qs.c_str(), sortedTypes);
	//	if (ti.is_typeref()) {
	//		qs.clear();
	//		ti.get_next_type_name(&qs);
	//		if (qs.size() != 0) {
	//			dfsVisit(qs.c_str(), sortedTypes);
	//			return;
	//		}
	//	}
	//	if (ti.is_ptr()) {
	//		ptr_type_data_t ptd;
	//		ti.get_ptr_details(&ptd);
	//		dfsVisitType(ptd.obj_type, sortedTypes, false);
	//	} else if (ti.is_array()) {
	//		array_type_data_t atd;
	//		ti.get_array_details(&atd);
	//		dfsVisitType(atd.elem_type, sortedTypes, false);
	//	} else if (ti.is_udt()) {
	//		udt_type_data_t utd;
	//		ti.get_udt_details(&utd);
	//		for (size_t i = 0; i < utd.size(); i++)
	//			dfsVisitType(utd[i].type, sortedTypes, false);
	//	} else if (ti.is_func()) { //funcptr would be handled from is_ptr
	//		func_type_data_t ftd;
	//		ti.get_func_details(&ftd);
	//		for (size_t j = 0; j < ftd.size(); j++)
	//			dfsVisitType(ftd[j].type, sortedTypes, false);
	//		dfsVisitType(ftd.rettype, sortedTypes, false);
	//	}
	//}

	//note post order only!
	//this only works properly in all cases if you start with types which have no dependents
	//first must compute these and place them at the top of the list, the SCC not found as part of this would come after
	/*void IdaCallback::dfsVisit(std::string str, std::vector<std::string>& sortedTypes)
	{
		if (!usedTypes[str]) return;
		usedTypes[str] = false;
		tinfo_t ti;
		ti.get_named_type(get_idati(), str.c_str());
		dfsVisitType(ti, sortedTypes, true);
		sortedTypes.push_back(str);
	}*/
	void dfsTypeVisit(std::string str, std::map<std::string, bool>& unvisited, std::vector<std::string>& sortedTypes)
	{
		std::stack<std::string> s; //should switch to stack based
		//std::map<std::string, bool> tempMarkers;
		//std::copy(unvisited.begin(), unvisited.end(), std::inserter(tempMarkers, tempMarkers.end()));
		//tempMarkers.insert(unvisited.begin(), unvisited.end());
		//post order traversal can be considered to have 3 states - discovered, undiscovered and visited
		if (!unvisited[str]) return;
		s.push(str);
		//unvisited[str] = false;
		std::map<std::string, bool> unresolved = unvisited; //prevent infinite recursion
		while (s.size() != 0) {
			std::string v = s.top();
			tinfo_t ti;
			ti.get_named_type(get_idati(), v.c_str());
			std::stack<tinfo_t> t;
			t.push(ti);
			std::stack<std::string> rev;
			while (t.size() != 0) { //this also requires a post order traversal or the order will be wrong
				qstring qs, nm;
				const tinfo_t & inti = t.top();
				inti.get_type_name(&nm);
				unresolved[nm.c_str()] = false;
				if (!unvisited[nm.c_str()]) { t.pop(); continue; }
				bool bInnerUnvstChdrn = false;
				if (inti.is_ptr()) {
					ptr_type_data_t ptd;
					inti.get_ptr_details(&ptd);
					ptd.obj_type.get_type_name(&qs);
					if (qs.size() != 0 && unresolved[qs.c_str()]) { t.push(ptd.obj_type); bInnerUnvstChdrn = true; }
				} else if (inti.is_array()) {
					array_type_data_t atd;
					inti.get_array_details(&atd);
					atd.elem_type.get_type_name(&qs);
					if (qs.size() != 0 && unresolved[qs.c_str()]) { t.push(atd.elem_type); bInnerUnvstChdrn = true; }
				} else if (inti.is_udt()) {
					udt_type_data_t utd;
					inti.get_udt_details(&utd);
					for (size_t i = 0; i < utd.size(); i++) {
						utd[i].type.get_type_name(&qs);
						if (/*!is_anonymous_udt(inti) &&*/ qs.size() != 0 && unresolved[qs.c_str()]) { t.push(utd[i].type); bInnerUnvstChdrn = true; }
					}
				} else if (inti.is_func()) { //funcptr would be handled from is_ptr
					func_type_data_t ftd;
					ti.get_func_details(&ftd);
					for (size_t j = 0; j < ftd.size(); j++) {
						ftd[j].type.get_type_name(&qs);
						if (qs.size() != 0 && unresolved[qs.c_str()]) { t.push(ftd[j].type); bInnerUnvstChdrn = true; }
					}
					t.push(ftd.rettype);
					ftd.rettype.get_type_name(&qs);
					if (qs.size() != 0 && unresolved[qs.c_str()]) { t.push(ftd.rettype); bInnerUnvstChdrn = true; }
				}
				if (!bInnerUnvstChdrn) {
					if (inti.is_typeref()) {
						qs.clear();
						inti.get_next_type_name(&qs);
						if (qs.size() != 0) {
							if (unvisited[qs.c_str()]) {
								unvisited[qs.c_str()] = false; rev.push(qs.c_str());
							}
						}
					}
					unvisited[nm.c_str()] = false;
					if (inti != ti) rev.push(nm.c_str());
					t.pop();
				}
			}
			if (!rev.empty()) {
				while (!rev.empty()) {
					s.push(rev.top());
					rev.pop();
				}
			} else {
				s.pop();
				sortedTypes.push_back(v);
			}
		}
	}
	/*void IdaCallback::addAllDataRefs(ea_t ea, std::map<ea_t, bool>& refs, std::map<ea_t, bool>& isDependee, std::map<ea_t, bool>& frefs)
	{
		if (refs.find(ea) != refs.end() || frefs.find(ea) != frefs.end()) return;
		if (usedData.find(ea) == usedData.end()) frefs[ea] = true;
		else {
			refs[ea] = true;
		}
		for (size_t i = 0; i < dependentData[ea].size(); i++) {
			if (usedData.find(dependentData[ea][i]) != usedData.end()) isDependee[dependentData[ea][i]] = true;
			addAllDataRefs(dependentData[ea][i], refs, isDependee, frefs);
		}
	}*/

	void addAllDataRefs(ea_t ea, const std::map<ea_t, std::vector<ea_t>>& depData,
		std::map<ea_t, std::vector<TypeInfo>>& usdData,
		std::map<ea_t, bool>& refs, std::map<ea_t, bool>& isDependee, std::map<ea_t, bool>& frefs)
	{
		std::stack<ea_t> s;
		s.push(ea);
		while (s.size() != 0) {
			ea_t e = s.top();
			s.pop();
			if (refs.find(e) != refs.end() || frefs.find(e) != frefs.end()) continue;
			if (usdData.find(e) == usdData.end()) frefs[e] = true;
			else {
				refs[e] = true;
				const std::vector<ea_t>& vec = depData.at(e);
				for (size_t i = 0; i < vec.size(); i++) {
					if (usdData.find(vec[i]) != usdData.end()) isDependee[vec[i]] = true;
					s.push(vec[i]);
				}
			}
		}
	}

	//this only works properly in all cases if you start with types which have no dependents
	//first must compute these and place them at the top of the list, the SCC not found as part of this would come after
	/*void IdaCallback::dfsVisitData(ea_t ea, std::vector<ea_t>& sortedData, std::map<ea_t, bool>& visited)
	{
		if (visited.find(ea) == visited.end()) visited[ea] = true;
		if (!visited[ea]) return;
		visited[ea] = false;
		for (int i = 0; i < dependentData[ea].size(); i++) {
			if (usedData.find(dependentData[ea][i]) != usedData.end()) dfsVisitData(dependentData[ea][i], sortedData, visited);
		}
		sortedData.push_back(ea);
	}*/

	void dfsVisitData(ea_t ea, const std::map<ea_t, std::vector<ea_t>>& depData,
		std::map<ea_t, std::vector<TypeInfo>>& usdData,
		std::vector<ea_t>& sortedData, std::map<ea_t, bool>& unvisited)
	{
		std::stack<ea_t> s; //should switch to stack based
		if (unvisited.find(ea) != unvisited.end() && !unvisited[ea]) return;
		s.push(ea);
		unvisited[ea] = false;
		while (s.size() != 0) {
			ea_t e = s.top();
			bool hasUnvisitChdrn = false;
			const std::vector<ea_t>& vec = depData.at(e);
			for (size_t i = 0; i < vec.size(); i++) {
				if (usdData.find(vec[i]) != usdData.end()) {
					if (unvisited.find(vec[i]) == unvisited.end() || unvisited[vec[i]]) {
						unvisited[vec[i]] = false; s.push(vec[i]); hasUnvisitChdrn = true;
					}
				}
			}
			if (!hasUnvisitChdrn) {
				s.pop();
				sortedData.push_back(e);
			}
		}
	}

	/*void IdaCallback::checkFwdData(ea_t ea, const std::map<ea_t, bool>& alreadyDefined, std::map<ea_t, bool>& needDecl)
	{
		if (alreadyDefined.find(ea) != alreadyDefined.end()) return;
		else needDecl[ea] = true;
		for (size_t i = 0; i < dependentData[ea].size(); i++) {
			checkFwdData(dependentData[ea][i], alreadyDefined, needDecl);
		}
	}*/

	void checkFwdData(ea_t ea, const std::map<ea_t, std::vector<ea_t>>& depData,
		const std::map<ea_t, bool>& alreadyDefined, std::map<ea_t, bool>& needDecl)
	{
		std::stack<ea_t> s;
		s.push(ea);
		while (s.size() != 0) {
			ea_t e = s.top();
			s.pop();
			if (ea == e) {}
			else if (depData.find(e) == depData.end()) continue; //function pointers not in here
			else if (alreadyDefined.find(e) != alreadyDefined.end()) continue;
			else needDecl[e] = true;
			const std::vector<ea_t>& vec = depData.at(e);
			for (size_t i = 0; i < vec.size(); i++) {
				s.push(vec[i]);
			}
		}
	}

	std::string IdaCallback::dumpIdaInfo()
	{
		std::string str = "IDA SDK selected decompilation useful reported information dump follows:\n\n";
		str = "Processor: " + inf_procname +
			" Is 32-bit: " + std::string(inf_is_32bit() ? "yes" : "no") +
			" Is 64-bit: " + std::string(inf_is_64bit() ? "yes" : "no") +
			" Is big-endian: " + std::string(inf_is_be() ? "yes" : "no") +
			" MaxPtrSize: " + std::to_string(ph.max_ptr_size()) +
			" SegRegSize: " + std::to_string(ph.segreg_size) +
			" SegmBitness: " + std::to_string(ph.get_segm_bitness()) +
			" StkArgOffs: " + std::to_string(ph.get_stkarg_offset()) +
			" Use64: " + std::to_string(ph.use64()) +
			" Use32: " + std::to_string(ph.use32()) +
			" DefCCModl: 0x" + to_string((uint)inf_cc_cm, std::hex) +
			"\n\n";
		const til_t* idati = get_idati();
		str += "Number of registers: " + std::to_string(ph.regs_num) + "\n";
		for (int i = 0; i < ph.regs_num; i++) {
			reg_info_t ri, mnri;
			bitrange_t bits, mnbits;
			//get_reg_info(ph.reg_names[i], &bits);
			const char* regnm = nullptr;
			qstring qs;
			regnm = get_reg_info(ph.reg_names[i], &bits);
			parse_reg_name(&ri, ph.reg_names[i]);
			bool bSame = regnm == nullptr || strcmp(regnm, ph.reg_names[i]) == 0;
			if (!bSame) {
				get_reg_info(regnm, &mnbits);
				parse_reg_name(&mnri, regnm);
				get_reg_name(&qs, i, mnri.size);
			}
			str += "Idx: " + std::to_string(i) + " Reg: " + ph.reg_names[i] + " Size: " + std::to_string(ri.size) +
				" BitOffs: " + std::to_string(bits.bitoff()) + " Bits: " + std::to_string(bits.bitsize()) +
				(!bSame ? +" MainNm: " + std::string(regnm) + " Size: " + std::to_string(mnri.size) +
					" BitOffs: " + std::to_string(mnbits.bitoff()) + " Bits: " + std::to_string(mnbits.bitsize()) + " SzName: " + std::string(qs.c_str()) : "") +
				"\n";
		}
		//ph.reg_first_sreg, ph.reg_last_sreg, ph.segreg_size
		str += "\n";
		size_t num = 0;
		ea_t ea = get_first_fixup_ea();
		while (ea != BADADDR) {
			fixup_data_t fd;
			get_fixup(&fd, ea);
			qstring qs;
			get_fixup_desc(&qs, ea, fd);
			//fd.get_desc(&qs, ea); //has color info
			tag_remove(&qs);
			str += "Fixup: " + std::string(qs.c_str()) + " Addr: 0x" + to_string(ea, std::hex) +
				" Size: 0x" + to_string(calc_fixup_size(fd.get_type()), std::hex) + " Type: 0x" + to_string(fd.get_type(), std::hex) + "\n";
			num++;
			ea = get_next_fixup_ea(ea);
		}
		str += "Total fixups: " + std::to_string(num) + "\n\n";
		num = get_selector_qty();
		str += "Number of selectors: " + std::to_string(num) + "\n";
		for (size_t i = 0; i < num; i++) {
			sel_t base;
			getn_selector(&base, &ea, (int)i);
			segment_t* seg = get_segm_by_sel(base);
			qstring qs;
			get_segm_name(&qs, seg);
			str += "Selector Base: 0x" + to_string(base, std::hex) + " Addr: 0x" + to_string(ea, std::hex) + " Seg: " + std::string(qs.c_str()) + "\n";
		}
		if (num != 0) str += "\n";
		num = get_segm_qty();
		str += "Number of segments: " + std::to_string(num) + "\n";
		for (size_t i = 0; i < num; i++) {
			segment_t* seg = getnseg((int)i);
			qstring qs, qc, qcr, qcls;
			get_segm_name(&qs, seg);
			get_segment_cmt(&qc, seg, false);
			get_segment_cmt(&qcr, seg, false);
			get_segm_class(&qcls, seg);
			str += "Segment: " + std::string(qs.c_str()) + " Cmt: " + std::string(qc.c_str()) + " CmtRpt: " + std::string(qcr.c_str()) + " Class: " + std::string(qcls.c_str()) +
				" Sel: 0x" + to_string(seg->sel, std::hex) + " Align: 0x" + to_string((uint)seg->align, std::hex) + " Bitness: 0x" + to_string((uint)seg->bitness, std::hex) +
				" Comb: 0x" + to_string((uint)seg->comb, std::hex) + " Perm: 0x" + to_string((uint)seg->perm, std::hex) + " Flags: 0x" + to_string(seg->flags, std::hex) +
				" Type: 0x" + to_string((uint)seg->type, std::hex) + " StartEA: 0x" + to_string(seg->start_ea, std::hex) + " EndEA: 0x" + to_string(seg->end_ea, std::hex) + "\n";
		}
		if (num != 0) str += "\n";
		str += "til name: " + std::string(idati->name) + " Description: " + std::string(idati->desc) + " Bases: " + std::to_string(idati->nbases) + "\n";
		for (int i = 0; i < idati->nbases; i++) {
			str += "Base til name: " + std::string(idati->base[i]->name) + " Description: " + std::string(idati->base[i]->desc) + "\n";
		}
		//idati->types;
		num = get_idasgn_qty();
		str += "Number of signatures: " + std::to_string(num) + "\n";
		for (size_t i = 0; i < num; i++) {
			qstring signame, optlibs, longname;
			get_idasgn_desc(&signame, &optlibs, (int)i);
			//idasgn_t* is = get_idasgn_header_by_short_name(signame.c_str());
			get_idasgn_title(&longname, signame.c_str());
			str += "Signature short form name: " + std::string(signame.c_str()) + " Optional Libraries: " + std::string(optlibs.c_str()) + " Long form name: " + std::string(longname.c_str()) + "\n";
		}
		str += "\nNumber of imported modules: " + std::to_string(modNames.size()) + "\n";
		for (std::map<ea_t, ImportInfo>::iterator it = imports.begin(); it != imports.end(); it++) {
			qstring qs, qn, name;
			get_long_name(&qs, it->first);
			get_name(&qn, it->first);
			name = get_short_name(it->first);
			if (name.size() != 0) {
				name = name.substr(0, name.find("(", 0));
			}
			str += "Long name: " + std::string(qs.c_str()) + " Name: " + std::string(qn.c_str()) + " Callable name: " + std::string(name.c_str());
			tinfo_t tif;
			if (getFuncByGuess(it->first, tif)) {
				print_type(&qs, it->first, PRTYPE_1LINE);
				func_type_data_t ftd;
				tif.get_func_details(&ftd);
				str += " Type: " + std::string(qs.c_str()) + " Args: 0x" + to_string(ftd.size(), std::hex) + " RetType: 0x" + to_string(ftd.retloc.atype(), std::hex) + " RetSize: 0x" + to_string(ftd.rettype.get_size(), std::hex) + "\n";
			} else str += "\n";
		}
		num = get_entry_qty();
		str += "\nNumber of entry points: " + std::to_string(num) + "\n";
		for (size_t i = 0; i < num; i++) {
			std::string disp;
			ea_t ea = get_entry(get_entry_ordinal(i)); //qstring qs; get_entry_name(&qs, i);
			str += "Entry point ordinal: " + std::to_string(i) + " Addr: 0x" + to_string(ea, std::hex) + "\n";
		}
		num = get_func_qty();
		str += "\nNumber of functions: " + std::to_string(num) + "\n";
		for (size_t i = 0; i < num; i++) {
			func_t* f = getn_func(i);
			if (f == nullptr) continue;
			qstring qs, qt;
			get_func_name(&qs, f->start_ea);
			tinfo_t tif;
			get_long_name(&qt, f->start_ea);
			str += "Name: " + std::string(qs.c_str()) + " Long name: " + std::string(qt.c_str()) + " Addr: 0x" + to_string(f->start_ea, std::hex) + " LocVars: 0x" + to_string(f->frsize, std::hex) +
				" SaveRegs: 0x" + to_string(f->frregs, std::hex) + " PopArgs: 0x" + to_string(f->argsize, std::hex) + " AFlags: 0x" + to_string(get_aflags(f->start_ea), std::hex) + " AFlags0: 0x" + to_string(get_aflags0(f->start_ea), std::hex) +
				" RegArgs: 0x" + to_string(f->regargqty, std::hex) + " FrmPtrDelta: 0x" + to_string(f->fpd, std::hex) + " StkPts: 0x" + to_string(f->pntqty, std::hex) + "\n";
			if (getFuncByGuess(f->start_ea, tif)) {
				print_type(&qt, f->start_ea, PRTYPE_1LINE);
				func_type_data_t ftd;
				tif.get_func_details(&ftd);
				ftd.dump(&qs);
				qs.remove_last(1); //trailing new line
				str += "Type: " + std::string(qt.c_str()) + " Args: 0x" + to_string(ftd.size(), std::hex) + " CallMthd: 0x" + to_string(ftd.get_call_method(), std::hex) + " CC: 0x" + to_string((uint)ftd.cc, std::hex) + " Dump:\n" + qs.c_str() + "\n";
				for (size_t i = 0; i < ftd.size(); i++) {
					str += "ArgName: " + std::string(ftd[i].name.c_str()) + " ArgCmt: " + std::string(ftd[i].cmt.c_str()) + " ArgLoc: 0x" + to_string(ftd[i].argloc.is_stkoff() ? ftd[i].argloc.stkoff() : ftd[i].argloc.reg1(), std::hex) + "\n";
				}
			}
			if (f->frame != BADNODE) {
				struc_t* frame = get_frame(f->start_ea); //get_member_name has sometimes crashed with error 836
				str += "Frame members: 0x" + to_string(frame->memqty, std::hex) + " Frame size: 0x" + to_string(get_frame_size(f), std::hex) + " LocOffs: 0x" + to_string(frame_off_lvars(f), std::hex) +
					" SaveOffs: 0x" + to_string(frame_off_savregs(f), std::hex) + " RetOffs: 0x" + to_string(frame_off_retaddr(f), std::hex) + " RetSize: 0x" + to_string(get_frame_retsize(f), std::hex) + " ArgOffs: 0x" + to_string(frame_off_args(f), std::hex) + "\n";
				for (uint32 i = 0; i < frame->memqty; i++) {
					str += "Member name: " + std::string(get_member_name(frame->members[i].id).c_str()) + " Offset: 0x" + to_string(frame->members[i].get_soff(), std::hex) + "\n";
				}
			}
			//if (f->points == nullptr) get_spd(f, f->start_ea);
			/*if (f->points != nullptr) {
				for (int i = 0; i < f->pntqty; i++) {
					sval_t s = f->points[i].spd;
					str += "StackAdj: 0x" + to_string(s, std::hex) + " Offset: 0x" + to_string(f->points[i].ea, std::hex) + "\n";
				}
			}
			for (ea_t i = get_first_cref_to(f->start_ea); i != BADADDR;)
			{
				func_t* fp = get_func(i);
				if (fp->points == nullptr) get_spd(fp, fp->start_ea);
				str += "CdXref: 0x" + to_string(i, std::hex) + " SpDelta: " + to_string(get_spd(fp, get_item_end(i)), std::hex) +
					" SpEfctvDelta: " + to_string(get_effective_spd(fo, get_item_end(i)), std::hex) + " SpDelta: " + to_string(get_sp_delta(fp, get_item_end(i)), std::hex) + "\n";
				i = get_next_cref_to(f->start_ea, i);
				//str += (i == BADADDR) ? "\n" : " ";
			}*/
			if (f->regargs == nullptr) read_regargs(f);
			for (int i = 0; i < f->regargqty; i++) { //name can be a nullptr!
				qstring out, qs;
				size_t size = 0;
				//regargs are destroyed when the full function type is determined
				//if (tif.is_func()) {
					//qs = ph.reg_names[f->regargs[i].reg];
				//} else 
				if (f->regargs[i].type != nullptr) {
					tinfo_t ti; //The type information is internally kept as an array of bytes terminated by 0.
					qtype typeForDeser;
					typeForDeser.append(f->regargs[i].type);
					ti.deserialize(get_idati(), &typeForDeser); //will free buffer passed!
					ti.get_type_name(&out);
					size = ti.get_size();
					get_reg_name(&qs, f->regargs[i].reg, size);
					//qs = ph.reg_names[f->regargs[i].reg];
				} else qs = ph.reg_names[f->regargs[i].reg];
				str += "RegArgName: " + std::string(f->regargs[i].name == nullptr ? "" : f->regargs[i].name) +
					" Reg: " + std::string(qs.c_str()) + " Idx: 0x" + to_string(f->regargs[i].reg, std::hex) +
					" Size: 0x" + to_string(size, std::hex) +
					" TDataLen: 0x" + to_string(strlen((const char*)f->regargs[i].type), std::hex) + " Type: " + std::string(out.c_str()) + "\n";
			}
			if (f->regvars == nullptr) find_regvar(f, f->start_ea, nullptr);
			if (f->regvarqty != -1 && f->regvars != nullptr) {
				for (int i = 0; i < f->regvarqty; i++) {
					str += "RegVar: " + std::string(f->regvars[i].user != nullptr ? f->regvars[i].user : "") + " Reg: " + std::string(f->regvars[i].canon) + " Cmt: " + std::string(f->regvars[i].cmt == nullptr ? "" : f->regvars[i].cmt) + "\n";
				}
			}
			str += "\n";
		}
		return str;
	}

	std::string colorize(std::string str, bool bColor, char clr)
	{
		return bColor ? std::string({ COLOR_ON, clr }) + str + std::string({ COLOR_OFF, clr }) : str;
	}
	std::string printTypeForCode(const tinfo_t & t, std::string name, bool bColor, char NameColor, bool bTop, size_t indnt) //unions, bitfields and typedefs to default built in types require this
	{
		std::string out;
		std::stack<std::tuple<std::string, tinfo_t, bool>> s;
		s.push(std::tuple<std::string, tinfo_t, bool>(name, t, false));
		while (!s.empty()) {
			std::tuple<std::string, tinfo_t, bool> item = s.top();
			s.pop();
			if (std::get<2>(item)) { //post order time where index is complement
				const tinfo_t& ti = std::get<1>(item);
				if (ti.is_ptr() || ti.is_array() && t != ti) {
					out += std::string(std::get<0>(item).size() != 0 || ti.is_const() ? "* " : "*") + (ti.is_const() ? colorize("const", bColor, COLOR_KEYWORD) + (std::get<0>(item).size() != 0 ? " " : "") : "") + colorize(std::get<0>(item), bColor, NameColor);
				} else if (ti.is_array()) {
					array_type_data_t atd;
					ti.get_array_details(&atd); //pointer to array should be treated as pointer to pointer
					out += (std::get<0>(item).size() != 0 ? " " : "") + colorize(std::get<0>(item), bColor, NameColor) + "[" + std::to_string(atd.nelems) + "]";
				}
				continue;
			}
			const tinfo_t& ti = std::get<1>(item);
			qstring qs;
			ti.get_type_name(&qs);
			if ((!bTop || t != ti) && qs.size() != 0) out += (ti.is_const() ? colorize("const", bColor, COLOR_KEYWORD) + " " : "") + colorize(qs.c_str(), bColor, COLOR_LOCNAME) + (std::get<0>(item).size() != 0 ? " " : "") + colorize(std::get<0>(item), bColor, NameColor);
			else if (ti.is_ptr() && !ti.is_funcptr()) {
				ptr_type_data_t ptd;
				ti.get_ptr_details(&ptd);
				s.push(std::tuple<std::string, tinfo_t, bool>(std::get<0>(item), std::get<1>(item), true));
				s.push(std::tuple<std::string, tinfo_t, bool>("", ptd.obj_type, false));
			} else if (ti.is_array()) {
				array_type_data_t atd;
				ti.get_array_details(&atd);
				s.push(std::tuple<std::string, tinfo_t, bool>(std::get<0>(item), std::get<1>(item), true));
				s.push(std::tuple<std::string, tinfo_t, bool>("", atd.elem_type, false));
			} else if (ti.is_funcptr()) {
				func_type_data_t ftd;
				ti.get_func_details(&ftd);
				std::string str;
				for (size_t i = 0; i < ftd.size(); i++) {
					if (i != 0) str += ", ";
					str += printTypeForCode(ftd[i].type, ftd[i].name.c_str(), bColor, NameColor, false, indnt);
					if (ftd[i].cmt.size() != 0) str += colorize(" /*" + std::string(ftd[i].cmt.c_str()) + "*/", bColor, COLOR_AUTOCMT);
				}
				std::string model = ccToStr(ftd.cc, ftd.get_call_method(), false);
				out += printTypeForCode(ftd.rettype, "", bColor, NameColor, false, indnt) + " (" + (model != "default" && model != "unknown" ? colorize(model, bColor, COLOR_KEYWORD) + " " : "") + "*" + colorize(std::get<0>(item), bColor, COLOR_LOCNAME) + ")(" + str + ")";
			} else if (ti.is_udt()) {
				udt_type_data_t utd;
				ti.get_udt_details(&utd);
				std::string str;
				for (size_t i = 0; i < utd.size(); i++) {
					if (utd[i].type.is_bitfield()) {
						bitfield_type_data_t btd;
						utd[i].type.get_bitfield_details(&btd);
						str += std::string(indnt + 2, ' ') + colorize(std::string(utd[i].name.c_str()), bColor, NameColor) + ":" + std::to_string(btd.width);
					} else str += std::string(indnt + 2, ' ') + printTypeForCode(utd[i].type, utd[i].name.c_str(), bColor, NameColor, is_anonymous_udt(utd[i].type), indnt + 2);
					str += ";" + colorize(std::string(utd[i].cmt.size() != 0 ? " //" + std::string(utd[i].cmt.c_str()) : ""), bColor, COLOR_AUTOCMT) + "\n";
				}
				out += colorize(utd.is_union ? "union" : "struct", bColor, COLOR_KEYWORD) + " {\n" + str + std::string(indnt, ' ') + "}" + (std::get<0>(item).size() != 0 ? " " : "") + colorize(std::get<0>(item), bColor, NameColor);
			} else if (ti.is_enum()) {
				enum_type_data_t etd;
				ti.get_enum_details(&etd);
				out += colorize("enum", bColor, COLOR_KEYWORD) + " {\n";
				for (size_t i = 0; i < etd.size(); i++) {
					out += std::string(indnt + 2, ' ') + std::string(etd[i].name.c_str()) + " = " + std::to_string(etd[i].value) + (i != etd.size() - 1 ? "," : "") + colorize(std::string(etd[i].cmt.size() != 0 ? " //" + std::string(etd[i].cmt.c_str()) : ""), bColor, COLOR_AUTOCMT) + "\n";
				}
				out += "}" + std::string(std::get<0>(item).size() != 0 ? " " : "") + colorize(std::get<0>(item), bColor, NameColor);
			} else if (is_type_int(ti.get_realtype()) || ti.is_uint() || ti.is_floating() || ti.is_void() || ti.is_bool() || ti.is_unknown()) {
				qstring qs;
				ti.print(&qs, nullptr, PRTYPE_DEF);
				out += colorize(qs.c_str(), bColor, COLOR_KEYWORD) + (std::get<0>(item).size() != 0 ? " " + colorize(std::get<0>(item), bColor, NameColor) : "");
			}
		}
		return out;
	}
	std::string printTypeForCode(const std::vector<TypeInfo>& types, std::string name, bool bColor, char NameColor, bool bTop, size_t indnt)
	{
		std::string out;
		std::stack<std::pair<std::string, int>> s;
		s.push(std::pair<std::string, int>(name, 0));
		while (!s.empty()) {
			std::pair<std::string, int> item = s.top();
			s.pop();
			if (item.second < 0) { //post order time where index is complement
				const TypeInfo& ti = types.at(~item.second);
				if (ti.metaType == "ptr" || ti.metaType == "array" && 0 != ~item.second) {
					out += std::string(item.first.size() != 0 || ti.isReadOnly ? "* " : "*") + (ti.isReadOnly ? colorize("const", bColor, COLOR_KEYWORD) + (item.first.size() != 0 ? " " : "") : "") + colorize(item.first, bColor, NameColor);
				} else if (ti.metaType == "array") { //pointer to array should be treated as pointer to pointer
					out += (item.first.size() != 0 ? " " : "") + colorize(item.first, bColor, NameColor) + "[" + std::to_string(ti.arraySize) + "]";
				}
				continue;
			}
			const TypeInfo& ti = types.at(item.second);
			tinfo_t tinf;
			tinf.get_named_type(get_idati(), ti.typeName.c_str());
			bool bHasBitFields = false;
			if (tinf.is_struct()) {
				udt_type_data_t utd;
				tinf.get_udt_details(&utd);
				for (size_t i = 0; i < utd.size(); i++) {
					if (utd[i].type.is_bitfield()) {
						bHasBitFields = true; break;
					}
				}
			}
			if ((!bTop || item.second != 0) && ti.typeName.size() != 0) out += (ti.isReadOnly ? colorize("const", bColor, COLOR_KEYWORD) + " " : "") + colorize(ti.typeName, bColor, COLOR_LOCNAME) + (item.first.size() != 0 ? " " : "") + colorize(item.first, bColor, NameColor);
			else if (bTop &&
					(ti.metaType == "int" || ti.metaType == "uint" || ti.metaType == "void" ||
						ti.metaType == "bool" || ti.metaType == "float" || ti.metaType == "unknown") ||
						(tinf.is_union() || tinf.is_struct() && bHasBitFields) /*&& !ti.is_typeref()*/) { //&& has_bitfields
				out += printTypeForCode(tinf, item.first, bColor, NameColor, bTop, indnt);
			} else if (ti.metaType == "ptr") {
				s.push(std::pair<std::string, int>(item.first, ~item.second));
				s.push(std::pair<std::string, int>("", item.second + 1));
			} else if (ti.metaType == "array") {
				s.push(std::pair<std::string, int>(item.first, ~item.second));
				s.push(std::pair<std::string, int>("", item.second + 1));
			} else if (ti.metaType == "code") {
				std::string str;
				for (size_t i = 0; i < ti.funcInfo.syminfo.size(); i++) {
					if (i != 0) str += ", ";
					str += printTypeForCode(ti.funcInfo.syminfo[i].pi.ti, ti.funcInfo.syminfo[i].pi.name, bColor, NameColor, false, indnt);
				}
				out += printTypeForCode(ti.funcInfo.retType.pi.ti, "", bColor, NameColor, false, indnt) + " (" + (ti.funcInfo.model != "default" && ti.funcInfo.model != "unknown" ? colorize(ti.funcInfo.model, bColor, COLOR_KEYWORD) + " " : "") + "*" + colorize(item.first, bColor, COLOR_LOCNAME) + ")(" + str + ")";
			} else if (ti.metaType == "struct") { //note: bitfields and unions are not handled here except as byte arrays
				std::string str;
				for (size_t i = 0; i < ti.structMembers.size(); i++) {
					str += std::string(indnt + 2, ' ') + printTypeForCode(ti.structMembers[i].ti, ti.structMembers[i].name, bColor, NameColor, ti.structMembers[i].name == "", indnt + 2) + ";\n";
				}
				out += colorize("struct", bColor, COLOR_KEYWORD) + " {\n" + str + std::string(indnt, ' ') + "}" + (item.first.size() != 0 ? " " : "") + colorize(item.first, bColor, NameColor);
			} else if (ti.isEnum) {
				out += colorize("enum", bColor, COLOR_KEYWORD) + " {\n";
				for (size_t i = 0; i < ti.enumMembers.size(); i++) {
					out += std::string(indnt + 2, ' ') + std::string(ti.enumMembers[i].first.c_str()) + " = " + std::to_string(ti.enumMembers[i].second) + (i != ti.enumMembers.size() - 1 ? "," : "") + "\n";
				}
				out += "}" + std::string(item.first.size() != 0 ? " " : "") + colorize(item.first, bColor, NameColor);
			} else {}
		}
		return out;
	}

	std::string IdaCallback::initForType(const tinfo_t & t, ea_t addr, bool bColor) //unions/bitfields require this method
	{
		std::string out;
		std::stack<std::tuple<ea_t, tinfo_t, int>> s;
		s.push(std::tuple<ea_t, tinfo_t, int>(addr, t, 0));
		while (!s.empty()) {
			std::tuple<ea_t, tinfo_t, int> item = s.top();
			s.pop();
			if (std::get<2>(item) == -1) {
				out += " }";
				continue;
			} else if (std::get<2>(item) == -2) {
				out += ", ";
				continue;
			}
			const tinfo_t& ti = std::get<1>(item);
			std::string str;
			if (ti.is_typeref()) {
				qstring qs;
				ti.get_next_type_name(&qs);
				tinfo_t tinf;
				tinf.get_named_type(get_idati(), qs.c_str());
				str = initForType(tinf, std::get<0>(item), bColor);
			}  else if (ti.is_udt()) {
				udt_type_data_t utd;
				ti.get_udt_details(&utd);
				str = "{ ";
				if (utd.is_union) { //how to choose member? size of each member could be matched with next data, pointers in union could be matched to pointers in data
					if (utd.size() != 0) str += initForType(utd[0].type, std::get<0>(item), bColor);
				} else {
					for (size_t i = 0; i < utd.size(); i++) {
						if (i != 0) str += ", ";
						if (utd[i].type.is_bitfield()) {
							bitfield_type_data_t btd;
							utd[i].type.get_bitfield_details(&btd);
							uint64 val = 0;
							size_t totalSize = (size_t)((utd[i].offset + btd.width + 7) / 8 - utd[i].offset / 8);
							std::vector<uchar> bts(totalSize);
							get_bytes(bts.data(), totalSize, std::get<0>(item));
							unsigned long long num, shiftbits = 8 - ((utd[i].offset + btd.width) % 8);
							num |= (bts[0] >> shiftbits);
							for (size_t j = 1; j < bts.size(); j++) {
								num |= (((unsigned long long)bts[j]) << (j * 8 - shiftbits));
							}
							num &= ((~0ull) >> (8 - (btd.width % 8)));
							str += colorize("0x" + to_string(num, std::hex), bColor, COLOR_NUMBER);
						} else {
							str += initForType(utd[i].type, (ea_t)(std::get<0>(item) + utd[i].offset / 8), bColor);
						}
					}
				}
				str += " }";
			} else if (is_strlit(get_flags(std::get<0>(item)))) { //preempt integer and array handling
				//asize_t len = get_item_size(std::get<0>(item));
				//single byte formats: STRTYPE_TERMCHR, STRTYPE_C, STRTYPE_PASCAL, STRTYPE_LEN2, STRTYPE_LEN4
				//double byte formats: STRTYPE_C_16, STRTYPE_PASCAL_16, STRTYPE_LEN4_16
				//int len = get_max_strlit_length(addr, ti.get_size() == 1 ? STRTYPE_C : (ti.get_size() == 2 ? STRTYPE_C_16 : STRTYPE_C_32), ALOPT_IGNHEADS | ALOPT_IGNPRINT);
				qstring qs;
				get_strlit_contents(&qs, std::get<0>(item), -1, get_str_type(std::get<0>(item)), nullptr, STRCONV_ESCAPE); //u8 for UTF-8 with char and u for UTF-16 with char16_t
				if (ti.is_array()) {
					array_type_data_t atd;
					ti.get_array_details(&atd);
					str = std::string(atd.elem_type.get_size() == sizeof(wchar_t) ? "L" : (atd.elem_type.get_size() == sizeof(char32_t) ? "U" : "")) + "\"" + std::string(qs.c_str()) + "\"";
				} else str = std::string(ti.get_size() == sizeof(wchar_t) ? "L" : (ti.get_size() == sizeof(char32_t) ? "U" : "")) + "\"" + std::string(qs.c_str()) + "\"";
				str = colorize(str, bColor, COLOR_STRING);
			} else if (ti.is_array()) {
				array_type_data_t atd;
				ti.get_array_details(&atd);
				str = "{ ";
				s.push(std::tuple<ea_t, tinfo_t, int>(std::get<0>(item), ti, -1));
				for (uint32 i = atd.nelems - 1; true; i--) {
					s.push(std::tuple<ea_t, tinfo_t, int>((ea_t)(std::get<0>(item) + atd.elem_type.get_size() * i), atd.elem_type, 0));
					if (i != 0) s.push(std::tuple<ea_t, tinfo_t, int>((ea_t)std::get<0>(item), ti, -2));
					else break;
				}
			} else if (ti.is_char() || is_type_int(ti.get_realtype()) && ti.is_signed()) {
				long long num = 0;
				get_bytes(&num, ti.get_size(), std::get<0>(item));
				//heuristics for printing hex vs decimal?
				str = colorize("0x" + to_string(num, std::hex), bColor, COLOR_NUMBER);
			} else if (is_type_int(ti.get_realtype())) {
				unsigned long long num = 0;
				get_bytes(&num, ti.get_size(), std::get<0>(item));
				//heuristics for printing hex vs decimal?
				str = colorize("0x" + to_string(num, std::hex), bColor, COLOR_NUMBER);
			} else if (ti.is_bool()) {
				str = colorize(get_byte(std::get<0>(item)) != 0 ? "true" : "false", bColor, COLOR_KEYWORD);
			} else if (ti.is_floating()) {
				if (ti.is_float()) {
					float f;
					get_bytes(&f, ti.get_size(), std::get<0>(item));
					char buf[33];
					qsnprintf(buf, sizeof(buf), "%.*g", FLT_DECIMAL_DIG, f);
					str = colorize(buf, bColor, COLOR_NUMBER);
				} else if (ti.is_double()) {
					double f;
					get_bytes(&f, ti.get_size(), std::get<0>(item));
					char buf[33];
					qsnprintf(buf, sizeof(buf), "%.*lg", DBL_DECIMAL_DIG, f);
					str = colorize(buf, bColor, COLOR_NUMBER);
				} /*else if (ti.is_ldouble()) {
					long double f;
					get_bytes(&f, sz, it->first);
					char buf[33];
					qsnprintf(buf, sizeof(buf), "%.*Lg", LDBL_DECIMAL_DIG, f); LDBL_DECIMAL_DIG = 21 for 80-bit/10-byte?
				}*/ else {
					//msg("%s %s\n", ti.typeName.c_str(), ti.metaType.c_str());
				}
			} else if (ti.is_ptr()) {
				uval_t u;
				get_data_value(&u, std::get<0>(item), (asize_t)ti.get_size());
				//tinfo_t tif;
				//get_tinfo(&tif, u);
				qstring qs = get_name(u);
				str = qs.size() == 0 ? "0x" + to_string(u, std::hex) : "&" + colorize(std::string(qs.c_str()), bColor, COLOR_DEFAULT);//initForType(tif, addr);
			}
			out += str;
		}
		return out;
	}
	std::string IdaCallback::initForType(const std::vector<TypeInfo>& types, ea_t addr, bool bColor)
	{
		std::string out;
		std::stack<std::pair<ea_t, int>> s;
		s.push(std::pair<ea_t, int>(addr, 0));
		while (!s.empty()) {
			std::pair<ea_t, int> item = s.top();
			s.pop();
			if (item.second == -1) {
				out += " }";
				continue;
			} else if (item.second == -2) {
				out += ", ";
				continue;
			}
			const TypeInfo& ti = types.at(item.second);
			std::string str;
			if (ti.size == -1) { //typeref
				tinfo_t tinf;
				tinf.get_named_type(get_idati(), ti.typeName.c_str());
				std::vector<TypeInfo> nextChain;
				getType(tinf, nextChain, true);
				str = initForType(nextChain, item.first, bColor);
				//msg("%s %s\n", ti.typeName.c_str(), ti.metaType.c_str());
			} else if (ti.metaType == "struct") {
				str = "{ ";
				for (size_t i = 0; i < ti.structMembers.size(); i++) {
					if (i != 0) str += ", ";
					str += initForType(ti.structMembers[i].ti, item.first + (ea_t)ti.structMembers[i].offset, bColor);
				}
				str += " }";
			} else if (is_strlit(get_flags(item.first))) { //preempt integer and array handling
				//asize_t len = get_item_size(item.first);
				//single byte formats: STRTYPE_TERMCHR, STRTYPE_C, STRTYPE_PASCAL, STRTYPE_LEN2, STRTYPE_LEN4
				//double byte formats: STRTYPE_C_16, STRTYPE_PASCAL_16, STRTYPE_LEN4_16
				//int len = get_max_strlit_length(item.first, ti.get_size() == 1 ? STRTYPE_C : (ti.get_size() == 2 ? STRTYPE_C_16 : STRTYPE_C_32), ALOPT_IGNHEADS | ALOPT_IGNPRINT);
				qstring qs;
				get_strlit_contents(&qs, item.first, -1, get_str_type(item.first), nullptr, STRCONV_ESCAPE); //u8 for UTF-8 with char and u for UTF-16 with char16_t
				if (ti.metaType == "array") {
					//if ((ti + 1)->size == -1) //in case a chain of type refs until the size needs to be consumed which would require recursion since its the typeref case above, just divide
					str = std::string((ti.size / ti.arraySize) == sizeof(wchar_t) ? "L" : ((ti.size / ti.arraySize) == sizeof(char32_t) ? "U" : "")) + "\"" + std::string(qs.c_str()) + "\"";
				} else str = std::string(ti.size == sizeof(wchar_t) ? "L" : (ti.size == sizeof(char32_t) ? "U" : "")) + "\"" + std::string(qs.c_str()) + "\"";
				str = colorize(str, bColor, COLOR_STRING);
			} else if (ti.metaType == "array") {
				str = "{ ";
				s.push(std::pair<ea_t, int>(item.first, -1));
				for (unsigned long long i = ti.arraySize - 1; true; i--) {
					s.push(std::pair<ea_t, int>(item.first + (ea_t)types.at(item.second + 1).size * (ea_t)i, item.second + 1));
					if (i != 0) s.push(std::pair<ea_t, int>(item.first, -2));
					else break;
				}
			} else if (ti.metaType == "char" || ti.metaType == "int" || ti.metaType == "unknown") {
				long long num = 0;
				get_bytes(&num, (ssize_t)ti.size, item.first);
				//heuristics for printing hex vs decimal?
				str = colorize("0x" + to_string(num, std::hex), bColor, COLOR_NUMBER);
			} else if (ti.metaType == "uint") {
				unsigned long long num = 0;
				get_bytes(&num, (ssize_t)ti.size, item.first);
				//heuristics for printing hex vs decimal?
				str = colorize("0x" + to_string(num, std::hex), bColor, COLOR_NUMBER);
			} else if (ti.metaType == "bool") {
				str = colorize(get_byte(item.first) != 0 ? "true" : "false", bColor, COLOR_KEYWORD);
			} else if (ti.metaType == "float") {
				if (ti.size == sizeof(float)) {
					float f;
					get_bytes(&f, (ssize_t)ti.size, item.first);
					char buf[33];
					qsnprintf(buf, sizeof(buf), "%.*g", FLT_DECIMAL_DIG, f);
					str = colorize(buf, bColor, COLOR_NUMBER);
				} else if (ti.size == sizeof(double)) {
					double f;
					get_bytes(&f, (ssize_t)ti.size, item.first);
					char buf[33];
					qsnprintf(buf, sizeof(buf), "%.*lg", DBL_DECIMAL_DIG, f);
					str = colorize(buf, bColor, COLOR_NUMBER);
				} /*else if (ti.is_ldouble()) {
					long double f;
					get_bytes(&f, sz, it->first);
					char buf[33];
					qsnprintf(buf, sizeof(buf), "%.*Lg", LDBL_DECIMAL_DIG, f); LDBL_DECIMAL_DIG = 21 for 80-bit/10-byte?
				}*/ else {
					//msg("%s %s\n", ti.typeName.c_str(), ti.metaType.c_str());
				}
			} else if (ti.metaType == "ptr") { //code would have to be a function pointer
				//} else if (ti.is_ptr()) {
				uval_t u;
				get_data_value(&u, item.first, (asize_t)ti.size);
				//tinfo_t tif;
				//get_tinfo(&tif, u);
				qstring qs = get_name(u);
				str = qs.size() == 0 ? "0x" + to_string(u, std::hex) : "&" + colorize(std::string(qs.c_str()), bColor, COLOR_DEFAULT);//initForType(tif, item.first, bColor);
			}
			out += str;
		}
		return out;
	}

	std::string IdaCallback::print_func(ea_t ea, std::string& forDisplay, char NameColor)
	{
		//does model go before or after return type - seems to be compiler specific what about C language spec?
		std::string definitions = printTypeForCode(funcProtoInfos[ea].fpi.retType.pi.ti, "", false, COLOR_LOCNAME, false, 0) + (funcProtoInfos[ea].fpi.model != "default" && funcProtoInfos[ea].fpi.model != "unknown" ? " " + funcProtoInfos[ea].fpi.model : "") + " " + funcProtoInfos[ea].name + "(";
		forDisplay += printTypeForCode(funcProtoInfos[ea].fpi.retType.pi.ti, "", true, COLOR_LOCNAME, false, 0) +
			(funcProtoInfos[ea].fpi.model != "default" && funcProtoInfos[ea].fpi.model != "unknown" ? " " + std::string({ COLOR_ON, COLOR_KEYWORD }) + funcProtoInfos[ea].fpi.model + std::string({ COLOR_OFF, COLOR_KEYWORD }) : "") + " " +
			std::string({ COLOR_ON, NameColor }) + funcProtoInfos[ea].name + std::string({ COLOR_OFF, NameColor }) + "(";
		bool bFirst = true;
		for (size_t i = 0; i < funcProtoInfos[ea].fpi.syminfo.size(); i++) {
			if (funcProtoInfos[ea].fpi.syminfo[i].argIndex != -1) {
				if (!bFirst) {
					definitions += ", "; forDisplay += ", ";
				}
				bFirst = false;
				definitions += printTypeForCode(funcProtoInfos[ea].fpi.syminfo[i].pi.ti, funcProtoInfos[ea].fpi.syminfo[i].pi.name, false, COLOR_LOCNAME, false, 0);
				forDisplay += printTypeForCode(funcProtoInfos[ea].fpi.syminfo[i].pi.ti, funcProtoInfos[ea].fpi.syminfo[i].pi.name, true, COLOR_LOCNAME, false, 0);
			}
		}
		definitions += ")";
		forDisplay += ")";
		return definitions;
	}

	std::string IdaCallback::getHeaderDefFromAnalysis(bool allImports, std::string & forDisplay)
	{
		qstring qs;
		std::string definitions;
		std::vector<std::vector<ea_t>> impByMod;
		impByMod.resize(modNames.size());
		//all exported functions definitions
		if (allImports) {
			for (std::map<ea_t, ImportInfo>::iterator it = imports.begin(); it != imports.end(); it++) {
				impByMod[it->second.idx].push_back(it->first);
			}
		} else {
			//only used exported functions definitions
			for (std::map<ea_t, bool>::iterator it = usedImports.begin(); it != usedImports.end(); it++) {
				impByMod[imports[it->first].idx].push_back(it->first);
			}
		}

		//get_c_header_path, get_c_macros

		//there are no 3, 5, 6, 7 byte types so larger types are used instead
		//there is not a __int128 in C currently so __int64 must be used, or better yet a structure should be defined and emitted
		//10 and 16 byte floats are not existent on many platforms either so long double could potentially work but its a place holder, again a structure would be better
		//so both large floats and integers are truncated currently
		//other types are not guaranteed to be sizes specified so where nullptr occurs are C keyword types but perhaps the core type names should be redone from the defaults entirely

		std::map<ea_t, bool> dataDependee;
		std::map<ea_t, bool> usedRefData;
		std::map<ea_t, bool> usedRefFuncs;
		for (std::map<ea_t, std::vector<TypeInfo>>::iterator it = usedData.begin(); it != usedData.end(); it++) {
			if (finalData.find(getSymbolName(it->first).c_str()) != finalData.end()) {
				addAllDataRefs(it->first, dependentData, usedData, usedRefData, dataDependee, usedRefFuncs);
			}
		}
		for (std::map<ea_t, bool>::iterator it = usedImports.begin(); it != usedImports.end(); it++) {
			qs.clear();
			if (imports.find(it->first) != imports.end() && imports[it->first].name != "") {
				qs = demangle_name(imports[it->first].name.c_str(), MNG_SHORT_FORM);
				if (qs.size() == 0) qs = imports[it->first].name.c_str();
			}
			if (qs.size() == 0) qs = get_short_name(it->first, GN_STRICT);
			if (qs.size() != 0) qs = qs.substr(0, qs.find("(", 0));
			else qs = getSymbolName(it->first).c_str();
			if (finalFuncs.find(qs.c_str()) != finalFuncs.end()) usedRefFuncs[it->first] = true;
		}
		for (std::map<ea_t, bool>::iterator it = usedFuncs.begin(); it != usedFuncs.end(); it++) {
			qs = get_short_name(it->first, GN_STRICT);
			if (qs.size() != 0) qs = qs.substr(0, qs.find("(", 0));
			else qs = getSymbolName(it->first).c_str();
			if (finalFuncs.find(qs.c_str()) != finalFuncs.end()) usedRefFuncs[it->first] = true;
		}
		//for any single function failed decompilation with no function prototype - make sure one is printed in comments
		for (std::map<ea_t, bool>::iterator it = definedFuncs.begin(); it != definedFuncs.end(); it++) {
			if (funcProtos.find(it->first) == funcProtos.end()) {
				usedFuncs[it->first] = true;
				usedRefFuncs[it->first] = true;
			}
		}

		std::map<std::string, bool> usedRefTypes;
		//presumably this is already in reverse dependency order as the decompiler must query in this order
		//need to add forward declarations to any structures/union with circular references via their members however
		//functions are also needed only for forward declarations, including those queried 
		for (int i = 0; i < numDefCoreTypes; i++) {
			coreTypeUsed[i] = false; //reset as tracking as is done now to comment these is the most useless information as they were truly temporary
			if (finalTypes.find(defaultCoreTypes[i].name) != finalTypes.end()) coreTypeUsed[i] = true;
		}
		//core types are added in refTypes!
		for (std::map<ea_t, bool>::iterator it = usedRefFuncs.begin(); it != usedRefFuncs.end(); it++) {
			if (funcProtos.find(it->first) != funcProtos.end()) continue; //if a working function prototype exists then its types have been included
			if (funcProtoInfos.find(it->first) != funcProtoInfos.end()) {
				refTypes(funcProtoInfos[it->first].fpi.retType.pi.ti, usedRefTypes);
				for (size_t i = 0; i < funcProtoInfos[it->first].fpi.syminfo.size(); i++) {
					refTypes(funcProtoInfos[it->first].fpi.syminfo[i].pi.ti, usedRefTypes);
				}
			} //this case no longer can occur
			//get types of return and argument values and add ref to them
		}
		for (std::map<ea_t, bool>::iterator it = usedRefData.begin(); it != usedRefData.end(); it++) {
			refTypes(usedData[it->first], usedRefTypes);
		}

		std::map<std::string, bool> isDependee;
		std::for_each(usedTypes.begin(), usedTypes.end(), [&isDependee, &usedRefTypes, this](std::pair<std::string, bool> it) { buildDependents(it.first, usedRefTypes, isDependee); });
		isDependee.clear(); //more usedTypes can be added here but it is not a problem as the real reference ones previously search and later filter applied
		std::for_each(usedTypes.begin(), usedTypes.end(), [&isDependee, this](std::pair<std::string, bool> it) { buildDependents(it.first, usedTypes, isDependee); });
		std::for_each(usedTypes.begin(), usedTypes.end(), [](std::pair<const std::string, bool>& it) { it.second = true; }); //transform cannot mutate map but second can change with reference and const first
		std::map<std::string, bool> alreadyDefined; //for forward declarations
		std::vector<std::string> sortedTypes; //in dependency order
		//because it is cyclic must start with ones not depended upon dependents first to capture maximal dependencies
		for (std::map<std::string, bool>::iterator it = usedTypes.begin(); it != usedTypes.end(); it++) {
			//if (it->second.size() == 0)
			if (isDependee.find(it->first) == isDependee.end())
				dfsTypeVisit(it->first, usedTypes, sortedTypes);
				//dfsVisit(it->first, sortedTypes);
		}
		//as for the cliques theoretically a minimal forward declaration analysis would reduce them
		for (std::map<std::string, bool>::iterator it = usedTypes.begin(); it != usedTypes.end(); it++) {
			//if (it->second.size() != 0)
			if (isDependee.find(it->first) != isDependee.end())
				dfsTypeVisit(it->first, usedTypes, sortedTypes);
				//dfsVisit(it->first, sortedTypes);
		}

		//pointer initizations can lead to a dependency ordering here, otherwise address ordering is ideal perhaps
		//functions can be pointed to as well making their declarations need to go prior to the data
		std::vector<ea_t> sortedData; //in dependency order
		std::map<ea_t, bool> alreadyDefData; //for forward declarations
		//because it is cyclic must start with ones not depended upon dependents first to capture maximal dependencies
		std::map<ea_t, bool> visited;
		for (std::map<ea_t, std::vector<TypeInfo>>::iterator it = usedData.begin(); it != usedData.end(); it++) {
			//if (it->second.size() == 0)
			if (dataDependee.find(it->first) == dataDependee.end())
				dfsVisitData(it->first, dependentData, usedData, sortedData, visited);
		}
		//as for the cliques theoretically a minimal forward declaration analysis would reduce them
		for (std::map<ea_t, std::vector<TypeInfo>>::iterator it = usedData.begin(); it != usedData.end(); it++) {
			//if (it->second.size() != 0)
			if (dataDependee.find(it->first) != dataDependee.end())
				dfsVisitData(it->first, dependentData, usedData, sortedData, visited);
		}

		std::string str = "//Default calling convention set to: " + (di->customCallStyle.empty() ? ccToStr(inf_cc_cm, is_code_far(inf_cc_cm) ? FTI_FARCALL : FTI_NEARCALL, true) : di->customCallStyle) + "\n\n";
		definitions += str; //should really use callback interface to get cspec variant
		forDisplay += std::string({ COLOR_ON, COLOR_AUTOCMT }) + str + std::string({ COLOR_OFF, COLOR_AUTOCMT });
		for (int i = 0; i < numDefCoreTypes; i++) {
			if (usedRefTypes.find(defaultCoreTypes[i].name) != usedRefTypes.end()) {
				coreTypeUsed[i] = true;
				usedRefTypes.erase(defaultCoreTypes[i].name);
			}
			if (coreTypeUsed[i] && szCoreTypeDefs[i] != nullptr) {
				definitions += "typedef " + std::string(szCoreTypeDefs[i]) + " " + defaultCoreTypes[i].name + ";\n";
				forDisplay += std::string({ COLOR_ON, COLOR_KEYWORD }) + "typedef" + std::string({ COLOR_OFF, COLOR_KEYWORD }) + " " +
					std::string({ COLOR_ON, COLOR_LOCNAME }) + std::string(szCoreTypeDefs[i]) + std::string({ COLOR_OFF, COLOR_LOCNAME }) +
					" " +
					std::string({ COLOR_ON, COLOR_LOCNAME }) + defaultCoreTypes[i].name + std::string({ COLOR_OFF, COLOR_LOCNAME }) +
					";\n";
			}
		}
		//print_decls(); //could print all suitable for header file
		for (std::vector<std::string>::iterator it = sortedTypes.begin(); it != sortedTypes.end(); it++) {
			tinfo_t ti;
			ti.get_named_type(get_idati(), it->c_str());
			qstring qt = it->c_str();
			//ti.get_type_name(&qt);
			qstring qs;
			alreadyDefined[*it] = true;
			if (is_anonymous_udt(ti)) continue;
			if (ti.is_udt()) {
				std::map<std::string, bool> notDefined;
				checkForwardDecl(ti, alreadyDefined, notDefined);
				for (std::map<std::string, bool>::iterator iter = notDefined.begin(); iter != notDefined.end(); iter++) {
					tinfo_t t;
					t.get_named_type(get_idati(), iter->first.c_str());
					//t.print(&qs, NULL, PRTYPE_1LINE | PRTYPE_TYPE);
					if (usedRefTypes.find(*it) == usedRefTypes.end()) {
						definitions += "//"; forDisplay += std::string({ COLOR_ON, COLOR_AUTOCMT }) + "//";
					}
					definitions += std::string(t.is_union() ? "union " : "struct ") + iter->first + ";\n";
					forDisplay += std::string({ COLOR_ON, COLOR_KEYWORD }) + std::string(t.is_union() ? "union" : "struct") + std::string({ COLOR_OFF, COLOR_KEYWORD }) +
						" " + std::string({ COLOR_ON, COLOR_LOCNAME }) + iter->first + std::string({ COLOR_OFF, COLOR_LOCNAME }) + ";\n";
					if (usedRefTypes.find(*it) == usedRefTypes.end()) forDisplay += std::string({ COLOR_OFF, COLOR_AUTOCMT });
					alreadyDefined[iter->first] = true;
				}
			} else if (ti.is_func()) { //ti.is_funcptr()
				//function definitions need the entire definition printed but should be tracked separately
			}
			if (ti.is_forward_decl()) {
				if (usedRefTypes.find(*it) == usedRefTypes.end()) {
					definitions += "//"; forDisplay += std::string({ COLOR_ON, COLOR_AUTOCMT }) + "//";
				}
				definitions += std::string(ti.is_decl_union() ? "union " : "struct ") + " " + std::string(qt.c_str()) + ";\n";
				forDisplay += std::string({ COLOR_ON, COLOR_KEYWORD }) + std::string(ti.is_decl_union() ? "union" : "struct") + std::string({ COLOR_OFF, COLOR_KEYWORD }) +
					" " + std::string({ COLOR_ON, COLOR_LOCNAME }) + std::string(qt.c_str()) + std::string({ COLOR_OFF, COLOR_LOCNAME }) + ";\n";
				if (usedRefTypes.find(*it) == usedRefTypes.end()) forDisplay += std::string({ COLOR_OFF, COLOR_AUTOCMT });
			} else {
				//ti.print(&qs, nullptr, (ti.is_udt() ? PRTYPE_MULTI : PRTYPE_1LINE) | PRTYPE_DEF | PRTYPE_TYPE | PRTYPE_RESTORE);				
				if (usedRefTypes.find(*it) == usedRefTypes.end()) {
					definitions += "/*"; forDisplay += std::string({ COLOR_ON, COLOR_AUTOCMT }) + "/*";
				}
				//definitions += std::string((ti.is_udt() ? qs.rtrim('\n') : qs).c_str()) + " " + std::string(qt.c_str()) + ";";
				std::vector<TypeInfo> typeChain = typeDatabase[it->c_str()];
				//wchar_t is special exception with no entries since its also a core type but nice to have the comment regarding its IDA detected C integral definition
				if (typeChain.size() == 0) {
					definitions += "typedef " + printTypeForCode(ti, "", false, COLOR_LOCNAME, true, 0) + " " + std::string(qt.c_str()) + ";";
					//str = std::string((ti.is_udt() ? qs.rtrim('\n') : qs).c_str()) + " " + std::string(qt.c_str()) + ";";
					str = std::string({ COLOR_ON, COLOR_KEYWORD }) + "typedef" + std::string({ COLOR_OFF, COLOR_KEYWORD }) +
						" " + printTypeForCode(ti, "", true, COLOR_LOCNAME, true, 0) + " " +
						std::string({ COLOR_ON, COLOR_LOCNAME }) + std::string(qt.c_str()) + std::string({ COLOR_OFF, COLOR_LOCNAME }) + ";";
				} else {
					definitions += "typedef " + printTypeForCode(typeChain, "", false, COLOR_LOCNAME, typeChain.begin()->typeName == qt.c_str(), 0) + " " + std::string(qt.c_str()) + ";";
					//str = std::string((ti.is_udt() ? qs.rtrim('\n') : qs).c_str()) + " " + std::string(qt.c_str()) + ";";
					str = std::string({ COLOR_ON, COLOR_KEYWORD }) + "typedef" + std::string({ COLOR_OFF, COLOR_KEYWORD }) +
						" " + printTypeForCode(typeChain, "", true, COLOR_LOCNAME, typeChain.begin()->typeName == qt.c_str(), 0) + " " +
						std::string({ COLOR_ON, COLOR_LOCNAME }) + std::string(qt.c_str()) + std::string({ COLOR_OFF, COLOR_LOCNAME }) + ";";
				}
				if (usedRefTypes.find(*it) == usedRefTypes.end()) {
					std::string finalstr;
					std::for_each(str.begin(), str.end(), [&finalstr](char c) { if (c == '\n') finalstr += std::string({ COLOR_OFF, COLOR_AUTOCMT }) + c + std::string({ COLOR_ON, COLOR_AUTOCMT }); else finalstr += c; });
					forDisplay += std::string({ COLOR_ON, COLOR_AUTOCMT }) + finalstr + std::string({ COLOR_OFF, COLOR_AUTOCMT });
					definitions += "*/"; forDisplay += "*/" + std::string({ COLOR_OFF, COLOR_AUTOCMT });
				} else forDisplay += str;
				definitions += "\n";
				forDisplay += "\n";
			}
			/*if (ti.is_struct()) {
			} else if (ti.is_array()) {
			//} else if (ti.is_enum()) {
			} else if (ti.is_typeref()) {
				qstring qs;
				if (!ti.get_next_type_name(&qs)) {
					int typ = coreTypeLookup(ti.get_size(), getMetaTypeInfo(ti));
					qs = typ == -1 ? "undefined" : defaultCoreTypes[typ].name.c_str();
				}
				definitions += "typedef " + std::string(qs.c_str()) + " " + std::string(qt.c_str()) + ";\n";
			}
			else { //?
			}*/
		}
		if (definitions.size() != 0) {
			definitions += "\n"; forDisplay += "\n";
		}
		//these are sorted by library and have a comment to remind that libraries are link time imported
		for (size_t i = 0; i < modNames.size(); i++) {
			if (impByMod[i].size() != 0) {
				definitions += "//" + modNames[i] + '\n';
				forDisplay += std::string({ COLOR_ON, COLOR_AUTOCMT }) + "//" + modNames[i] + std::string({ COLOR_OFF, COLOR_AUTOCMT }) + '\n';
			}
			for (std::vector<ea_t>::iterator it = impByMod[i].begin(); it != impByMod[i].end(); it++) {
				tinfo_t tif;
				qstring qs;
				if (usedRefFuncs.find(*it) == usedRefFuncs.end()) {
					definitions += "//"; forDisplay += std::string({ COLOR_ON, COLOR_AUTOCMT }) + "//";
				}
				definitions += "extern ";
				forDisplay += std::string({ COLOR_ON, COLOR_KEYWORD }) + "extern" + std::string({ COLOR_OFF, COLOR_KEYWORD }) + " ";
				if (getFuncByGuess(*it, tif) && funcProtoInfos.find(*it) != funcProtoInfos.end()) {
					//print_type(&qs, *it, PRTYPE_1LINE);
					//func_type_data_t ftd;
					//tif.get_func_details(&ftd);
					definitions += print_func(*it, forDisplay, COLOR_IMPNAME);
				} else {
					qs = demangle_name(imports[*it].name.c_str(), MNG_LONG_FORM).c_str();
					if (qs.size() == 0) qs = imports[*it].name.c_str();
					if (qs.size() == 0) get_long_name(&qs, *it, GN_STRICT);
					if (qs.size() == 0) qs = getSymbolName(*it).c_str();
					//str = get_name(*it).c_str();				
					if (qs.find("(", 0) == qstring::npos) {
						definitions += "void" " " + std::string(qs.c_str()) + "(" "void" ")";
						forDisplay += std::string({ COLOR_ON, COLOR_LOCNAME }) + "void" + std::string({ COLOR_OFF, COLOR_LOCNAME }) + " " + 
							std::string({ COLOR_ON, COLOR_IMPNAME }) + std::string(qs.c_str()) + std::string({ COLOR_OFF, COLOR_IMPNAME }) + "(" + std::string({ COLOR_ON, COLOR_LOCNAME }) + "void" + std::string({ COLOR_OFF, COLOR_LOCNAME }) + ")";
					} else {
						definitions += qs.c_str();
						forDisplay += std::string({ COLOR_ON, COLOR_IMPNAME }) + qs.c_str() + std::string({ COLOR_OFF, COLOR_IMPNAME });
					}
					//most of the names are not mangled, and thus do not have the needed C declaration information attached
				}
				definitions += ";\n";
				forDisplay += ";\n";
				if (usedRefFuncs.find(*it) == usedRefFuncs.end()) forDisplay += std::string({ COLOR_OFF, COLOR_AUTOCMT });
			}
			if (impByMod[i].size() != 0) {
				definitions += '\n'; forDisplay += '\n';
			}
		}
		for (std::map<ea_t, bool>::iterator it = usedFuncs.begin(); it != usedFuncs.end(); it++) {
			if (imports.find(it->first) != imports.end()) continue;
			if (usedRefFuncs.find(it->first) == usedRefFuncs.end()) {
				definitions += "//"; forDisplay += std::string({ COLOR_ON, COLOR_AUTOCMT }) + "//";
			}
			if (funcProtos.find(it->first) != funcProtos.end()) {
				definitions += funcProtos[it->first];
				forDisplay += funcColorProtos[it->first];
			} else if (funcProtoInfos.find(it->first) != funcProtoInfos.end()) {
				definitions += print_func(it->first, forDisplay, COLOR_DEFAULT);
			} else { //only function pointers currently but after fixed this case should never occur
				tinfo_t tif;
				qs.clear(); //first check decompiler prototypes, then type info, then demangler, then void <name>(void)
				if (getFuncByGuess(it->first, tif)) print_type(&qs, it->first, PRTYPE_1LINE);
				if (qs.size() == 0) get_long_name(&qs, it->first, GN_STRICT);
				if (qs.size() == 0) qs = getSymbolName(it->first).c_str();
				if (qs.find("(", 0) == qstring::npos) qs = qstring("void ") + qs + "(void)";
				//most of the names are not mangled, and thus do not have the needed C declaration information attached
				definitions += qs.c_str();
				forDisplay += std::string({ COLOR_ON, COLOR_DEFAULT }) + qs.c_str() + std::string({ COLOR_OFF, COLOR_DEFAULT });
			}
			definitions += ";\n";
			forDisplay += ";\n";
			if (usedRefFuncs.find(it->first) == usedRefFuncs.end()) forDisplay += std::string({ COLOR_OFF, COLOR_AUTOCMT });
		}
		if (usedFuncs.size() != 0) {
			definitions += "\n"; forDisplay += "\n";
		}
		for (std::vector<ea_t>::iterator it = sortedData.begin(); it != sortedData.end(); it++) {
			tinfo_t ti;
			//segment_t* seg = getseg(*it);
			//msg("EA: %llX Flags: %lX\n", *it, get_full_flags(*it));
			bool bInitData = is_loaded(*it); //has_value(get_full_flags(*it)) && getseg(*it)->type != SEG_BSS; //seg->type == SEG_DATA; //not totally reliable
			//getseg(*it)->type != SEG_BSS
			//std::string dat;
			alreadyDefData[*it] = true;
			std::map<ea_t, bool> notDefined;
			checkFwdData(*it, dependentData, alreadyDefData, notDefined);
			for (std::map<ea_t, bool>::iterator iter = notDefined.begin(); iter != notDefined.end(); iter++) {
				if (usedRefData.find(*it) == usedRefData.end()) {
					definitions += "//"; forDisplay += std::string({ COLOR_ON, COLOR_AUTOCMT }) + "//";
				}
				definitions += "extern " + printTypeForCode(usedData[iter->first], std::string(get_name(iter->first).c_str()), false, COLOR_DEFAULT, false, 0) + ";\n";
				forDisplay += std::string({ COLOR_ON, COLOR_KEYWORD }) + "extern" + std::string({ COLOR_OFF, COLOR_KEYWORD }) + " " + printTypeForCode(usedData[iter->first], std::string(get_name(iter->first).c_str()), true, COLOR_DEFAULT, false, 0) + ";\n";
				if (usedRefData.find(*it) == usedRefData.end()) forDisplay += std::string({ COLOR_OFF, COLOR_AUTOCMT });
				alreadyDefData[iter->first] = true;
			}
			if (usedRefData.find(*it) == usedRefData.end()) {
				definitions += "//"; forDisplay += std::string({ COLOR_ON, COLOR_AUTOCMT }) + "//";
			}
			str = getSymbolName(*it);
			definitions += printTypeForCode(usedData[*it], str.c_str(), false, COLOR_DEFAULT, false, 0);
			forDisplay += printTypeForCode(usedData[*it], str.c_str(), true, COLOR_DEFAULT, false, 0);
			//if (get_tinfo(&ti, *it)) {
				/*qstring qt;
				ti.get_type_name(&qt);
				if (qt == "" && ti.is_array()) {
					array_type_data_t atd;
					ti.get_array_details(&atd);
					atd.elem_type.get_type_name(&qt);
				}
				definitions += std::string(qt.c_str()) + " " + std::string(ti.is_ptr() ? "*" : "") + get_name(*it).c_str() + std::string(ti.is_array() ? "[]" : "");*/
				//if (bInitData) dat = initForType(ti, *it);
			//} else {
				/*asize_t sz = get_item_size(*it);
				int typ = coreTypeLookup(sz, "unknown");
				if (typ == -1 && sz != 1) {
					//definitions += "undefined " + std::string(get_name(*it).c_str()) + "[" + std::to_string(sz) + "]";
					if (bInitData) {
						if (is_strlit(get_flags(*it))) { //preempt array, undefined, unknown, char, etc
							//asize_t len = get_item_size(*it);
							//single byte formats: STRTYPE_TERMCHR, STRTYPE_C, STRTYPE_PASCAL, STRTYPE_LEN2, STRTYPE_LEN4
							//double byte formats: STRTYPE_C_16, STRTYPE_PASCAL_16, STRTYPE_LEN4_16
							//int len = get_max_strlit_length(addr, ti.get_size() == 1 ? STRTYPE_C : (ti.get_size() == 2 ? STRTYPE_C_16 : STRTYPE_C_32), ALOPT_IGNHEADS | ALOPT_IGNPRINT);
							qstring qs;
							get_strlit_contents(&qs, *it, -1, get_str_type(*it), nullptr, STRCONV_ESCAPE);
							dat = "\"" + std::string(qs.c_str()) + "\"";
						} else {
							dat = "{ ";
							std::vector<char> arr;
							arr.resize(sz);
							get_bytes(arr.data(), sz, *it);
							for (int i = 0; i < sz; i++) {
								if (i != 0) dat += ", ";
								dat += "0x" + to_string(arr[i], std::hex);
							}
							dat += " }";
						}
					}
				} else {
					//definitions += std::string(typ == -1 || is_strlit(get_flags(*it)) ? "undefined" : defaultCoreTypes[typ].name) + " " + get_name(*it).c_str();
					if (bInitData) {
						if (is_strlit(get_flags(*it))) { //preempt array, undefined, unknown, char, etc
							//asize_t len = get_item_size(*it);
							//single byte formats: STRTYPE_TERMCHR, STRTYPE_C, STRTYPE_PASCAL, STRTYPE_LEN2, STRTYPE_LEN4
							//double byte formats: STRTYPE_C_16, STRTYPE_PASCAL_16, STRTYPE_LEN4_16
							//int len = get_max_strlit_length(addr, ti.get_size() == 1 ? STRTYPE_C : (ti.get_size() == 2 ? STRTYPE_C_16 : STRTYPE_C_32), ALOPT_IGNHEADS | ALOPT_IGNPRINT);
							qstring qs;
							get_strlit_contents(&qs, *it, -1, get_str_type(*it), nullptr, STRCONV_ESCAPE);
							dat = "\"" + std::string(qs.c_str()) + "\"";
						} else if (typ == -1 || defaultCoreTypes[typ].metaType == "int" || defaultCoreTypes[typ].metaType == "unknown") { //endianness matters here for 0 pad
							long long num = 0;
							get_bytes(&num, sz, *it);
							//heuristics for printing hex vs decimal?
							dat = "0x" + to_string(num, std::hex);
						} else if (defaultCoreTypes[typ].metaType == "uint") { //endianness matters here for 0 pad
							unsigned long long num = 0;
							get_bytes(&num, sz, *it);
							//heuristics for printing hex vs decimal?
							dat = "0x" + to_string(num, std::hex);
						}
						else if (defaultCoreTypes[typ].metaType == "bool") {
							dat = get_byte(*it) != 0 ? "true" : "false";
						} else if (defaultCoreTypes[typ].metaType == "float") {
							if (sz == sizeof(float)) {
								float f;
								get_bytes(&f, sz, *it);
								char buf[33];
								qsnprintf(buf, sizeof(buf), "%.*g", FLT_DECIMAL_DIG, f);
							}
							else if (sz == sizeof(double)) {
								double f;
								get_bytes(&f, sz, *it);
								char buf[33];
								qsnprintf(buf, sizeof(buf), "%.*lg", DBL_DECIMAL_DIG, f);
							}*/ /*else if (sz == sizeof(long double)) {
								long double f;
								get_bytes(&f, sz, *it);
								char buf[33];
								qsnprintf(buf, sizeof(buf), "%.*Lg", LDBL_DECIMAL_DIG, f); LDBL_DECIMAL_DIG = 21 for 80-bit/10-byte?
							}*/
							/*}
						}
					}*/
					//}
			if (bInitData) {
				definitions += " = " + initForType(usedData[*it], *it, false);
				forDisplay += " = " + initForType(usedData[*it], *it, true);
			}
			definitions += ";\n";
			forDisplay += ";\n";
			if (usedRefData.find(*it) == usedRefData.end()) forDisplay += std::string({ COLOR_OFF, COLOR_AUTOCMT });
		}
		if (usedData.size() != 0) {
			definitions += "\n";
			forDisplay += "\n";
		}

		//definitions += '\n';
		return definitions;
	}
	void IdaCallback::analysisDump(std::string& definitions, std::string& forDisplay, std::string& idaInfo)
	{
		executeOnMainThread([this, &definitions, &forDisplay, &idaInfo]() {
			definitions = getHeaderDefFromAnalysis(di->decompiledFunction == nullptr, forDisplay);
			if (PRINT_DEBUG) idaInfo = dumpIdaInfo();
			});
	}
	/// Execute code in the main thread - to be used with execute_sync().
	struct execFunctor : public exec_request_t
	{
		std::function<void()> fun;
		qsemaphore_t finishSem;
		int* taskNo;
		execFunctor(std::function<void()> f, qsemaphore_t qs, int* tsk) : fun(f), finishSem(qs), taskNo(tsk) {}
		virtual ~execFunctor(void) override {}
		/// Callback to be executed.
		/// If this function raises an exception, execute_sync() never returns.
		virtual int idaapi execute(void)
		{
			fun();
			*taskNo = -1;
			qsem_post(finishSem);
			return 0;
		}
	};
	void IdaCallback::executeOnMainThread(std::function<void()> fun)
	{
		//qsemaphore_t finishSem = qsem_create(nullptr, 0);
		execFunctor* ef = new execFunctor(fun, di->termSem, &di->uiExecutingTask);
		//int id = execute_sync(a, MFF_READ);
		{ //must be a locked unit otherwise race condition can occur
			qmutex_locker_t lock(di->qm);
			if (di->exiting) return;
			di->uiExecutingTask = execute_sync(*ef, MFF_NOWAIT);// | MFF_READ);
		}
		//int idx = -1;
		//qhandle_t handles[2] = { finishSem, di->termSem };
		//while (qwait_for_handles(&idx, handles, 2, 0, -1), idx == -1) {
		/*while (!qsem_wait(finishSem, 50)) {
			if (di->exiting) {
				di->uiExecutingTask = -1;
				qsem_free(finishSem);
				return;
			}
		}*/
		qsem_wait(di->termSem, -1);
		if (di->uiExecutingTask == -1) delete ef;
		//qsem_free(finishSem);
		//if (idx == 0) delete ef;
	}
	void IdaCallback::init(std::string s, std::string p, std::string c)
	{
		sleighfilename = s;
		pspec = p;
		cspec = c;
		cacheCount = 0;
		byteCache.clear();
		allFuncs.clear();
		allFuncNames.clear();
		sregRanges.clear();
		typeDatabase.clear();
		exportData.clear();
		modNames.clear();
		imports.clear();
		importNames.clear();
		typeToAddress.clear();
		usedImports.clear();
		usedTypes.clear();
		usedData.clear();
		definedFuncs.clear();
		usedFuncs.clear();
		funcProtos.clear();
		funcColorProtos.clear();
		funcProtoInfos.clear();
		finalData.clear();
		finalTypes.clear();
		finalFuncs.clear();
		dependentData.clear();
		coreTypeUsed.clear();
		coreTypeUsed.resize(numDefCoreTypes, false);
		size_t num = get_import_module_qty();
		qstring qs;
		ImportParam imp;
		imp.pImports = &imports;
		//validate_idb_names2(true);
		for (size_t i = 0; i < num; i++) {
			get_import_module_name(&qs, (int)i);
			modNames.push_back(qs.c_str());
			imp.cur = (int)i;
			//causing Ida Pro to error with internal error 835, 836 - because not in main thread
			enum_import_names((int)i, &EnumImportNames, &imp);
		}
		for (std::map<ea_t, ImportInfo>::iterator it = imports.begin(); it != imports.end(); it++) {
			if (it->second.name.size() == 0) {
				std::string name = get_short_name(it->first, GN_STRICT).c_str();
				if (name.size() != 0) name = name.substr(0, name.find("(", 0));
				else name = getSymbolName(it->first);
				importNames[name] = true;
			} else importNames[it->second.name] = true;
		}
		/*num = get_ordinal_qty(nullptr);
		for (size_t i = 1; i <= num; i++) {
			const type_t* t = nullptr;
			const p_list* pf = nullptr, *pfc = nullptr;
			const char* nm = get_numbered_type_name(nullptr, i);
			sclass_t s;
			msg("%s\n", nm);
			get_numbered_type(nullptr, i, &t, &pf, nullptr, &pfc, &s);
			qtype typeForDeser;
			typeForDeser.append(t);
			qtype pfForDeser;
			pfForDeser.append(pf);
			qtype pfcForDeser;
			pfcForDeser.append(pfc);
			tinfo_t ti;
			ti.deserialize(nullptr, &typeForDeser, &pfForDeser, &pfcForDeser); //will free buffer passed!
			qstring qs;
			ti.get_numbered_type(nullptr, i, BTF_TYPEDEF, false);
			ti.get_type_name(&qs);
			ti.get_next_type_name(&qs);
			ti.get_final_type_name(&qs);
			ti.print(&qs);
			//replace_ordinal_typerefs(nullptr, &ti);
		}*/
		ea_t ea = inf_min_ea;
		do {
			ea = next_head(ea, inf_max_ea);
			if (ea == BADADDR) break;
			tinfo_t ti;
			if (get_tinfo(&ti, ea) && ti.get_type_name(&qs)) {
				typeToAddress[qs.c_str()].push_back(ea);
			}
		} while (true);

		const char* nm = first_named_type(nullptr, NTF_TYPE);
		while (nm != nullptr) {
			tinfo_t ti;
			ti.get_named_type(nullptr, nm);
			if (!is_anonymous_udt(ti)) getType(ti, typeDatabase[nm]);
			nm = next_named_type(nullptr, nm, NTF_TYPE);
		}
		/*num = get_idati()->nbases;
		for (int i = 0; i < num; i++) {
			nm = first_named_type(get_idati()->base[i], NTF_TYPE);
			while (nm != nullptr) {
				tinfo_t ti;
				ti.get_named_type(nullptr, nm);
				if (ti.is_typeref()) { //function parameters are not in the database
					getType(ti, typeDatabase[nm]);
				}
				nm = next_named_type(get_idati()->base[i], nm, NTF_TYPE);
			}
		}*/
		if (ph.has_segregs()) { //initialize segment registers for tracking - vital for x86-16
			for (int i = 0; i <= ph.reg_last_sreg - ph.reg_first_sreg; i++) {
				//this is a much more powerful analysis by the processor engine than the default segregs which now are not used
				sregRanges.push_back(std::vector<sreg_range_t>());
				for (size_t k = 0; k < get_sreg_ranges_qty(i + ph.reg_first_sreg); k++) {
					sreg_range_t out;
					getn_sreg_range(&out, i + ph.reg_first_sreg, (int)k);
					sregRanges[i].push_back(out);
				}
			}
		}
		if (di->decompiledFunction == nullptr) {
			std::map<ea_t, size_t> entries;
			num = get_entry_qty();
			int successes = 0, total = 0;
			for (size_t i = 0; i < num; i++) {
				std::string disp;
				ea_t ea = get_entry(get_entry_ordinal(i)); //qstring qs; get_entry_name(&qs, i);
				if (is_func(get_flags(ea))) {
					entries[ea] = i;
					allFuncs.push_back(ea);
					qstring qs;
					get_entry_name(&qs, get_entry_ordinal(i)); //get_func_name(&qs, ea);
					allFuncNames[ea] = qs.c_str();
				} else { //exported data
					exportData[ea] = true;
				}
			}
			num = get_func_qty();
			for (size_t i = 0; i < num; i++) {
				std::string disp;
				func_t* f = getn_func(i);
				if (entries.find(f->start_ea) != entries.end()) continue;
				if (di->idacb->imports.find(f->start_ea) != di->idacb->imports.end()) continue;
				allFuncs.push_back(f->start_ea);
				qstring qs;
				get_func_name(&qs, f->start_ea);
				allFuncNames[f->start_ea] = qs.c_str();
			}
		} else {
			allFuncNames[di->decompiledFunction->start_ea] = get_name(di->decompiledFunction->start_ea).c_str();
		}
	}
	void IdaCallback::addrToArgLoc(SizedAddrInfo addr, argloc_t & al)
	{
		if (addr.addr.space == "register") {
			std::string reg = decInt->getRegisterFromIndex(addr.addr.offset, (int)addr.size);
			reg_info_t ri;
			parse_reg_name(&ri, reg.c_str());
			al.set_reg1(ri.reg);
		} else if (addr.addr.space == "ram") {
			al.set_ea((ea_t)addr.addr.offset);
		} else if (addr.addr.space == "stack") {
			al.set_stkoff((sval_t)addr.addr.offset);
		} else if (addr.addr.space == "join") {
			if (addr.addr.joins.size() == 2 &&
				addr.addr.joins[0].addr.space == "register" &&
				addr.addr.joins[1].addr.space == "register") {
				std::string reg1 = decInt->getRegisterFromIndex(addr.addr.joins[0].addr.offset, (int)addr.addr.joins[0].size);
				std::string reg2 = decInt->getRegisterFromIndex(addr.addr.joins[1].addr.offset, (int)addr.addr.joins[1].size);
				reg_info_t ri1, ri2;
				parse_reg_name(&ri1, reg1.c_str());
				parse_reg_name(&ri2, reg1.c_str());
				al.set_reg2(ri2.reg, ri1.reg);
			} else {
				scattered_aloc_t sa;
				for (size_t j = 0; j < addr.addr.joins.size(); j++) {
					argpart_t ap;
					if (addr.addr.joins[j].addr.space == "register") {
						std::string reg = decInt->getRegisterFromIndex(addr.addr.joins[j].addr.offset, (int)addr.addr.joins[j].size);
						reg_info_t ri;
						parse_reg_name(&ri, reg.c_str());
						ap.set_reg1(ri.reg);
					} else if (addr.addr.joins[j].addr.space == "ram") {
						ap.set_ea((ea_t)addr.addr.joins[j].addr.offset);
					} else if (addr.addr.joins[j].addr.space == "stack") {
						ap.set_stkoff((sval_t)addr.addr.joins[j].addr.offset);
					}
					sa.push_back(ap);
				}
				al.consume_scattered(&sa);
			}
		}
	}
	void getPtrSizes(tinfo_t& ti, std::vector<unsigned long long>& ptrSizes, std::vector<unsigned long long>& funcPtrSizes)
	{
		if (ti.is_typeref()) {
			qstring qs;
			ti.get_next_type_name(&qs);
			if (qs.size() != 0) {
				ti.get_named_type(get_idati(), qs.c_str());
				getPtrSizes(ti, ptrSizes, funcPtrSizes);
			}
		} else if (ti.is_funcptr()) { //code pointer not data
			funcPtrSizes.push_back(ti.get_size());
		} else if (ti.is_ptr()) { //cannot trace into pointers without fixing infinite recursion due to structure members
			ptrSizes.push_back(ti.get_size());
		} else if (ti.is_struct()) {
			udt_type_data_t utd;
			ti.get_udt_details(&utd);
			for (size_t i = 0; i < utd.size(); i++) {
				getPtrSizes(utd[i].type, ptrSizes, funcPtrSizes);
			}
		} else if (ti.is_array()) {
			array_type_data_t atd;
			ti.get_array_details(&atd);
			getPtrSizes(atd.elem_type, ptrSizes, funcPtrSizes);
		}
	}
	void fixPtrSizes(tinfo_t& ti, unsigned long long nearsize, unsigned long long farsize,
		bool isNear, bool isFar, bool isCodeNear, bool isCodeFar)
	{
		if (ti.is_typeref()) {
			qstring qs;
			ti.get_next_type_name(&qs);
			if (qs.size() != 0) {
				ti.get_named_type(get_idati(), qs.c_str());
				fixPtrSizes(ti, nearsize, farsize, isNear, isFar, isCodeNear, isCodeFar);
			}
		} else if (ti.is_funcptr()) { //code pointer not data
			//type_t bt2 = (ftd[i].type.is_const() ? BTM_CONST : 0) | (ftd[i].type.is_volatile() ? BTM_VOLATILE : 0);
			ptr_type_data_t ptd;
			ti.get_ptr_details(&ptd); //what to do about BTMT_CLOSURE?
			if (ptd.based_ptr_size == farsize && isCodeNear) {
				ptd.based_ptr_size -= ph.segreg_size;
				tinfo_t newti;
				newti.create_ptr(ptd, BT_PTR | BTMT_NEAR);
				ti = newti;
			} else if (ptd.based_ptr_size == nearsize && isCodeFar) {
				ptd.based_ptr_size += ph.segreg_size;
				tinfo_t newti;
				newti.create_ptr(ptd, BT_PTR | BTMT_FAR);
				ti = newti;
			}
		} else if (ti.is_ptr()) { //cannot trace into pointers without fixing infinite recursion due to structure members
			ptr_type_data_t ptd;
			ti.get_ptr_details(&ptd);
			if (ptd.based_ptr_size == farsize && isNear) {
				ptd.based_ptr_size -= ph.segreg_size;
				tinfo_t newti;
				newti.create_ptr(ptd, BT_PTR | BTMT_NEAR);
				ti = newti;
			} else if (ptd.based_ptr_size == nearsize && isFar) {
				ptd.based_ptr_size += ph.segreg_size;
				tinfo_t newti;
				newti.create_ptr(ptd, BT_PTR | BTMT_FAR);
				ti = newti;
			}
		} else if (ti.is_struct()) {
			udt_type_data_t utd;
			ti.get_udt_details(&utd);
			for (size_t i = 0; i < utd.size(); i++) {
				fixPtrSizes(utd[i].type, nearsize, farsize, isNear, isFar, isCodeNear, isCodeFar);
			}
		} else if (ti.is_array()) {
			array_type_data_t atd;
			ti.get_array_details(&atd);
			fixPtrSizes(atd.elem_type, nearsize, farsize, isNear, isFar, isCodeNear, isCodeFar);
		}
	}
	bool tryFixFuncModel(func_type_data_t& ftd, ea_t argSize) //argSize -1 for not trying to toggle pointer sizes
	{
		bool bFrameChange = false;
		std::vector<unsigned long long> ptrSizes, funcPtrSizes;
		//cannot include return pointer or will throw off comparison - need separate check but how?
		//getPtrSizes(ftd.rettype, ptrSizes, funcPtrSizes);
		for (size_t i = 0; i < ftd.size(); i++) {
			getPtrSizes(ftd[i].type, ptrSizes, funcPtrSizes);
		}
		unsigned long long nearsize = inf_is_64bit() ? 8 : (inf_is_32bit() ? 4 : 2),
			farsize = nearsize + ph.segreg_size;
		bool isNear = false, isFar = false, isCodeNear = false, isCodeFar = false;
		for (size_t i = 0; i < ptrSizes.size(); i++) {
			if (ptrSizes[i] == farsize) {
				if (isNear) { isNear = false; break; }
				isFar = true;
			} else if (ptrSizes[i] == nearsize) {
				if (isFar) { isFar = false; break; }
				isNear = true;
			}
		}
		for (size_t i = 0; i < funcPtrSizes.size(); i++) {
			if (funcPtrSizes[i] == farsize) {
				if (isCodeNear) { isCodeNear = false; break; }
				isCodeFar = true;
			} else if (funcPtrSizes[i] == nearsize) {
				if (isCodeFar) { isCodeFar = false; break; }
				isCodeNear = true;
			}
		}
		if (argSize != -1 && ftd.stkargs != argSize) {
			if (isFar && ftd.stkargs - ptrSizes.size() * ph.segreg_size == argSize) {
				isNear = true;
				isFar = false;
				bFrameChange = true;
				ftd.stkargs = argSize;
			} else if (isNear && ftd.stkargs + ptrSizes.size() * ph.segreg_size == argSize) {
				isFar = true;
				isNear = false;
				bFrameChange = true;
				ftd.stkargs = argSize;
			} else if (isCodeFar && ftd.stkargs - funcPtrSizes.size() * ph.segreg_size == argSize) {
				isCodeNear = true;
				isCodeFar = false;
				bFrameChange = true;
				ftd.stkargs = argSize;
			} else if (isCodeNear && ftd.stkargs + funcPtrSizes.size() * ph.segreg_size == argSize) {
				isCodeFar = true;
				isCodeNear = false;
				bFrameChange = true;
				ftd.stkargs = argSize;
			} else if (isFar && isCodeFar && ftd.stkargs - (ptrSizes.size() + funcPtrSizes.size()) * ph.segreg_size == argSize) {
				isNear = true; isCodeNear = true;
				isFar = false; isCodeFar = false;
				bFrameChange = true;
				ftd.stkargs = argSize;
			} else if (isNear && isCodeNear && ftd.stkargs + (ptrSizes.size() + funcPtrSizes.size()) * ph.segreg_size == argSize) {
				isFar = true; isCodeFar = true;
				isNear = false; isCodeNear = false;
				bFrameChange = true;
				ftd.stkargs = argSize;
			} else if (isNear && isCodeFar && ftd.stkargs + (ptrSizes.size() - funcPtrSizes.size()) * ph.segreg_size == argSize) {
				isFar = true; isCodeNear = true;
				isNear = false; isCodeFar = false;
				bFrameChange = true;
				ftd.stkargs = argSize;
			} else if (isFar && isCodeNear && ftd.stkargs + (funcPtrSizes.size() - ptrSizes.size()) * ph.segreg_size == argSize) {
				isNear = true; isCodeFar = true;
				isFar = false; isCodeNear = false;
				bFrameChange = true;
				ftd.stkargs = argSize;
			}
			if (bFrameChange) {
				fixPtrSizes(ftd.rettype, nearsize, farsize, isNear, isFar, isCodeNear, isCodeFar);
				for (size_t i = 0; i < ftd.size(); i++) {
					fixPtrSizes(ftd[i].type, nearsize, farsize, isNear, isFar, isCodeNear, isCodeFar);
				}
			}
		}
		if ((ftd.cc & CM_M_MASK) == CM_M_FN && isFar) ftd.cc = (ftd.cc & ~CM_M_MASK) | CM_M_FF;
		else if ((ftd.cc & CM_M_MASK) == CM_M_NN && isFar) ftd.cc = (ftd.cc & ~CM_M_MASK) | CM_M_NF;
		else if ((ftd.cc & CM_M_MASK) == CM_M_NF && isNear) ftd.cc = (ftd.cc & ~CM_M_MASK) | CM_M_NN;
		else if ((ftd.cc & CM_M_MASK) == CM_M_FF && isNear) ftd.cc = (ftd.cc & ~CM_M_MASK) | CM_M_FN;
		if ((ftd.cc & CM_M_MASK) == CM_M_NF && isCodeFar) ftd.cc = (ftd.cc & ~CM_M_MASK) | CM_M_FF;
		else if ((ftd.cc & CM_M_MASK) == CM_M_NN && isCodeFar) ftd.cc = (ftd.cc & ~CM_M_MASK) | CM_M_FN;
		else if ((ftd.cc & CM_M_MASK) == CM_M_FN && isCodeNear) ftd.cc = (ftd.cc & ~CM_M_MASK) | CM_M_NN;
		else if ((ftd.cc & CM_M_MASK) == CM_M_FF && isCodeNear) ftd.cc = (ftd.cc & ~CM_M_MASK) | CM_M_NF;
		return bFrameChange;
	}
	void IdaCallback::funcInfoToIDA(FuncProtoInfo& paramInfo, tinfo_t & ti)
	{
		func_type_data_t ftd;
		//ti.get_func_details(&ftd);
		addrToArgLoc(paramInfo.retType.addr, ftd.retloc);
		typeInfoToIDA(0, paramInfo.retType.pi.ti, ftd.rettype);
		if (paramInfo.isNoReturn) ftd.flags |= FTI_NORET;
		ftd.flags |= FTI_ARGLOCS;
		if (paramInfo.killedByCall.size() != 0) ftd.flags |= FTI_SPOILED;
		if (paramInfo.model == "__stdcall") {
			ftd.cc |= (paramInfo.customStorage ? CM_CC_SPECIALP : CM_CC_STDCALL);
		} else if (paramInfo.model == "__fastcall" || paramInfo.model == "__regcall" || paramInfo.model == "__vectorcall") {
			ftd.cc |= (paramInfo.customStorage ? CM_CC_SPECIALP : CM_CC_FASTCALL);
		} else if (paramInfo.model == "__thiscall") {
			ftd.cc |= (paramInfo.customStorage ? CM_CC_SPECIALP : CM_CC_THISCALL);
		} else if (paramInfo.model == "__cdecl") {
			ftd.cc |= (paramInfo.dotdotdot ? (paramInfo.customStorage ? CM_CC_SPECIALE : CM_CC_ELLIPSIS) : (paramInfo.customStorage ? CM_CC_SPECIAL : CM_CC_CDECL));
		} else if (paramInfo.model == "__pascal") {
			ftd.cc |= (paramInfo.customStorage ? CM_CC_SPECIALP : CM_CC_PASCAL);
		} else if (paramInfo.model == "unknown") {
			ftd.cc |= CM_CC_UNKNOWN;
		} else if (paramInfo.model == "__stdcall16far") {
			ftd.cc |= (paramInfo.customStorage ? CM_CC_SPECIALP : CM_CC_STDCALL) | CM_N16_F32;
			if (!is_code_far(inf_cc_cm)) ftd.flags |= FTI_FARCALL;
		} else if (paramInfo.model == "__stdcall16near") {
			ftd.cc |= (paramInfo.customStorage ? CM_CC_SPECIALP : CM_CC_STDCALL) | CM_N16_F32;
			if (is_code_far(inf_cc_cm)) ftd.flags |= FTI_NEARCALL;
		} else if (paramInfo.model == "__cdecl16far") {
			ftd.cc |= (paramInfo.dotdotdot ? (paramInfo.customStorage ? CM_CC_SPECIALE : CM_CC_ELLIPSIS) : (paramInfo.customStorage ? CM_CC_SPECIAL : CM_CC_CDECL)) | CM_N16_F32;
			if (!is_code_far(inf_cc_cm)) ftd.flags |= FTI_FARCALL;
		} else if (paramInfo.model == "__cdecl16near") {
			ftd.cc |= (paramInfo.dotdotdot ? (paramInfo.customStorage ? CM_CC_SPECIALE : CM_CC_ELLIPSIS) : (paramInfo.customStorage ? CM_CC_SPECIAL : CM_CC_CDECL)) | CM_N16_F32;
			if (is_code_far(inf_cc_cm)) ftd.flags |= FTI_NEARCALL;
		}
		ftd.cc |= (inf_cc_cm & CM_M_MASK);
		ftd.cc |= (inf_cc_cm & CM_MASK);
		/*if (paramInfo.customStorage && ((ftd.cc & CM_CC_MASK) == CM_CC_STDCALL ||
			(ftd.cc & CM_CC_MASK) == CM_CC_PASCAL || (ftd.cc & CM_CC_MASK) == CM_CC_FASTCALL
			|| (ftd.cc & CM_CC_MASK) == CM_CC_THISCALL)) ftd.cc = (ftd.cc & CM_CC_MASK) | CM_CC_SPECIALP;
		if (paramInfo.customStorage && (ftd.cc & CM_CC_MASK) != CM_CC_STDCALL) ftd.cc = (ftd.cc & CM_CC_MASK) | CM_CC_SPECIAL;
		if (paramInfo.dotdotdot && (ftd.cc & CM_CC_MASK) != CM_CC_ELLIPSIS) ftd.cc = (ftd.cc & CM_CC_MASK) | CM_CC_SPECIALE;*/
		for (size_t i = 0; i < paramInfo.killedByCall.size(); i++) {
			reg_info_t ri;
			std::string reg = decInt->getRegisterFromIndex(paramInfo.killedByCall[i].addr.offset, (int)paramInfo.killedByCall[i].size);
			parse_reg_name(&ri, reg.c_str());
			ftd.spoiled.push_back(ri);
		}
		uval_t stkargs = 0;
		for (size_t i = 0; i < paramInfo.syminfo.size(); i++) {
			if (paramInfo.syminfo[i].argIndex == -1) continue;
			funcarg_t fa;
			addrToArgLoc(paramInfo.syminfo[i].addr, fa.argloc);
			typeInfoToIDA(0, paramInfo.syminfo[i].pi.ti, fa.type);
			fa.name = qstring(paramInfo.syminfo[i].pi.name.c_str());
			if (ftd.size() <= paramInfo.syminfo[i].argIndex) ftd.resize(paramInfo.syminfo[i].argIndex + 1);
			if (paramInfo.syminfo[i].addr.addr.space == "stack") stkargs += (uval_t)paramInfo.syminfo[i].addr.size;
			ftd[paramInfo.syminfo[i].argIndex] = fa;
		}
		ftd.stkargs = stkargs;
		//the far/near data/call method is basically irrelevant unless there are arguments/return value containing data or function pointers
		//either the default as above, or the prior value would be fine
		tryFixFuncModel(ftd, BADADDR);
		//type_t t = BTMT_DEFCALL; //FTI_DEFCALL
		//if ((ftd.flags & FTI_NEARCALL) != 0) t = BTMT_NEARCALL;
		//else if ((ftd.flags & FTI_FARCALL) != 0) t = BTMT_FARCALL;
		//else if ((ftd.flags & FTI_INTCALL) != 0) t = BTMT_INTCALL;
		ti.create_func(ftd, BT_FUNC | (ftd.get_call_method() >> 2));
	}
	void IdaCallback::typeInfoToIDA(int idx, std::vector<TypeInfo>& type, tinfo_t & ti)
	{
		if (type[idx].size == -1) {
			ti.get_named_type(get_idati(), type[idx].typeName.c_str());
			return;
		}
		if (type[idx].metaType == "ptr") {
			ptr_type_data_t ptd;
			typeInfoToIDA(idx + 1, type, ptd.obj_type);
			type_t t = BTMT_DEFPTR;
			//if (ptd.obj_type.is_func()) {
				//ptd.closure;
				//t = BTMT_CLOSURE; //this pointer of capture object constructed
			//} else {
				ptd.based_ptr_size = (uchar)type[idx].size;
				unsigned long long nearsize = inf_is_64bit() ? 8 : (inf_is_32bit() ? 4 : 2),
					farsize = nearsize + ph.segreg_size;
				if (type[idx].size == nearsize && is_data_far(inf_cc_cm)) t = BTMT_NEAR;
				else if (type[idx].size == farsize && !is_data_far(inf_cc_cm)) t = BTMT_FAR;				
			//}
			ti.create_ptr(ptd, BT_PTR | t);
		} else if (type[idx].metaType == "struct") {
			udt_type_data_t utd;
			for (size_t i = 0; i < type[idx].structMembers.size(); i++) {
				udt_member_t um;
				um.name = type[idx].structMembers[i].name.c_str();
				um.offset = type[idx].structMembers[i].offset;
				typeInfoToIDA(0, type[idx].structMembers[i].ti, um.type);
				utd.push_back(um);
			}
			ti.create_udt(utd, BTF_STRUCT);
		} else if (type[idx].metaType == "array") {
			array_type_data_t atd;
			atd.nelems = (uint32)type[idx].arraySize;
			typeInfoToIDA(idx + 1, type, atd.elem_type);
			ti.create_array(atd);
		} else if (type[idx].metaType == "code") {
			funcInfoToIDA(type[idx].funcInfo, ti);
		} else if (type[idx].metaType == "void") {
			ti.create_simple_type(BT_VOID);
		} else {
			if (type[idx].isEnum) {
				enum_type_data_t etd;
				for (size_t i = 0; i < type[idx].enumMembers.size(); i++) {
					enum_member_t em;
					em.name = type[idx].enumMembers[i].first.c_str();
					em.value = type[idx].enumMembers[i].second;
					etd.push_back(em);
				}
				ti.create_enum(etd);
			} else {
				if ((type[idx].metaType == "int" || type[idx].metaType == "uint" || type[idx].metaType == "unknown") &&
					(type[idx].size == 1 || type[idx].size == 2 || type[idx].size == 4 || type[idx].size == 8)) {
					type_t t = 0;
					if (type[idx].metaType == "int") t = BTMT_SIGNED;
					else if (type[idx].metaType == "uint") t = BTMT_UNSIGNED;
					else if (type[idx].metaType == "unknown") t = BTMT_UNKSIGN;
					if (type[idx].size == 1) ti.create_simple_type(BT_INT8 | t);
					else if (type[idx].size == 2) ti.create_simple_type(BT_INT16 | t);
					else if (type[idx].size == 4) ti.create_simple_type(BT_INT32 | t);
					else if (type[idx].size == 8) ti.create_simple_type(BT_INT64 | t);
				} else if (type[idx].metaType == "float" && (type[idx].size == 4 || type[idx].size == 8 || type[idx].size == 10)) {
					ti.create_simple_type(BT_FLOAT | (type[idx].size == 4 ? BTMT_FLOAT : (type[idx].size == 8 ? BTMT_DOUBLE : BTMT_LNGDBL)));
#ifndef __X64__
				} else if (type[idx].metaType == "bool" && (type[idx].size == 1 || type[idx].size == 4 || type[idx].size == 2)) {
#else
				} else if (type[idx].metaType == "bool" && (type[idx].size == 1 || type[idx].size == 4 || type[idx].size == (inf_is_64bit() ? 8 : 2))) {
#endif
					ti.create_simple_type(BT_BOOL | (type[idx].size == 1 ? BTMT_BOOL1 : (type[idx].size == 4 ? BTMT_BOOL4 : BTMT_BOOL2 | BTMT_BOOL8)));
				} else {
					array_type_data_t atd;
					atd.nelems = (uint32)type[idx].size;
					atd.elem_type.create_simple_type(BT_INT8 | BTMT_CHAR);
					ti.create_array(atd);
				}
			}
		}
	}
	Options IdaCallback::getOpts()
	{
		Options opt = defaultOptions;
		opt.protoEval = di->customCallStyle.empty() ? ccToStr(inf_cc_cm, is_code_far(inf_cc_cm) ? FTI_FARCALL : FTI_NEARCALL, true) : di->customCallStyle; //default calling convention can be very important such as on x86-16
		opt.decompileUnreachable = (di->alysChecks & 1) != 0;
		opt.ignoreUnimplemented = (di->alysChecks & 2) != 0;
		opt.inferConstPtr = (di->alysChecks & 4) != 0;
		opt.readonly = (di->alysChecks & 8) != 0;
		opt.decompileDoublePrecis = (di->alysChecks & 16) != 0;
		opt.conditionalexe = (di->alysChecks & 32) != 0;
		opt.inPlaceOps = (di->alysChecks & 64) != 0;
		opt.commentIndent = (int)di->cmtLevel;
		opt.noCastPrinting = (di->dispChecks & 1) != 0;
		opt.commentUser1 = (di->dispChecks & 2) != 0; //EOL
		opt.commentHeader = (di->dispChecks & 4) != 0;
		opt.commentInstructionHeader = (di->dispChecks & 32) != 0; //PLATE
		opt.commentUser3 = (di->dispChecks & 64) != 0; //POST
		opt.commentUser2 = (di->dispChecks & 128) != 0; //PRE
		opt.commentWarning = (di->dispChecks & 256) != 0;
		opt.commentWarningHeader = (di->dispChecks & 256) != 0;
		opt.nullPrinting = (di->dispChecks & 512) != 0;
		opt.conventionPrinting = (di->dispChecks & 1024) != 0;
		opt.commentStyle = (di->comStyle ? "cplusplus" : "c");
		opt.integerFormat = (di->intFormat ? (di->intFormat == 2 ? "best" : "dec") : "hex");
		opt.maxLineWidth = (int)di->maxChars;
		opt.indentIncrement = (int)di->numChars;
		return opt;
	}
	void checkNearFarFuncModelInfo(ea_t ea)
	{
		func_t* f = get_func(ea);
		if (f == nullptr) return;
		tinfo_t ti;
		if (getFuncByGuess(ea, ti)) {
			func_type_data_t ftd;
			bool bChanged = false, bFrameChange = false;
			if (ti.get_func_details(&ftd)) {
				if (f->is_far() && (ftd.get_call_method() == FTI_NEARCALL || ftd.get_call_method() == FTI_DEFCALL && !is_code_far(inf_cc_cm)) ||
					!f->is_far() && (ftd.get_call_method() == FTI_FARCALL || ftd.get_call_method() == FTI_DEFCALL && is_code_far(inf_cc_cm))) {
					ftd.flags &= ~FTI_CALLTYPE;
					ftd.flags |= (f->is_far() ? FTI_FARCALL : FTI_NEARCALL);
					bChanged = true;
				}
			}
			if (f->frame != BADNODE) {
				struc_t* s = get_frame(f);
				//range_t range;
				//get_frame_part(&range, f, FPC_ARGS);
				//int argSize = range.end_ea - range.start_ea;
				//argloc_t al;
				//const tinfo_t t(inf_is_64bit() ? BT_INT64 : (inf_is_32bit() ? BT_INT32 : BT_INT16)); //or use BT_INT8 - 1 byte like Ghidra?
				//ph.calc_retloc(&al, t, inf_cc_cm) == 1;
				ea_t argSize = s->memqty == 0 ? 0 : s->members[s->memqty - 1].eoff - frame_off_args(f);
				//unsigned long long nearsize = inf_is_64bit() ? 8 : (inf_is_32bit() ? 4 : 2),
					//farsize = nearsize + ph.segreg_size;
				//if (ftd.stkargs != argSize) {
					//ftd.cc = (ftd.cc & ~CM_M_MASK) | (is_code_far(ftd.cc) ? CM_M_FN : CM_M_NN);
					//ftd.guess_cc(f->argsize, 0);
					//if (!is_user_cc(ftd.cc)) ph.calc_arglocs(&ftd);
					/*if (ftd.is_vararg_cc()) {
						regobjs_t rego;
						relobj_t relo;
						ph.calc_varglocs(&ftd, &rego, &relo, ftd.size());
					}*/					
				//}
				bFrameChange = tryFixFuncModel(ftd, argSize);
				/*for (size_t i = 0; i < s->memqty; i++) {
					xreflist_t xlt;
					build_stkvar_xrefs(&xlt, f, &s->members[i]);
					for (size_t j = 0; j < xlt.size(); j++) {
						flags_t fl = get_flags(xlt[j].ea);
						int stkvar = is_stkvar0(fl) ? 0 : (is_stkvar1(fl) ? 1 : -1);
						if (stkvar == -1) continue;
						insn_t insn;
						decode_insn(&insn, xlt[j].ea);
						op_t op = insn.ops[stkvar];
						if (op.type == o_displ && op.dtype == dt_word && s->members[i].eoff - s->members[i].soff == nearsize) {
							//weak indicator: 2 byte near pointer detected as 4 bytes
						} else if (op.type == o_displ && op.dtype == dt_dword && s->members[i].eoff - s->members[i].soff == farsize) {
							//weak indicator: 4 byte near pointer detected as 6 bytes
						} else if (op.type == o_displ && op.dtype == dt_dword && s->members[i].eoff - s->members[i].soff == nearsize) {
							//strong indicator: 4 byte far pointer
						} else if (op.type == o_displ && op.dtype == dt_fword && s->members[i].eoff - s->members[i].soff == farsize) {
							//strong indicator: 6 byte far pointer
						}
							//lds/les/lfs/lgs/lss are the normal way of loading a far pointer, second operand is m16:16/32
					}
				}*/
			}
			if (bChanged || bFrameChange) {
				tinfo_t newti;
				newti.create_func(ftd, BT_FUNC | (ftd.get_call_method() >> 2));
				apply_tinfo(ea, newti, TINFO_DEFINITE); //if a structure name has the function name, seems to be causing a warning message - very long standing issue in IDA apparently
				if (bFrameChange) {
					del_frame(f);
					ph.create_func_frame(f);
				}
			}
		}
	}
	bool compareFuncInfo(const FuncProtoInfo& f1, const FuncProtoInfo& f2);
	bool compareParamInfo(const std::vector<TypeInfo> & t1, const std::vector<TypeInfo>& t2)
	{
		if (t1.size() != t2.size()) return false;
		for (size_t i = 0; i < t1.size(); i++) {
			if (t1[i].arraySize != t2[i].arraySize || t1[i].enumMembers.size() != t2[i].enumMembers.size() ||
				!compareFuncInfo(t1[i].funcInfo, t2[i].funcInfo) || t1[i].isChar != t2[i].isChar ||
				t1[i].isEnum != t2[i].isEnum || t1[i].isReadOnly != t2[i].isReadOnly || t1[i].isUtf != t2[i].isUtf ||
				t1[i].metaType != t2[i].metaType || t1[i].size != t2[i].size || t1[i].structMembers.size() != t2[i].structMembers.size() ||
				t1[i].typeName != t2[i].typeName) return false;
			for (size_t j = 0; j < t1[i].enumMembers.size(); j++) {
				if (t1[i].enumMembers[j].first != t2[i].enumMembers[j].first || t1[i].enumMembers[j].second != t2[i].enumMembers[j].second) return false;
			}
			for (size_t j = 0; j < t1[i].structMembers.size(); j++) {
				if (t1[i].structMembers[j].name != t2[i].structMembers[j].name || t1[i].structMembers[j].offset != t2[i].structMembers[j].offset ||
					!compareParamInfo(t1[i].structMembers[j].ti, t2[i].structMembers[j].ti)) return false;
			}
		}
		return true;
	}
	bool compareFuncInfo(const FuncProtoInfo & f1, const FuncProtoInfo & f2)
	{
		if (f1.customStorage != f2.customStorage || f1.dotdotdot != f2.dotdotdot ||
			f1.extraPop != f2.extraPop || f1.hasThis != f2.hasThis || f1.isConstruct != f2.isConstruct ||
			f1.isDestruct != f2.isDestruct || f1.isInline != f2.isInline || f1.isNoReturn != f2.isNoReturn ||
			f1.model != f2.model || f1.killedByCall.size() != f2.killedByCall.size() || f1.syminfo.size() != f2.syminfo.size()) return false;
		for (size_t i = 0; i < f1.killedByCall.size(); i++) {
			if (f1.killedByCall[i].size != f2.killedByCall[i].size || f1.killedByCall[i].addr.space != f2.killedByCall[i].addr.space ||
				f1.killedByCall[i].addr.offset != f2.killedByCall[i].addr.offset || f1.killedByCall[i].addr.joins.size() != f2.killedByCall[i].addr.joins.size()) return false;
			for (size_t j = 0; j < f1.killedByCall[i].addr.joins.size(); j++) {
				if (f1.killedByCall[i].addr.joins[j].size != f2.killedByCall[i].addr.joins[j].size ||
					f1.killedByCall[i].addr.joins[j].addr.offset != f2.killedByCall[i].addr.joins[j].addr.offset ||
					f1.killedByCall[i].addr.joins[j].addr.space != f2.killedByCall[i].addr.joins[j].addr.space) return false;
			}
		}
		for (size_t i = 0; i < f1.syminfo.size(); i++) {
			if (f1.syminfo[i].argIndex != f2.syminfo[i].argIndex ||
				f1.syminfo[i].addr.size != f2.syminfo[i].addr.size || f1.syminfo[i].addr.addr.offset != f2.syminfo[i].addr.addr.offset ||
				f1.syminfo[i].addr.addr.space != f2.syminfo[i].addr.addr.space || f1.syminfo[i].addr.addr.joins.size() != f2.syminfo[i].addr.addr.joins.size() ||
				f1.syminfo[i].range.space != f2.syminfo[i].range.space || f1.syminfo[i].range.beginoffset != f2.syminfo[i].range.beginoffset ||
				f1.syminfo[i].range.endoffset != f2.syminfo[i].range.endoffset ||
				f1.syminfo[i].pi.name != f2.syminfo[i].pi.name) return false;
			for (size_t j = 0; j < f1.syminfo[i].addr.addr.joins.size(); j++) {
				if (f1.syminfo[i].addr.addr.joins[j].size != f2.syminfo[i].addr.addr.joins[j].size ||
					f1.syminfo[i].addr.addr.joins[j].addr.offset != f2.syminfo[i].addr.addr.joins[j].addr.offset ||
					f1.syminfo[i].addr.addr.joins[j].addr.space != f2.syminfo[i].addr.addr.joins[j].addr.space) return false;
			}
			if (!compareParamInfo(f1.syminfo[i].pi.ti, f2.syminfo[i].pi.ti)) return false;
		}
		if (f1.retType.argIndex != f2.retType.argIndex ||
			f1.retType.addr.size != f2.retType.addr.size || f1.retType.addr.addr.offset != f2.retType.addr.addr.offset ||
			f1.retType.addr.addr.space != f2.retType.addr.addr.space || f1.retType.addr.addr.joins.size() != f2.retType.addr.addr.joins.size() ||
			f1.retType.range.space != f2.retType.range.space || f1.retType.range.beginoffset != f2.retType.range.beginoffset ||
			f1.retType.range.endoffset != f2.retType.range.endoffset ||
			f1.retType.pi.name != f2.retType.pi.name) return false;
		for (size_t j = 0; j < f1.retType.addr.addr.joins.size(); j++) {
			if (f1.retType.addr.addr.joins[j].size != f2.retType.addr.addr.joins[j].size ||
				f1.retType.addr.addr.joins[j].addr.offset != f2.retType.addr.addr.joins[j].addr.offset ||
				f1.retType.addr.addr.joins[j].addr.space != f2.retType.addr.addr.joins[j].addr.space) return false;
		}
		if (!compareParamInfo(f1.retType.pi.ti, f2.retType.pi.ti)) return false;
		return true;
	}
	void IdaCallback::identParams(ea_t ea)
	{
		//if (funcProtoInfos.find(ea) != funcProtoInfos.end() && funcProtoInfos[ea].bFromParamId) return;
		//how to avoid excessive repeated calls?
		//if all types in frame are known, its a very good indicator
		//however sometimes specifically IDA will choose a wrong data model for libraries - far instead of near, and the frame and type both have bad information
		//perhaps this peculiar case should be solved by a simple database script which correlates the stack frame pointers to its specific usages
		//as largely it is just working around and fixing a bug in this particular area
		executeOnMainThread([ea]() { checkNearFarFuncModelInfo(ea); });
		DecMode dm = defaultDecMode;
		dm.actionname = "paramid";
		std::string display, funcProto, funcColorProto;
		FuncProtoInfo paramInfo = {};
		std::vector<std::tuple<std::vector<unsigned int>, std::string, unsigned int>> blockGraph;
		//should turn off eliminate unreachable option switch
		Options opt = getOpts();
		bool bChangeOpt = false;
		if (!opt.decompileUnreachable) {
			opt.decompileUnreachable = true;
			decInt->setOptions(opt);
			bChangeOpt = true;
		}
		//decInt->toggleCCode(false);
		//decInt->toggleSyntaxTree(false);
		dm.printCCode = false;
		dm.printSyntaxTree = false;
		decInt->doDecompile(dm, AddrInfo{ "ram", ea }, display, funcProto,
			funcColorProto, paramInfo, blockGraph); //let outer try handle errors
		if (bChangeOpt) decInt->setOptions(getOpts());
		//perhaps better way to detect graceful error than empty model - return XML has comment field - perhaps empty string if success for paramid
		if (paramInfo.model.size() == 0) return; //error occurred such as most likely overlapping input varnodes
		funcProtoInfos[ea] = FuncInfo{ funcProtoInfos[ea].name, true, paramInfo, funcProtoInfos[ea].fpi };
	}
#ifdef __X64__
	class FuncChooser : public chooser_multi_t
	{
	public:
		struct FuncRow
		{
			qstring oldval;
			qstring val;
		};
		std::vector<FuncRow> inits;
		static const int widths[];
		static const char* strs[];
		FuncChooser(std::vector<FuncRow>& its, sizevec_t* s) : chooser_multi_t(CH_KEEP | CH_MODAL, 2, widths, strs, "Function Prototype Changes"), inits(its), sv(s)
		{}
		sizevec_t* sv;
		/// User pressed the enter key.
		/// \param[in,out] sel  selected items
		/// \return             what is changed
		virtual cbres_t idaapi enter(sizevec_t* sel)
		{
			//sv = *sel;
			*sv = *sel;
#if IDA_SDK_VERSION < 720
			if (sel->size() == 0) return NOTHING_CHANGED;
			return (sel->size() == inits.size()) ? ALL_CHANGED : SELECTION_CHANGED;
#else
			return cbres_t(callui(ui_chooser_default_enter, this, sel).i);
#endif
		}
		/// Selection changed
		/// \note This callback is not supported in the txt-version.
		/// \param sel  new selected items
		virtual void idaapi select(const sizevec_t& sel) const
		{
			*sv = sel;
			//void* thisPtr = (void*)this; //serious hack to get around the const
			//((FuncChooser*)thisPtr)->sv = sel;
		}
		/// get the number of elements in the chooser
		virtual size_t idaapi get_count() const
		{
			return inits.size();
		}

		/// get a description of an element.
		/// \param[out] cols   vector of strings. \n
		///                    will receive the contents of each column
		/// \param[out] icon   element's icon id, -1 - no icon
		/// \param[out] attrs  element attributes
		/// \param n           element number (0..get_count()-1)
		virtual void idaapi get_row(
			qstrvec_t* cols,
			int* icon_,
			chooser_item_attrs_t* attrs,
			size_t n) const
		{
			if (n >= inits.size()) return;
			if (cols != nullptr) {
				(*cols)[0] = inits[n].oldval;
				(*cols)[1] = inits[n].val;
			}
			if (icon_ != nullptr) *icon_ = -1;
			if (attrs != nullptr) {
				//attrs->color = DEFCOLOR;
				//attrs->flags = 0; //CHITEM_...
			}
		}
	};
	const int FuncChooser::widths[] = { 40, 40 };
	const char* FuncChooser::strs[] = { "Old", "New" };
#endif

	void IdaCallback::updateDatabaseFromParams(std::vector<ea_t> eas)
	{
		if (!di->bSaveParamIdToIDA) return;
		for (size_t i = 0; i < eas.size(); ) {
			if (!funcProtoInfos[eas[i]].bFromParamId) eas.erase(eas.begin() + i); else i++;
		}
#ifdef __X64__
		executeOnMainThread([this, eas]() {
			//std::vector<ushort> checks;
			//checks.resize(eas.size());
			sizevec_t checks;
			if (eas.size() == 1) {
				std::string val, disp;
				if (!compareFuncInfo(funcProtoInfos[eas[0]].fpi, funcProtoInfos[eas[0]].oldfpi)) {
					val = "Change signature from '";
					//disp = "Change signature from '";
					FuncProtoInfo fpi = funcProtoInfos[eas[0]].fpi;
					funcProtoInfos[eas[0]].fpi = funcProtoInfos[eas[0]].oldfpi;
					val += print_func(eas[0], disp, COLOR_DEFAULT);
					funcProtoInfos[eas[0]].fpi = fpi;
					val += "' to parameter identified version from Ghidra: '";
					//disp += "' to parameter identified version from Ghidra: '";
					val += print_func(eas[0], disp, COLOR_DEFAULT);
					val += "'?";
					//disp += "'?";
				} else return;
				if (ask_yn(ASKBTN_YES, val.c_str()) != ASKBTN_YES) return;
				//checks[0] = 1;
				checks.push_back(0);
			} else {
				//qstring ask = "GhidraDec Parameter Identification Acceptance\n"
				//	"\n"
				//	"\n"
				//	"Accepted parameter identifications will cause updates to occur in both IDA function prototype and function stack frame.\n"
				//	"\n"
				//	"<~C~hooser:E1:0:80:::>\n\n";
				std::vector<FuncChooser::FuncRow> vals;
				for (size_t idx = 0; idx < eas.size(); idx++) {
					std::string val, valold, disp;
					if (!compareFuncInfo(funcProtoInfos[eas[idx]].fpi, funcProtoInfos[eas[idx]].oldfpi)) {
						FuncProtoInfo fpi = funcProtoInfos[eas[idx]].fpi;
						funcProtoInfos[eas[idx]].fpi = funcProtoInfos[eas[idx]].oldfpi;
						valold = print_func(eas[idx], disp, COLOR_DEFAULT);
						funcProtoInfos[eas[idx]].fpi = fpi;
						val = print_func(eas[idx], disp, COLOR_DEFAULT);
						//ask += qstring("<:C>") + qstring(std::to_string(idx + 1).c_str()) + ">" + qstring(val.c_str()) + "   " + qstring(valold.c_str()) + "\n";
						vals.push_back(FuncChooser::FuncRow{ valold.c_str(), val.c_str() });
						checks.push_back(idx);
					}
					//checks[idx] = 1;
				}
				if (vals.size() == 0) return;
				//(va_list)checks.data() //this will not work on x86-64 GCC which uses different ABI format for va_list
				FuncChooser cm(vals, &checks);
				//ask_form(ask.c_str(), &cm);
				ssize_t choice = cm.choose(checks); //initial choice but does not return and select is const method that cannot be used
				if (choice == chooser_base_t::NO_SELECTION || choice == chooser_base_t::EMPTY_CHOOSER) return;
				//checks = cm.sv;
			}
			for (size_t idx = 0; idx < eas.size(); idx++) {
				if (checks.find(idx) == checks.end()) continue;
				ea_t ea = eas[idx];
				tinfo_t ti;
				if (!getFuncByGuess(eas[idx], ti)) {}
				tinfo_t newti;
				FuncProtoInfo& paramInfo = funcProtoInfos[ea].fpi;
				funcInfoToIDA(paramInfo, newti);
				apply_tinfo(ea, newti, TINFO_DEFINITE); //can cause crash error 984 if type not correct
				func_type_data_t ftd;
				newti.get_func_details(&ftd);
				func_t* f = get_func(ea);
				if (f != nullptr) {
					bool bChanged = false;
					if (ftd.get_call_method() == FTI_FARCALL || ftd.get_call_method() == FTI_DEFCALL && is_code_far(inf_cc_cm) && !f->is_far() && (f->flags & FUNC_USERFAR) == 0) {
						f->flags |= FUNC_FAR; //FUNC_USERFAR
						bChanged = true;
					}
					if ((ftd.flags & FTI_NORET) != 0) {
						f->flags |= FUNC_NORET; bChanged = true;
					}
					if (f->argsize == 0 && (f->flags & FUNC_PURGED_OK) == 0) {
						f->flags |= FUNC_PURGED_OK;
						if (is_purging_cc(ftd.cc)) f->argsize = ftd.stkargs;
						bChanged = true;
					}
					if (bChanged) update_func(f);

					struc_t* frame = nullptr;
					if (f->frame != BADNODE) frame = get_frame(f);
					sval_t frsize = 0;
					asize_t argsize = 0;
					for (size_t i = 0; i < paramInfo.syminfo.size(); i++) {
						if (paramInfo.syminfo[i].addr.addr.space != "stack") continue;
						if (paramInfo.syminfo[i].argIndex == -1)
							frsize += (sval_t)paramInfo.syminfo[i].pi.ti.begin()->size;
						else
							argsize += (asize_t)paramInfo.syminfo[i].pi.ti.begin()->size;
					}
					if (frame != nullptr) {
						set_frame_size(f, frsize, f->frregs /*frame_off_savregs(f) - frame_off_lvars(f)*/, argsize);
					} else {
						if (frsize != 0 || argsize != 0) {
							add_frame(f, frsize, f->frregs, argsize);
							frame = get_frame(f);
						}
					}
					for (size_t i = 0; i < paramInfo.syminfo.size(); i++) {
						if (paramInfo.syminfo[i].addr.addr.space == "register") {
							if (paramInfo.syminfo[i].argIndex == -1) {
								std::string reg = decInt->getRegisterFromIndex(paramInfo.syminfo[i].addr.addr.offset, (int)paramInfo.syminfo[i].addr.size);
								regvar_t* rv = find_regvar(f, (ea_t)paramInfo.syminfo[i].range.beginoffset, (ea_t)paramInfo.syminfo[i].range.endoffset, reg.c_str(), nullptr);
								if (rv == nullptr)
									add_regvar(f, (ea_t)paramInfo.syminfo[i].range.beginoffset, (ea_t)paramInfo.syminfo[i].range.endoffset,
										reg.c_str(), paramInfo.syminfo[i].pi.name.c_str(), nullptr);
							} else {
								std::string reg = decInt->getRegisterFromIndex(paramInfo.syminfo[i].addr.addr.offset, (int)paramInfo.syminfo[i].addr.size);
								reg_info_t ri;
								parse_reg_name(&ri, reg.c_str());
								read_regargs(f);
								/*size_t j;
								for (j = 0; j < f->regargqty; j++) {
									if (f->regargs[j].reg == ri.reg) break;
								}
								if (j == f->regargqty) {}*/
								tinfo_t ti;
								typeInfoToIDA(0, paramInfo.syminfo[i].pi.ti, ti);
								add_regarg(f, ri.reg, ti, paramInfo.syminfo[i].pi.name.c_str());
							}
						} else if (paramInfo.syminfo[i].addr.addr.space == "stack") {
							tinfo_t ti;
							typeInfoToIDA(0, paramInfo.syminfo[i].pi.ti, ti);
							opinfo_t oi = {};
							flags_t flag;
							size_t sz; //size of ti
							get_idainfo_by_type(&sz, &flag, &oi, ti, nullptr);
							define_stkvar(f, paramInfo.syminfo[i].pi.name.c_str(),
								(sval_t)(paramInfo.syminfo[i].addr.addr.offset - (frame_off_args(f) - frame_off_retaddr(f))),
								flag, &oi, (asize_t)paramInfo.syminfo[i].addr.size);
						} else if (paramInfo.syminfo[i].addr.addr.space == "ram") {
						}
					}
				}
			}
			});
#endif
	}
	std::string IdaCallback::tryDecomp(DecMode dec, ea_t ea, std::string funcName, std::string& display, std::string& err,
		std::vector<std::tuple<std::vector<unsigned int>, std::string, unsigned int>>& blockGraph, bool paramOnly)
	{
		std::string code, funcProto, funcColorProto;
		FuncProtoInfo paramInfo;
		definedFuncs[ea] = true;
		try {
			if (di->decompPid == 0) decInt->registerProgram();
			if (funcProtoInfos.find(ea) == funcProtoInfos.end() || !funcProtoInfos[ea].bFromParamId) {
				identParams(ea);
				std::vector<ea_t> notIded;
				for (std::map<ea_t, FuncInfo>::iterator it = funcProtoInfos.begin(); it != funcProtoInfos.end(); it++)
					if (!it->second.bFromParamId) {
						if (imports.find(it->first) == imports.end()) notIded.push_back(it->first);
					}
				for (size_t i = 0; i < notIded.size(); i++) identParams(notIded[i]);
				for (std::map<ea_t, FuncInfo>::iterator it = funcProtoInfos.begin(); it != funcProtoInfos.end(); )
					if (!it->second.bFromParamId) funcProtoInfos.erase(it++); else it++;
				if (!paramOnly) {
					notIded.push_back(ea);
					updateDatabaseFromParams(notIded);
				}
				//usage tracking obviously should not be affected by sub-functions
				usedImports.clear();
				usedTypes.clear();
				usedData.clear();
				usedFuncs.clear();
			}
			if (paramOnly) return code;
			code = decInt->doDecompile(dec, AddrInfo{ "ram", ea }, display, funcProto, funcColorProto, paramInfo, blockGraph);
			if (funcProto.size() != 0) {
				funcProtos[ea] = funcProto;
				funcColorProtos[ea] = funcColorProto;
			} else {
				err = code.substr(3).c_str();
				code += "//Decompiling function: " + funcName + " @ 0x" + to_string(ea, std::hex) + "\n";
				display += "//Decompiling function: " + funcName + " @ 0x" + to_string(ea, std::hex) + "\n";
			}
		} catch (DecompError& e) {
			std::string str = e.explain;
			//str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
			//str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
			err = "Caught decompilation error: " + str + "\n";
			stopDecompilation(di, false, false, true);
			code = "//Error decompiling function: " + funcName + " @ 0x" + to_string(ea, std::hex) + "\n" "//" + str + "\n";
			display = "//Error decompiling function: " + funcName + " @ 0x" + to_string(ea, std::hex) + "\n" "//" + str + "\n";
		}
		if (recvfp != nullptr) qflush(recvfp);
		if (sendfp != nullptr) qflush(sendfp);
		if (recfp != nullptr) qflush(recfp);
		return code;
	}

	//graph.hpp and a graph emitter should take care of the AST information...

std::string tryDecomp(RdGlobalInfo* di, DecMode dec, ea_t ea, std::string & display, bool & bSucc,
	std::vector<std::tuple<std::vector<unsigned int>, std::string, unsigned int>>& blockGraph, bool paramOnly = false)
{
	std::string code, err;
	std::string fn = di->idacb->allFuncNames[ea];
	INFO_MSG((paramOnly ? "Identifying function parameters: " : "Decompiling function: ") << fn.c_str() << " @ " << std::hex << ea << std::endl);
	code = di->idacb->tryDecomp(dec, ea, fn.c_str(), display, err, blockGraph, paramOnly);
	if (err.size() != 0) { INFO_MSG(err); }
	else bSucc = true;
	return code;
}

bool detectProcCompiler(RdGlobalInfo* di, std::string& pspec, std::string & cspec, std::string& sleighfilename)
{
	std::vector<int>::iterator it = di->toolMap[inf_procname].begin();
	for (; it != di->toolMap[inf_procname].end(); it++) {
		if ((di->li[*it].size == 32 && inf_is_32bit() && !inf_is_64bit() || di->li[*it].size == 64 && inf_is_64bit() || di->li[*it].size != 32 && di->li[*it].size != 64 && !inf_is_32bit() && !inf_is_64bit()) &&
			di->li[*it].bigEndian == inf_is_be() || di->toolMap[inf_procname].size() == 1) { //should only be one match though, and parameters should always match
			sleighfilename = di->li[*it].basePath + di->li[*it].slafile;
			pspec = di->li[*it].basePath + di->li[*it].processorspec;
			if (di->li[*it].compilers.size() == 1) {
				cspec = di->li[*it].basePath + di->li[*it].compilers.begin()->second;
			} else {
				//gcc, windows, o32, n32, o64, macosx, borlandcpp, borlanddelphi, posStack, pointer16, pointer32, pointer64, iarV1, imgCraftV8, Archimedes and default
				if (default_compiler() == COMP_MS && di->li[*it].compilers.find("windows") != di->li[*it].compilers.end()) cspec = di->li[*it].basePath + di->li[*it].compilers["windows"];
				else if (default_compiler() == COMP_BC && di->li[*it].compilers.find("borlandcpp") != di->li[*it].compilers.end()) cspec = di->li[*it].basePath + di->li[*it].compilers["borlandcpp"];
				else if (default_compiler() == COMP_BP && di->li[*it].compilers.find("borlanddelphi") != di->li[*it].compilers.end()) cspec = di->li[*it].basePath + di->li[*it].compilers["borlanddelphi"];
				else if (default_compiler() == COMP_GNU && di->li[*it].compilers.find("gcc") != di->li[*it].compilers.end()) cspec = di->li[*it].basePath + di->li[*it].compilers["gcc"];
				else if (di->li[*it].compilers.find("default") != di->li[*it].compilers.end()) cspec = di->li[*it].basePath + di->li[*it].compilers["default"];
				else cspec = di->li[*it].basePath + di->li[*it].compilers.begin()->second;
			}
			break;
		}
	}
	if (it == di->toolMap[inf_procname].end()) {
		return false;
	}
	return true;
}

/**
 * Decompile locally on work station.
 * Working RetDec must be installed on the station.
 * @param di Plugin's global information.
 */
static void idaapi localDecompilation(RdGlobalInfo *di)
{
	//di->endian == "big", inf.min_ea
	std::string code, display;
	std::vector<std::tuple<std::vector<unsigned int>, std::string, unsigned int>> blockGraph;
	if (di->idacb->decInt == nullptr) di->idacb->decInt = new DecompInterface();
	std::vector<CoreType> cts(&defaultCoreTypes[0], &defaultCoreTypes[numDefCoreTypes]);
	try {
		di->idacb->decInt->setup(di->idacb, di->idacb->sleighfilename, di->idacb->pspec, di->idacb->cspec, cts, di->idacb->getOpts(), (int)di->timeout, (int)di->maxPayload);
	} catch (DecompError& err) {
		WARNING_GUI("Decompilation FAILED: " << err.explain << ".\n");
		di->decompSuccess = false;
		di->outputFile.clear();
		return;
	}
	for (std::map<ea_t, bool>::iterator it = di->idacb->exportData.begin(); it != di->idacb->exportData.end(); it++) {
		bool rdonly, volat;
		std::vector<TypeInfo> tc;
		di->idacb->lookupDataInfo(it->first, &rdonly, &volat, tc); //must create interface first - for register translation
	}
	time_t startTime = time(NULL);
	if (di->decompiledFunction == nullptr) {
		//can decompile entry points first
		std::map<ea_t, size_t> entries;
		size_t num = di->idacb->allFuncs.size();
		if (di->decompPid == 0) di->idacb->decInt->registerProgram();
		std::vector<ea_t> notIded;
		for (size_t i = 0; i < num; i++) {
			if (di->idacb->imports.find(di->idacb->allFuncs[i]) != di->idacb->imports.end()) continue;
			bool bSucc = false;
			std::string disp;
			tryDecomp(di, defaultDecMode, di->idacb->allFuncs[i], disp, bSucc, blockGraph, true);
			notIded.push_back(di->idacb->allFuncs[i]);
		}
		di->idacb->updateDatabaseFromParams(notIded);
		int successes = 0, total = 0;
		for (size_t i = 0; i < num; i++) {
			if (di->idacb->imports.find(di->idacb->allFuncs[i]) != di->idacb->imports.end()) continue;
			std::string disp;
			total++;
			bool bSucc = false;
			code += tryDecomp(di, defaultDecMode, di->idacb->allFuncs[i], disp, bSucc, blockGraph);
			if (di->exiting) {
				di->decompSuccess = false; return;
			}
			if (bSucc) successes++;
			display += disp;
		}
		INFO_MSG("Decompilation completed: " << successes << " successfully decompiled out of " << total << " (" << (100.0 * (successes / (double)total)) << "%) in " <<
			std::dec << (time(NULL) - startTime) << " seconds\n");
	} else {
		bool bSucc = false;
		if (di->idacb->imports.find(di->decompiledFunction->start_ea) != di->idacb->imports.end()) {
			di->decompSuccess = false;
			return;
		}
		code = tryDecomp(di, defaultDecMode, (unsigned long long)di->decompiledFunction->start_ea, display, bSucc, blockGraph);
		if (di->exiting) {
			di->decompSuccess = false; return;
		}
		INFO_MSG("Decompilation completed: " << di->idacb->allFuncNames[di->decompiledFunction->start_ea] << " in " << std::dec << (time(NULL) - startTime) << " seconds\n");
	}

	if (code.size() == 0) {
		WARNING_GUI("Decompilation FAILED.\n");
		di->decompSuccess = false;
		di->outputFile.clear();
		return;
	}

	std::string definitions, defdisplay, idaInfo;
	di->idacb->analysisDump(definitions, defdisplay, idaInfo);
	code = definitions + code + "\n";
	display = defdisplay + display + "\n";
	if (!idaInfo.empty()) {
		code += "/*\n" + idaInfo + "\n*/\n";
		std::string str;
		idaInfo = "/*\n" + idaInfo + "\n*/";
		std::for_each(idaInfo.begin(), idaInfo.end(), [&str](char c) { if (c == '\n') str += std::string({ COLOR_OFF, COLOR_AUTOCMT }) + c + std::string({ COLOR_ON, COLOR_AUTOCMT }); else str += c; });
		display += std::string({ COLOR_ON, COLOR_AUTOCMT }) + str + std::string({ COLOR_OFF, COLOR_AUTOCMT }) + "\n";
	}

	//should save to a .c file?
	//INFO_MSG("Decompiled file: " << decName << "\n");
	if (di->outputFile.size() != 0) {
		FILE* fp = qfopen(di->outputFile.c_str(), "wb");
		qfwrite(fp, code.c_str(), code.size());
		qfclose(fp);
		di->outputFile.clear();
	}

	if (di->isSelectiveDecompilation())
	{
		di->fnc2code[di->decompiledFunction].code = display;
		di->fnc2code[di->decompiledFunction].blockGraph = blockGraph;
	}

	di->decompSuccess = true;
}

ssize_t idaapi GraphCallback(void* user_data, int notification_code, va_list va)
{
	RdGlobalInfo* di = (RdGlobalInfo*)user_data;
	if (notification_code == grcode_user_refresh) {
		mutable_graph_t* mg = va_arg(va, mutable_graph_t*);
		rect_t r;
		ea_t ea = di->decompiledFunction->start_ea;
		const std::vector<std::tuple<std::vector<unsigned int>, std::string, unsigned int>>& blockGraph =
			di->fnc2code[get_func(ea)].blockGraph;
		mg->resize((int)blockGraph.size());
		/*for (size_t i = 0; i < blockGraph.size(); i++) {
			int node = mg->add_node(&r);
			node_info_t ni;
#ifdef __X64__
			ni.flags = NIFF_SHOW_CONTENTS;
#endif
			ni.text = ("  //" + std::to_string(i) + "\n" + std::get<1>(blockGraph[i])).c_str();
			set_node_info(mg->gid, node, ni, NIF_TEXT | NIF_FLAGS);
			//if (std::get<1>(blockGraph[i]).size() == 0) mg->node_flags[node] |= MTG_NON_DISPLAYABLE_NODE;
		}*/
		for (size_t i = 0; i < blockGraph.size(); i++) {
			for (size_t j = 0; j < std::get<0>(blockGraph[i]).size(); j++) {
				edge_info_t ei;
				mg->add_edge(std::get<0>(blockGraph[i])[j], (int)i, &ei);
			}
		}
		//for (size_t i = blockGraph.size() - 1; i != -1; i--) {
			//if (std::get<1>(blockGraph[i]).size() == 0) mg->del_node((int)i);
		//}
		mg->create_digraph_layout();
		mg->redo_layout();
		//refresh_viewer(di->graphViewer);
		return 1;
	} else if (notification_code == grcode_user_gentext) {
		mutable_graph_t* mg = va_arg(va, mutable_graph_t*);
		ea_t ea = di->decompiledFunction->start_ea;
		const std::vector<std::tuple<std::vector<unsigned int>, std::string, unsigned int>>& blockGraph =
			di->fnc2code[get_func(ea)].blockGraph;
		di->graphText.resize(blockGraph.size());
		for (size_t i = 0; i < blockGraph.size(); i++) {
			if (std::get<1>(blockGraph[i]).size() != 0)
				di->graphText[i] = ("  //" + std::to_string(std::get<2>(blockGraph[i])) + "\n" + std::get<1>(blockGraph[i])).c_str();
		}
		return 1;
	} else if (notification_code == grcode_user_text) {
		mutable_graph_t* mg = va_arg(va, mutable_graph_t*);
		int node = va_argi(va, int);
		const char** text = va_arg(va, const char**);
		bgcolor_t* bgcolor = va_arg(va, bgcolor_t*);
		*text = (di->graphText[node].size() == 0) ? nullptr : di->graphText[node].c_str();
		return 1;
		//} else if (notification_code == grcode_user_hint) {
	} else if (notification_code == grcode_user_title) {
		mutable_graph_t* mg = va_arg(va, mutable_graph_t*);
		int node = va_arg(va, int);
		rect_t* title_rect = va_arg(va, rect_t*);
		int title_bg_color = va_arg(va, int);
		void* dc = va_arg(va, void*); //this is not a Windows GDI device context apparently but a QT::QPainter object
		//QT::QPainter dc
		//RECT rect = { 0, 0, title_rect->right - title_rect->left, title_rect->bottom - title_rect->top };
		int invColor = (~title_bg_color & 0xffffff) | (title_bg_color & 0xff000000);
		//HBRUSH hbr = CreateSolidBrush(invColor);// title_bg_color);
		//FillRect(dc, &rect, hbr);
		//HPEN pn = CreatePen(PS_SOLID, 1, invColor);
		//HFONT fnt = CreateFont();
		//HGDIOBJ old = SelectObject(dc, pn);
		//COLORREF oldbk = SetBkColor(dc, invColor);
		//COLORREF oldfg = SetTextColor(dc, invColor);
		//std::string str = "//" + std::to_string(node);
		//DrawTextA(dc, str.c_str(), (int)str.size(), &rect, DT_CENTER | DT_TOP | DT_SINGLELINE);
		//SetBkColor(dc, oldbk);
		//SetTextColor(dc, oldfg);
		//SelectObject(dc, old);
		//DeleteObject(hbr);
		return 0;
	} else if (notification_code == grcode_user_draw) {
		return 0;
	//} else if (notification_code == grcode_destroyed) {
	//} else if (notification_code == grcode_changed_graph) {
	}
	return 0;
}

void displayBlockGraph(RdGlobalInfo* di, ea_t ea)
{
	if (!di->bShowGraph) return;
	di->idacb->executeOnMainThread([di, ea]() {
		if (di->graphViewer != nullptr) {
			close_widget(di->graphViewer, 0);
			di->graphViewer = nullptr;
		}
		if (di->graphWidget != nullptr) {
			close_widget(di->graphWidget, 0);
			di->graphWidget = nullptr;
			delete_mutable_graph(di->mg);
			di->graphText.clear();
			netnode nn("GhidraGraph", 0, true);
			nn.kill();
		}
		ea_t ea = di->decompiledFunction->start_ea;
		const std::vector<std::tuple<std::vector<unsigned int>, std::string, unsigned int>>& blockGraph =
			di->fnc2code[get_func(ea)].blockGraph;
		if (blockGraph.size() == 0) return;

		di->graphWidget = create_empty_widget((di->viewerName + " Graph").c_str());
		netnode nn("GhidraGraph", 0, true);
		di->mg = create_mutable_graph(nn);
		di->graphViewer = create_graph_viewer((di->viewerName + "_Graph").c_str(),
			nn, GraphCallback, di, 12, di->graphWidget);
		display_widget(di->graphWidget,
			WOPN_DP_TAB
#if !defined(IDA_SDK_VERSION) || IDA_SDK_VERSION < 720
			| WOPN_MENU
#endif
		);
		viewer_fit_window(di->graphViewer);
	});
}

/**
 * Thread function, it runs the decompilation and displays decompiled code.
 * @param ud Plugin's global information.
 */
static int idaapi threadFunc(void* ud)
{
	RdGlobalInfo* di = static_cast<RdGlobalInfo*>(ud);
	di->decompRunning = true;

	INFO_MSG("Local decompilation ...\n");
	localDecompilation(di);

	if (di->decompSuccess && di->isSelectiveDecompilation())
	{
		//showDecompiledCode(di);
		ShowOutput show(di);
		di->idacb->executeOnMainThread([&show]() {
			show.execute();
			});
		if (di->decompiledFunction != nullptr &&
			di->fnc2code.find(di->decompiledFunction) != di->fnc2code.end()) {
			displayBlockGraph(di, di->decompiledFunction->start_ea);
		}
	}

	di->outputFile.clear();
	di->decompRunning = false;
	return 0;
}

/**
 * Create ranges to decompile from the provided function.
 * @param[out] decompInfo Plugin's global information.
 * @param      fnc        Function selected for decompilation.
 */
void createRangesFromSelectedFunction(RdGlobalInfo& decompInfo, func_t* fnc)
{
	std::stringstream ss;
	ss << "0x" << std::hex << fnc->start_ea << "-" << "0x" << std::hex << (fnc->end_ea-1);

	decompInfo.ranges = ss.str();
	decompInfo.decompiledFunction = fnc;
}

/**
 * Decompile IDA's input.
 * @param decompInfo Plugin's global information.
 */
void decompileInput(RdGlobalInfo &decompInfo)
{
	INFO_MSG("Decompile input ...\n");

	// Construct decompiler call command.
	//
	decompInfo.decCmd = decompInfo.decompilerExePath + decompInfo.decompilerExeName;

	//database queries for main thread
	if (decompInfo.idacb == nullptr) decompInfo.idacb = new IdaCallback(&decompInfo);
	auto tmp = decompInfo.decCmd;
	std::replace(tmp.begin(), tmp.end(), ' ', '\n');
	INFO_MSG("Decompilation command: " << tmp << "\n");
	INFO_MSG("Running the decompilation command ...\n");

	std::string sleighfilename;
	std::string pspec;
	std::string cspec;

	if ((decompInfo.customCspec.empty() || decompInfo.customPspec.empty() || decompInfo.customSlafile.empty()) &&
		!detectProcCompiler(&decompInfo, pspec, cspec, sleighfilename)) {
		WARNING_GUI("No matching Ghidra processor found - Decompilation FAILED.\n");
		decompInfo.decompSuccess = false;
		decompInfo.outputFile.clear();
		return;
	}

	if (!decompInfo.customCspec.empty()) cspec = decompInfo.customCspec;
	if (!decompInfo.customPspec.empty()) pspec = decompInfo.customPspec;
	if (!decompInfo.customSlafile.empty()) sleighfilename = decompInfo.customSlafile;
	INFO_MSG("Detected Processor spec: " << pspec << " Compiler spec: " << cspec << " Sleigh file: " << sleighfilename << "\n");
	decompInfo.idacb->init(sleighfilename, pspec, cspec);

	// Create decompilation thread.
	//
	if (decompInfo.isUseThreads())
	{
		decompInfo.decompThread = qthread_create(threadFunc, static_cast<void*>(&decompInfo));
	}
	else
	{
		threadFunc(static_cast<void*>(&decompInfo));
	}
}

} // namespace idaplugin


/*
IDA offers a good amount of info about functions but its SDK is basically undocumented for accessing some of it. Fortunately ida64.dll can be decompiled and the little info the SDK docs understood.

Functions: func_t offers function details including members:

Code:
	  uval_t frame;        ///< netnode id of frame structure - see frame.hpp
...
		// the following fields should not be accessed directly:

	  uint32 pntqty;       ///< number of SP change points
	  stkpnt_t *points;    ///< array of SP change points.
						   ///< use ...stkpnt...() functions to access this array.

	  int regvarqty;       ///< number of register variables (-1-not read in yet)
						   ///< use find_regvar() to read register variables
	  regvar_t *regvars;   ///< array of register variables.
						   ///< this array is sorted by: start_ea.
						   ///< use ...regvar...() functions to access this array.

	  int llabelqty;       ///< number of local labels
	  llabel_t *llabels;   ///< local labels.
						   ///< this array is sorted by ea.
						   ///< use ...llabel...() functions to access this array.

	  int regargqty;       ///< number of register arguments
	  regarg_t *regargs;   ///< unsorted array of register arguments.
						   ///< use ...regarg...() functions to access this array.

	  int tailqty;         ///< number of function tails
	  range_t *tails;      ///< array of tails, sorted by ea.
						   ///< use func_tail_iterator_t to access function tails.
For regargs for example there is no way to read except through this do not access structure member. Most of them can only be queried on a per address or per item basis and not obtaining all for the function simultaneously as the structure hints should be easily possible. Further these are stored packed in netnodes and are not populated except on demand. Although the quantity fields will have correct values, the actual data will be null pointers waiting for a load on first use. So how to invoke this first use?

Code:
	size_t num = get_func_qty();
	for (size_t i = 0; i < num; i++) {
		func_t* f = getn_func(i);
		get_frame(f->start_ea); //load frame into internal pointer table
		find_regvar(f, f->start_ea, nullptr); //load regvars
		get_spd(f, f->start_ea); //load points
		read_regargs(f); //load regargs
		get_llabel(f, ea); //load llabels
		func_tail_iterator_t fti(pfn); //load tails
	}
get_frame(f/ea) is the only and necessary way to immediately load the frame information from the database. It is not stored in the function structure but the pointer can be looked up without being loaded/unpacked from db again. If the functions frame member is BADNODE then obviously this will return a null pointer and there is no frame for the function.

find_regvar(f, ea, _) although it is undocumented can accept a nullptr for the third argument and regardless it will always load the regvars member on first use. Presumably one could find the register variables by giving every possible canonical register name via enumeration of ph.reg_names[] array but this would be over doing it as regvars can be populated with a nullptr and accessing the array directly.

get_spd(f, ea), get_effective_sp(f, ea), get_sp_delta(f, ea) will always populate the points array if you pass the function pointer on first use. Although IDA claims a null pointer is possible, this function will always return 0 until it is called at least once on the function in question with that filled in. It does not call get_func(ea) for you like you would think. Totally misdocumented feature. recalc_spd(ea) will not always work either if one of the get_*sp* functions has not been used first - it works in some cases depending on which address in the function is used. If the ea for recalc_spd is part of a function chunk with no sp changes - it will not do anything leaving the points null. It actually walks through instructions in the particular function chunk/tail entry of the ea only so if there is a stack change in that exact one it will populate but its highly unlikely one would know this in advance. With that in mind, the stack points could be enumerated by going through every assembly instruction in the function using the appropriate enumeration functions, and querying get_*sp* functions to gather the info. But since we know the function has the array already such intensive searching is fortunately not necessary.

read_regargs(f) populates the regargs and is the only way to do so. They have to be accessed directly.

Now you have a regarg_t structure so what about this strange type_t *type; member? Mostly we want tinfo_t not type_t raw data.

Well this again is a poorly documented and tricky to access structure. Obviously we do not want to parse through it ourselves as its not SDK version portable, and its packed in a really detailed format. But if you try to load it directly it will cause corruption and a crash or bad data.

Code:
if (f->regargs[i].type != nullptr) {
	tinfo_t ti; //The type information is internally kept as an array of bytes terminated by 0.
	qtype typeForDeser;
	typeForDeser.append(f->regargs[i].type);
	ti.deserialize(get_idati(), &typeForDeser); //will free buffer passed!
}
The underlying deserialize API function looks like and the pointer to a pointer for the type_t member is notable - you must pass an IDA API compatible buffer:
Code:
decl bool ida_export deserialize_tinfo(tinfo_t *tif, const til_t *til, const type_t **ptype, const p_list **pfields, const p_list **pfldcmts);
As for function tails, these are relatively well documented right in funcs.hpp, finally having an iterator and not needing to use the internal access (although you can't index them but must enumerate them, a very small downside, in this case the SDK method is recommended):
Code:
	  func_tail_iterator_t fti(pfn);
	  for ( bool ok=fti.first(); ok; ok=fti.next() )
		const range_t &a = fti.chunk();
However if you want direct access to the array, and want preloading of it, this will do the trick (or anything that ultimately invokes func_tail_iterator_set(func_tail_iterator_t*, f, ea)):
Code:
	  func_tail_iterator_t fti(pfn);
Finally for labels, a different situation occurs. The actual llabel_t structure for a local label and the 3 associated functions set_llabel, get_llabel_ea and get_llabel are all mentioned with caveat: "These are LOW LEVEL FUNCTIONS. When possible, they should not be used. Use high level functions from <name.hpp>". Instead set_name, get_name_ea and get_name respectively should be used. And yes named could be enumerated by doing a walk through each assembly instruction or using various next_* searching functions. But whatever populating for direct access to all function labels quick and easy? This one seems to not be in ida64.dll but is actually provided as a .lib method. But the lib will just forward these to the recommended functions so it does not really matter.

So get_llabel(f, ea) or get_name(f, ea) will reliably do the job and there is no other suitable candidate though get_llabel_ea(f, nullptr) and get_name_ea(f, nullptr) might work if they accept a null pointer for the string.

IDA provides a very practical SDK for a very practical purpose. With a very big support price tag for it, it is no wonder there is poor documentation and lack of consistency. Working on the clock, it would be easier to justify a paper trail of support emails asking about these type of details rather than painstakingly tracking them out through trial and error or reverse engineering SDK lib files or the ida64.dll.

Perhaps these are part of IDA Pro's trade secrets, if you were to think of it as such.

It was quite difficult to get these simple things working correctly as none of them are properly documented. I hope this helps someone on their own IDA SDK development efforts.

In a multithreaded plugin, preloading everything is actually a necessity to minimize thread safety issues around various unsafe database access to race conditions. By preloading all of these structures, even if you want to access them using nicer API methods, it will not cause any database calls. Unfortunately the IDA SDK is designed for single threaded use mostly so doing a dump or lots of UI thread callbacks would be the only other options. But this idea will mitigate all contention except those caused by the user's own interaction changing data structures in use.

Another interesting function is execute_sync with MFF_NOWAIT which is needed when a thread doing work may need to be interrupted to prevent deadlock.
Code:
THREAD_SAFE inline int execute_sync(exec_request_t &req, int reqf) { return callui(ui_execute_sync, &req, reqf).i; }
We see a exec_request_t object reference is passed, and then a pointer to this is passed along. The return value is the ID for cancelling:
Code:
THREAD_SAFE inline bool cancel_exec_request(int req_id)
Now if you want to cancel the request, such as in an interruption case, likely crashes will occur. This is because for whatever inexplicable reason, cancel_exec_request is calling delete on the exec_request_t pointer. So if you did not allocate it with new but just as a stack variable, it will be trying to free stack memory causing an obvious crash.

To deal with this, the best I could come up with was using both a semaphore to track the exec request completion, along with a termination flag and a mutex around that to prevent race conditions:
Global (hopefully stored somewhere referenceable from a singular global object):
Code:
		bool exiting = false;
		int uiExecutingTask = -1;
		qmutex_t qm = qmutex_create();
Main thread termination:
Code:
		{ //must be a locked unit otherwise race condition can occur
			qmutex_locker_t lock(qm);
			exiting = true;
			if (uiExecutingTask != -1) {
				cancel_exec_request(uiExecutingTask);
			}
		}
Code:
	struct execFunctor : public exec_request_t
	{
		std::function<void()> fun;
		qsemaphore_t finishSem;
		execFunctor(std::function<void()> f, qsemaphore_t qs) : fun(f), finishSem(qs) {}
		virtual ~execFunctor(void) override {}
		/// Callback to be executed.
		/// If this function raises an exception, execute_sync() never returns.
		virtual int idaapi execute(void)
		{
			fun();
			qsem_post(finishSem);
			return 0;
		}
	};
	void executeOnMainThread(std::function<void()> fun)
	{
		qsemaphore_t finishSem = qsem_create(nullptr, 0);
		execFunctor* ef = new execFunctor(fun, finishSem);
		{ //must be a locked unit otherwise race condition can occur
			qmutex_locker_t lock(qm);
			if (exiting) return;
			uiExecutingTask = execute_sync(*ef, MFF_NOWAIT);
		}
		while (!qsem_wait(finishSem, 50)) {
			if (exiting) {
				uiExecutingTask = -1;
				qsem_free(finishSem);
				return;
			}
		}
		uiExecutingTask = -1;
		qsem_free(finishSem);
		delete ef;
Admittedly polling the exiting flag is not elegant - but IDA does not offer a way to get event object signaling, and multiple object wait so something like this is sufficient at least to be safe.

Now you can just wrap any code into a lambda as an argument to std::function<void()> to bind it and very easily execute anything on the main thread taking into account data thread safety issues if any. This allows for cancellable long running operations.

Another area of concern is process creation using
Code:
	launch_process_params_t procInf;
...
	PROCESS_INFORMATION pi{};
	procInf.info = &pi;
	void* p = launch_process(procInf, &errbuf);
IDA will leak thread handles on Windows unless you remember:
Code:
if (pi.hThread != INVALID_HANDLE_VALUE) CloseHandle(pi.hThread);
term_process and check_exit_process will both close the process handle for you.

Any uses of q functions with a create such as qthread_create must always have a matching free e.g. qthread_free and in this case should always have a qthread_join as well. Although qthread_kill is possible, it is a temporary workaround for deeper architectural issues or an emergency as the side effects nearly always outweigh the benefits and it could destabilize the IDA process.

Again all of these things were discovered, and not mentioned in any documentation.

Here is the final clean code of the above which is polling-free since polling technically should not be done in a waiting loop ever in modern OS code where the OS can always schedule that much more safely and efficiently:

Code:
	/// Execute code in the main thread - to be used with execute_sync().
	struct execFunctor : public exec_request_t
	{
		std::function<void()> fun;
		qsemaphore_t finishSem;
		int* taskNo;
		execFunctor(std::function<void()> f, qsemaphore_t qs, int* tsk) : fun(f), finishSem(qs), taskNo(tsk) {}
		virtual ~execFunctor(void) override {}
		/// Callback to be executed.
		/// If this function raises an exception, execute_sync() never returns.
		virtual int idaapi execute(void)
		{
			fun();
			*taskNo = -1;
			qsem_post(finishSem);
			return 0;
		}
	};
	void IdaCallback::executeOnMainThread(std::function<void()> fun)
	{
		execFunctor* ef = new execFunctor(fun, di->termSem, &di->uiExecutingTask);
		{ //must be a locked unit otherwise race condition can occur
			qmutex_locker_t lock(di->qm);
			if (di->exiting) return;
			di->uiExecutingTask = execute_sync(*ef, MFF_NOWAIT);
		}
		qsem_wait(di->termSem, -1);
		if (di->uiExecutingTask == -1) delete ef;
	}
Forced termination (of course the qsem_create/qsem_free must be called at program initialization and termination):
Code:
		{ //must be a locked unit otherwise race condition can occur
			qmutex_locker_t lock(decompInfo->qm);
			decompInfo->exiting = true;
			if (decompInfo->uiExecutingTask != -1) {
				if (cancel_exec_request(decompInfo->uiExecutingTask)) {
					qsem_post(decompInfo->termSem);
				}
			}
		}
Note that qwait_for_handles despite the docs not making it clear works for semaphores so you can do a multiple wait (WaitForMultipleObjects/select) e.g.:
Code:
int idx = -1;
qhandle_t handles[2] = { finishSem, termSem };
while (qwait_for_handles(&idx, handles, 2, 0, -1), idx == -1) {}
*/




/*
Advanced IdaPython to change details UI cannot change

----------------------------------------
Advanced IDAPython coding tutorial
----------------------------------------

For example, in 16-bit code, there is far-code near-data and the other 3 combinations for farness and nearness. Sometimes IDA gets it wrong with FLIRT and chooses the wrong model messing up stack parameter alignments and so forth. But you cannot change the function type as it already has pointers in it e.g.: void *__cdecl memset(void *, int, size_t);

The Options -> Compilers is the only place where this model can be specified but its a global setting despite that every function prototype has some setting for this.

So you want 2 byte pointers not 4 byte pointers for this particular function since it uses near-data far-code model. But the default for the file is far-data far-code. If the function has an unknown calling convention it will also use this default.

So how can you fix it without ruining the pointer type of the return and first arguments?

Code:
ea = idaapi.get_name_ea(idaapi.BADADDR, "_memset") "_fflush" "_rename" "_strcat" "_strlen"
tinfo = idaapi.tinfo_t()
idaapi.get_tinfo2(ea, tinfo)
fi = idaapi.func_type_data_t()
tinfo.get_func_details(fi)
fi.cc
fi.cc will return a value such as 50 (48+2) and the docs will help on the calling convention masks and values:
Code:
const cm_t  CM_N16_F32   = 0x02;  ///< 2: near 2bytes, far 4bytes

const cm_t CM_M_MASK = 0x0C;
const cm_t  CM_M_NN      = 0x00;  ///< small:   code=near, data=near (or unknown if CM_UNKNOWN)
const cm_t  CM_M_FF      = 0x04;  ///< large:   code=far, data=far
const cm_t  CM_M_NF      = 0x08;  ///< compact: code=near, data=far
const cm_t  CM_M_FN      = 0x0C;  ///< medium:  code=far, data=near

const cm_t  CM_CC_UNKNOWN  = 0x10;  ///< unknown calling convention
const cm_t  CM_CC_CDECL    = 0x30;  ///< stack
So it is marked as near 16/far 32 pointers and CDECL both of which are correct.

Code:
fi.cc = (fi.cc & ~idaapi.CM_M_MASK) | (idaapi.CM_M_FN if ida_typeinf.is_code_far(fi.cc) else idaapi.CM_M_NN)
tinfo.create_func(fi, ida_typeinf.BT_FUNC | (fi.get_call_method() >> 2))
ida_typeinf.apply_tinfo(ea, tinfo, idaapi.TINFO_DEFINITE)
This works in IDA (creating a new tinfo would crash it because data would be missing). If you use apply_tinfo without creating a newtinfo it does nothing as the fi needs to be rebound to a tinfo to work. If you do not change fi.cc to far code near data, it also works. So the code is correct, can query the type again and it is changed.

Now unfortunately that did not fix the stack of the function nor the callers.

This gets a bit trickier. Intuitively one would think that just using `ida_funcs.reanalyze_function(idaapi.get_func(ea))` would fix the function up immediately. But it does not work and the answer is deeper in IDAs functioning. The tool to create frames lies in the processor module which is not properly exposed to IDAPython. So some manual C tricks will be needed: first get the DLL name, then get the 'ph' processor_t structure (unfortunately the undocumented ida_idp.ph is not helpful here). Of course inspection is always a best first start such as:
Code:
dir(ida_idp)
import inspect
inspect.getmembers(ida_idp)
ida_idp.ph
Anyway, this done manually requires careful adjustment to pad out the not needed fields before the notify callback. IDA 7.x changes things quite a bit not just with 64 bit calling convention but additional members and the pointer size change. Some commented code is left in for those brave enough to try a solution for Linux where theoretically the variadic argument list is a data structure supporting register arguments and quite a bit more complicated. For whatever strange reason, Windows is not following the recommended ABI with va_list but the old model pre-amd64 model where its a mere pointer to the arguments, conveniently making things easier.

Code:
import ctypes
def get_dll():
	# http://www.hexblog.com/?p=695
	idaname = "ida64" if idc.__EA64__ else "ida"
	if sys.platform == "win32":
		return ctypes.windll[idaname + (".dll" if idaapi.IDA_SDK_VERSION >= 700 else ".wll")]
	elif sys.platform == "linux2":
		return ctypes.cdll["lib" + idaname + ".so"]
	elif sys.platform == "darwin":
		return ctypes.cdll["lib" + idaname + ".dylib"]
def get_ph(notif):
#int32 version, id, uint32 flag;
#uint32 flag2; # >= 7.0
#int32 cnbits, dnbits;
#const char *const *psnames, *plnames, const asm_t *const *assemblers; # ptr >= 7.0 is 64-bit
#hook_cb_t *_notify; # >= 7.0 otherwise its just notify(int, ...) instead of (void*, int, va_list/...)
#va_list is just a pointer argument to the variadic list which at invocation time would be difficult to achieve by pushing the next stack address onto the stack as such low level operation probably is not in ctypes
	class processor_t(ctypes.Structure):
		_fields_ = [ ('padding', ctypes.c_int * (8 + 4 if idaapi.IDA_SDK_VERSION >= 700 else 0)),
					 ('notify', notif), ]
	# The exported 'ph' global is the processor_t of the current proc module
	return processor_t.in_dll(get_dll(), 'ph')

def ph_notify(msgid, *args):
  if idaapi.IDA_SDK_VERSION >= 700:
	class VA_LIST(ctypes.Structure):
	  _fields_ = [("arg_" + str(i), args[i].__class__) for i in range(len(args))]
	va_list = VA_LIST(*args)
	return get_ph(ctypes.WINFUNCTYPE(ctypes.c_size_t, ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p)).notify(None, msgid, ctypes.pointer(va_list))
  else:
	return get_ph(ctypes.WINFUNCTYPE(ctypes.c_size_t, ctypes.c_int, [i.__class__ for i in args])).notify(msgid, *args)

#idaman ssize_t ida_export invoke_callbacks(hook_type_t hook_type, int notification_code, va_list va);
def invoke_callbacks(msgid, *args):
  HT_IDP = 0 #Hook to the processor module.
#AMD64 ABI va_list: unsigned int gp_offset (max 48=6*8), fp_offset (max 304=6*8+16*16), void* overflow_arg_area, reg_save_area
  #class VA_LIST(ctypes.Structure):
  #    _fields_ = [("gp_offset", ctypes.c_uint), ("fp_offset", ctypes.c_uint),
  #      ("overflow_arg_area", ctypes.c_void_p), ("reg_save_area", ctypes.c_void_p)]
  class VA_OF_LIST(ctypes.Structure):
	  _fields_ = [("arg_" + str(i), args[i].__class__) for i in range(len(args))]
  va_of_list = VA_OF_LIST(*args)
  #va_list = VA_LIST(0, 6*8, ctypes.cast(ctypes.pointer(va_of_list), ctypes.c_void_p), ctypes.cast(ctypes.pointer(va_of_list), ctypes.c_void_p))
  return get_dll().invoke_callbacks(HT_IDP, msgid, ctypes.pointer(va_of_list))
Finally the reward for that mess is that any processor calls are now possible if done carefully:
Code:
#event_t enum
ev_verify_sp = 58
ev_create_func_frame = 60
ev_analyze_prolog = 81
ev_max_ptr_size = 2002
ev_calc_cdecl_purged_bytes = 2008

#ssize_t notify(event_t event_code, ...)
f = idaapi.get_func(ea)
invoke_callbacks(ev_verify_sp, ctypes.c_void_p(int(f.this)))
ph_notify(ev_analyze_prolog, ctypes.c_size_t(ea))
ph_notify(ev_calc_cdecl_purged_bytes, ctypes.c_size_t(ea))
invoke_callbacks(ev_max_ptr_size)
Note that IDA 7.0 has and should use invoke_callbacks as all the wrapper functions do so and not to use ph_notify directly - though there is no reason why it is not possible to do so as it is still exposed especially for processor module development where a custom user defined value is needed. So either one will work.

Finally the reward of all the hard work:
Code:
ida_frame.del_frame(f)
ph_notify(ev_create_func_frame, ctypes.c_void_p(int(f.this)))
The 'int(f.this)' is a tricky point to as it actually yields the true func_t* structure address which is the only one which will work. id(f.this), id(f) are both wrong and yielding the Python pointer value. The string message even misleadingly gives the wrong address. IDA has used the SWIG interface to wrap the objects and the C code here is implemented using the ctypes library. But as an RE the details of the exact mechanisms should be understood or decipherable by compiling small samples and studying disassemblies or even live debugging IDA64.dll.

Now to get this to take place in the callers of the function still the type has stale stack offsets (its separate from the frame):
Code:
ida_typeinf.guess_tinfo(tinfo, ea)
fi.cc = (fi.cc & ~idaapi.CM_M_MASK) | (idaapi.CM_M_FN if ida_typeinf.is_code_far(fi.cc) else idaapi.CM_M_NN)
tinfo.create_func(fi, ida_typeinf.BT_FUNC | (fi.get_call_method() >> 2))
ida_typeinf.apply_tinfo(ea, tinfo, idaapi.TINFO_DEFINITE)

Manually one could enumerate code x-refs to the function and use ida_typeinf.apply_callee_tinfo(callee_ea, tinfo).

Also an alternative to the processor module may be to use ida_typeinf.delete_wrong_frame_info(f, reanalyze_callback) but it requires a callback which takes instructions as an argument.  Its not really intended for a complete refresh.

Anyway, the wrong far data pointer identification was fixed and I have not found any simpler or more straight forward way to do so. Regardless, the techniques and ideas here could be useful beyond such legacy cases. Happy reversing.


import idautils
def get_lib_funcs():
  libfuncs = []
  for segea in Segments():
    for funcea in idautils.Functions(segea, idc.SegEnd(segea)):
      functionName = idc.GetFunctionName(funcea)
      f = idaapi.get_func(funcea)
      fr = ida_frame.get_frame(f)
      tinfo = idaapi.tinfo_t()
      idaapi.get_tinfo2(funcea, tinfo)
      fi = idaapi.func_type_data_t()
      tinfo.get_func_details(fi)
      if (f.flags & idaapi.FUNC_LIB) != 0: libfuncs.append((funcea, functionName, f, fr, fi))
  return libfuncs

def check_fix_all(b):
  res = []
  for (ea, fn, f, fr, fi) in get_lib_funcs():
    r = check_func_frame((ea, fn, f, fr, fi))
    #if r:
	#  print(fn)
    #  idc.AskStr('', '')
    if (r and b): repair_func(ea)
    res.append((fn, r))
  return res

def check_func_frame((ea, fn, f, fr, fi)):
  if fr is None: return False
  #import ida_range
  #r = ida_range.range_t()
  #ida_frame.get_frame_part(r, f, ida_frame.FPC_ARGS)
  #argSize = r.end_ea - r.start_ea
  argSize = fr.get_member(fr.memqty - 1).eoff - ida_frame.frame_off_args(f) if fr.memqty != 0 else 0
  return (fi.stkargs != argSize)
  for i in range(fr.memqty):
    if fr.get_member(i).soff < ida_frame.frame_off_args(f): continue
    xrefs = idaapi.xreflist_t()
    idaapi.build_stkvar_xrefs(xrefs, f, fr.get_member(i))
    for xref in xrefs:
      flags = idc.GetFlags(xref.ea)
      stkvar = 0 if idc.isStkvar0(flags) else 1 if idc.isStkvar1(flags) else None
      if stkvar is None: continue
      ida_ua.decode_insn(xref.ea)
      op = ida_ua.cmd.Operands[stkvar]
      if op.type == ida_ua.o_displ and op.dtype == ida_ua.dt_word and fr.get_member(i).eoff - fr.get_member(i).soff == 4: return True
  return False


import ctypes
def get_dll():
	# http://www.hexblog.com/?p=695
	idaname = "ida64" if idc.__EA64__ else "ida"
	if sys.platform == "win32":
		return ctypes.windll[idaname + (".dll" if idaapi.IDA_SDK_VERSION >= 700 else ".wll")]
	elif sys.platform == "linux2":
		return ctypes.cdll["lib" + idaname + ".so"]
	elif sys.platform == "darwin":
		return ctypes.cdll["lib" + idaname + ".dylib"]
def get_ph(notif):
#int32 version, id, uint32 flag;
#uint32 flag2; # >= 7.0
#int32 cnbits, dnbits;
#const char *const *psnames, *plnames, const asm_t *const *assemblers; # ptr >= 7.0 is 64-bit
#hook_cb_t *_notify; # >= 7.0 otherwise its just notify(int, ...) instead of (void*, int, va_list/...)
#va_list is just a pointer argument to the variadic list which at invocation time would be difficult to achieve by pushing the next stack address onto the stack as such low level operation probably is not in ctypes
	class processor_t(ctypes.Structure):
		_fields_ = [ ('padding', ctypes.c_int * (8 + 4 if idaapi.IDA_SDK_VERSION >= 700 else 0)),
					 ('notify', notif), ]
	# The exported 'ph' global is the processor_t of the current proc module
	return processor_t.in_dll(get_dll(), 'ph')
def ph_notify(msgid, *args):
  if idaapi.IDA_SDK_VERSION >= 700:
	class VA_LIST(ctypes.Structure):
	  _fields_ = [("arg_" + str(i), args[i].__class__) for i in range(len(args))]
	va_list = VA_LIST(*args)
	return get_ph(ctypes.WINFUNCTYPE(ctypes.c_size_t, ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p)).notify(None, msgid, ctypes.pointer(va_list))
  else:
	return get_ph(ctypes.WINFUNCTYPE(ctypes.c_size_t, ctypes.c_int, [i.__class__ for i in args])).notify(msgid, *args)

#idaman ssize_t ida_export invoke_callbacks(hook_type_t hook_type, int notification_code, va_list va);
def invoke_callbacks(msgid, *args):
  HT_IDP = 0 #Hook to the processor module.
#AMD64 ABI va_list: unsigned int gp_offset (max 48=6*8), fp_offset (max 304=6*8+16*16), void* overflow_arg_area, reg_save_area
  #class VA_LIST(ctypes.Structure):
  #    _fields_ = [("gp_offset", ctypes.c_uint), ("fp_offset", ctypes.c_uint),
  #      ("overflow_arg_area", ctypes.c_void_p), ("reg_save_area", ctypes.c_void_p)]
  class VA_OF_LIST(ctypes.Structure):
	  _fields_ = [("arg_" + str(i), args[i].__class__) for i in range(len(args))]
  va_of_list = VA_OF_LIST(*args)
  #va_list = VA_LIST(0, 6*8, ctypes.cast(ctypes.pointer(va_of_list), ctypes.c_void_p), ctypes.cast(ctypes.pointer(va_of_list), ctypes.c_void_p))
  return get_dll().invoke_callbacks(HT_IDP, msgid, ctypes.pointer(va_of_list))


def repair_func(ea):
  tinfo = idaapi.tinfo_t()
  if not idaapi.get_tinfo2(ea, tinfo):
    if not ida_typeinf.guess_tinfo(tinfo, ea): return False
  fi = idaapi.func_type_data_t()
  if not tinfo.get_func_details(fi): return False
  f = ida_funcs.get_func(ea)
  bChanged = False
  if f.is_far() and (fi.get_call_method() == ida_typeinf.FTI_NEARCALL or fi.get_call_method() == ida_typeinf.FTI_DEFCALL and not idaapi.is_code_far(idaapi.get_inf_structure().cc.cm)) or not f.is_far() and (fi.get_call_method() == ida_typeinf.FTI_FARCALL or fi.get_call_method() == ida_typeinf.FTI_DEFCALL and idaapi.is_code_far(idaapi.get_inf_structure().cc.cm)):
    fi.flags &= ~ida_typeinf.FTI_CALLTYPE
    fi.flags |= ida_typeinf.FTI_FARCALL if f.is_far() else ida_typeinf.FTI_NEARCALL
    bChanged = True
  fi.cc = (fi.cc & ~idaapi.CM_M_MASK) | (idaapi.CM_M_FN if ida_typeinf.is_code_far(fi.cc) else idaapi.CM_M_NN)
  bFrameChange = False
  for i in range(fi.size()):
    if fi[i].type.is_funcptr(): pass
    elif fi[i].type.is_ptr():
      ptd = ida_typeinf.ptr_type_data_t()
      fi[i].type.get_ptr_details(ptd)
      if ptd.based_ptr_size == 4:
        ptd.based_ptr_size -= 2
        fi.stkargs -= 2
        newti = idaapi.tinfo_t()
        newti.create_ptr(ptd, ida_typeinf.BT_PTR | (ida_typeinf.BTMT_NEAR if idaapi.is_data_far(idaapi.get_inf_structure().cc.cm) else ida_typeinf.BTMT_DEFPTR))
        fi[i].type = newti
        bFrameChange = True
  #ev_calc_arglocs = 2011
  #ph_notify(ev_calc_arglocs, ctypes.c_void_p(int(fi.this)))
  if bChanged or bFrameChange:
    tinfo.create_func(fi, ida_typeinf.BT_FUNC | (fi.get_call_method() >> 2))
    ida_typeinf.apply_tinfo(ea, tinfo, idaapi.TINFO_DEFINITE)
  if bFrameChange:
    f = idaapi.get_func(ea)
    ida_frame.del_frame(f)
    ev_create_func_frame = 60
    ph_notify(ev_create_func_frame, ctypes.c_void_p(int(f.this)))

*/