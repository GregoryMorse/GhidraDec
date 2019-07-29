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
inf.is_64bit() ? 8 : 2, inf.cc.size_ldbl,                   ph.max_ptr_size(),                  -1, -1, inf.cc.size_e, -1, -1,

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

	std::string getMetaTypeInfo(tinfo_t& ti)
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
		else if (ti.is_enum()) return "uint";
		else return "unknown";
	}
	bool isX86()
	{
		std::string procName = inf.procname;
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
		//if (!bInit && c == inf.cc.cm) return "default"; //&& ccToStr(inf.cc.cm, 0, true) == modelDefault //if this default is not used its meaningless here
		if (!inf.is_32bit() && !inf.is_64bit() && isX86() && ((c & CM_CC_MASK) == CM_CC_STDCALL || (c & CM_CC_MASK) == CM_CC_PASCAL || (c & CM_CC_MASK) == CM_CC_CDECL)) {//16-bit x86 code needs right model
			std::string model;
			if ((c & CM_CC_MASK) == CM_CC_STDCALL || (c & CM_CC_MASK) == CM_CC_PASCAL) model = "__stdcall";
			else model = "__cdecl";
			model += "16"; // && is_code_far(inf.cc.cm)
			model += callMethod == FTI_FARCALL || callMethod == FTI_DEFCALL && is_code_far(c) ? "far" : "near"; //|| (c & (CM_MASK | CM_M_MASK)) == (CM_UNKNOWN | CM_M_NN)
			return model;
		}
		//if (!bInit && (c & CM_CC_MASK) == (inf.cc.cm & CM_CC_MASK)) return "default";
		switch (c & CM_CC_MASK) {
		case CM_CC_FASTCALL: return !inf.is_32bit() && !inf.is_64bit() ? "__regcall" : "__fastcall"; //return "__vectorcall"; - extension to fastcall
		case CM_CC_STDCALL: return "__stdcall";
		case CM_CC_CDECL: return "__cdecl";
		case CM_CC_THISCALL: return "__thiscall";
		case CM_CC_PASCAL: return "__pascal"; //"__stdcall"
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
	std::string IdaCallback::arglocToAddr(argloc_t al, unsigned long long* offset, std::vector<JoinEntryInfo>& joins, bool noResolveReg) {
		if (al.is_ea()) {
			*offset = al.get_ea();
			return "ram";
		} else if (al.is_stkoff()) {
			*offset = al.stkoff();
			return "stack";
		} else if (al.is_scattered()) {
			scattered_aloc_t scat;
			for (int i = 0; i < scat.size(); i++) {
				unsigned long long offs;
				std::vector<JoinEntryInfo> j;
				std::string spc = arglocToAddr(scat[i], &offs, j, false); //all "ram" - certainly join or reg2 would not make sense
				joins.push_back(JoinEntryInfo{ spc, scat[i].off, scat[i].size });
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
			joins.push_back(JoinEntryInfo{ "register", noResolveReg ? -1 : (unsigned long long)regNameToIndexIda(qs1.c_str()), (unsigned long long)ri1.size });
			joins.push_back(JoinEntryInfo{ "register", noResolveReg ? -1 : (unsigned long long)regNameToIndexIda(qs2.c_str()), (unsigned long long)ri2.size });
			return "join";
		} else if (al.is_rrel()) {
			*offset = al.get_rrel().off; //some type of spacebase
			//qstring qs;
			//get_reg_name(&qs, al.get_rrel().reg, size);
			//regNameToIndexIda(ph.reg_names[al.get_rrel().reg]);
			return "ram";
		}
		//al.is_custom() || al.is_badloc();
		*offset = 0;
		return "ram";
	}
#ifdef _DEBUG
	FILE* debugfp;
#endif
	void IdaCallback::launchDecompiler()
	{
#ifdef _DEBUG
		debugfp = qfopen("d:/source/repos/ghidradec/tests/written", "wb");
#endif
		runCommand(di->decCmd, "",
			&di->decompPid, &di->hDecomp, &di->rdHandle, &di->wrHandle,
			true);
	}
	size_t IdaCallback::readDec(void* Buf, size_t MaxCharCount)
	{
		return _read(di->rdHandle, Buf, MaxCharCount);
		//return qpipe_read(di->rdHandle, Buf, MaxCharCount);
	}
	size_t IdaCallback::writeDec(void const* Buf, size_t MaxCharCount)
	{
#ifdef _DEBUG
		qfwrite(debugfp, Buf, MaxCharCount);
		qflush(debugfp);
#endif
		return _write(di->wrHandle, Buf, MaxCharCount);
		//return qpipe_write(di->wrHandle, Buf, MaxCharCount);
	}

	void IdaCallback::terminate()
	{
#ifdef _DEBUG
		qfclose(debugfp);
#endif
		stopDecompilation(di, false, false, false);
	}
	void IdaCallback::getInits(std::vector<InitStateItem>& inits)
	{
		//inf.start_ea == ea; inf.start_ss; inf.start_sp;
		if (ph.has_segregs()) { //initialize segment registers for tracking - vital for x86-16
			//code segment register although a pointless mapping helps track things for Ghidra
			unsigned long long regtrans = (unsigned long long)regNameToIndexIda(ph.reg_names[ph.reg_code_sreg]);
			for (int j = 0; j < get_segm_qty(); j++) {
				segment_t* s = getnseg(j);
				if (s != nullptr) {
					qstring qs;
					if (get_segm_class(&qs, s) != -1 && qs == "CODE") {
						inits.push_back(InitStateItem{ "ram", s->start_ea, "ram", s->end_ea, "register", regtrans, (unsigned long)ph.segreg_size, (unsigned long long)s->sel });
					}
				}
			}
			for (int i = 0; i <= ph.reg_last_sreg - ph.reg_first_sreg; i++) {
				regtrans = (unsigned long long)regNameToIndexIda(ph.reg_names[i + ph.reg_first_sreg]);
				if (regtrans == -1) continue;
				//this is a much more powerful analysis by the processor engine than the default segregs which now are not used
				for (int k = 0; k < sregRanges[i].size(); k++) {
					if (sregRanges[i][k].val == -1) continue;
					//if (out.val == ((1 << (ph.segreg_size * 8)) - 1)) continue;
					inits.push_back(InitStateItem{ "ram", sregRanges[i][k].start_ea, "ram", sregRanges[i][k].end_ea, "register", regtrans, (unsigned long)ph.segreg_size, (unsigned long long)sregRanges[i][k].val });
				}
				//if (s->defsr[i] != -1) {
					//inits.push_back(InitStateItem{ "ram", s->start_ea, "ram", s->end_ea, "register", (unsigned long long)regNameToIndexIda(ph.reg_names[i + ph.reg_first_sreg]), (unsigned long)ph.segreg_size, (unsigned long long)s->defsr[i] });
				//}
			}
		}
	}
	std::string IdaCallback::getPcodeInject(int type, std::string name, std::string base, unsigned long long offset, std::string fixupbase, unsigned long long fixupoffset)
	{
		if (type == DecompInterface::CALLMECHANISM_TYPE) {
			if ((name == "__stdcall16far@@inject_uponreturn" || name == "__cdecl16far@@inject_uponreturn") && base == "ram") {
				sel_t s;
				executeOnMainThread([&s, offset]() { s = getseg(offset)->sel; });
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
	int IdaCallback::getBytes(unsigned char* ptr, int size, std::string base, unsigned long long offset)
	{
		int i = 0;
		if (base == "ram") {
			if (byteCache.size() > CACHECLEANUPAFTER) {
				for (std::map<ea_t, std::pair<unsigned long long, std::vector<unsigned short>>>::iterator it = byteCache.begin(); it != byteCache.end();) {
					if (it->second.first < cacheCount - CACHELIMIT) it = byteCache.erase(it);
					else it++;
				}
			}
			//paging strategy for easy lookup - just like the processor utilizes
			bool bFetch = false;
			unsigned long long page = offset & (~(CACHEPAGESIZE - 1)),
				pgoffs = offset & (CACHEPAGESIZE - 1);
			do {
				if (byteCache.find(page) == byteCache.end()) {
					bFetch = true;
					break;
				}
				page += CACHEPAGESIZE;
			} while (page < offset + size);
			if (bFetch) executeOnMainThread([this, size, offset]() {
					unsigned long long page = offset & (~(CACHEPAGESIZE - 1));
					do {
						for (int i = 0; i < CACHEPAGESIZE; i++) { //i = offset & (CACHEPAGESIZE - 1);
							byteCache[page].second.push_back(is_loaded(page + i) ? get_byte(page + i) : 256);
						}
						page += CACHEPAGESIZE;
					} while (page < offset + size);
				});
			page = offset & (~(CACHEPAGESIZE - 1));
			pgoffs = offset & (CACHEPAGESIZE - 1);
			byteCache[page].first = cacheCount++;
			for (i = 0; i < size; i++) {
				if (pgoffs == CACHEPAGESIZE) {
					page += CACHEPAGESIZE;
					pgoffs = 0;
					byteCache[page].first = cacheCount++;
				}
				if ((byteCache[page].second[pgoffs] & 256) == 0) ptr[i] = (unsigned char)byteCache[page].second[pgoffs];
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
		return get_name(offset).c_str();
	}
	void getMemoryInfo(unsigned long long offset, bool* readonly, bool* volatil)
	{
		*volatil = !is_mapped(offset);
		segment_t* s = getseg(offset);
		//(s->type & SEG_DATA) != 0;
		*readonly = s != nullptr && (s->perm & SEGPERM_WRITE) == 0 && (s->perm & SEGPERM_MAXVAL) != 0;
		xrefblk_t xr;
		if (xr.first_to(offset, XREF_ALL)) {
			do {
				if ((xr.type & XREF_MASK) == dr_W) {
					*readonly = false;
				}
			} while (xr.next_to());
		}
	}
	void IdaCallback::getFuncInfo(std::string base, ea_t offset, func_t* f, std::string & name, FuncProtoInfo & func)
	{
		//name = demangle_name(imports[offset].name.c_str(), MNG_SHORT_FORM).c_str();
		//name = get_name(offset).c_str();
		//get_ea_name(&qs, offset, GN_VISIBLE | GN_DEMANGLED | GN_SHORT);
		name = get_short_name(offset, GN_STRICT).c_str();
		if (name.size() != 0) name = name.substr(0, name.find("(", 0));
		else name = getSymbolName(offset).c_str();
		func.isInline = false;
		bool bFuncTinfo = getFuncTypeInfoByAddr(f != nullptr ? f->start_ea : offset, &func.extraPop, &func.isNoReturn, &func.dotdotdot, &func.hasThis, func.model, &func.retType, func.syminfo);
		func.isNoReturn = (f != nullptr) ? !f->does_return() : (bFuncTinfo ? func.isNoReturn : false);// f->flags& FUNC_NORET ? true : false;
		//(f->flags & FUNC_USERFAR) != 0
		//(f->flags & FUNC_LIB) != 0
		//(f->flags & FUNC_STATICDEF) != 0
		size_t curArgs = func.syminfo.size();
		if (f != nullptr) {
			if (func.model == "unknown") {
				if (f->argsize == 0) {
					func.model = ccToStr(CM_CC_CDECL, f->is_far() ? FTI_FARCALL : FTI_NEARCALL, false);
				} else {
					func.model = ccToStr((inf.cc.cm & CM_CC_MASK) == CM_CC_CDECL ? CM_CC_STDCALL : inf.cc.cm, f->is_far() ? FTI_FARCALL : FTI_NEARCALL, false);
				}
			}
			if (func.extraPop == -1) func.extraPop = (unsigned long long)f->argsize; //type unknown but have frame so now can calculate
			if (f->regargs == nullptr) read_regargs(f); //populates regargs, similar to how get_spd or the like with f specified populate stkpts
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
					if (get_reg_name(&qs, f->regargs[i].reg, size) == -1) offset = -1;  //for example 1 byte of DS register will fail
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
					it = std::find_if(func.syminfo.begin(), func.syminfo.end(), [offset](SymInfo it) { return it.pi.name == "" && it.space == "register" && it.offset == offset; });
				}
				if (it != func.syminfo.end()) {
					if (it->pi.name == "") it->pi.name = nm;
					it->space = "register";
					it->offset = offset;
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
				func.syminfo.push_back(SymInfo{ { nm, typeChain }, "register", offset, size, (int)curArgs++ });
			}
		}
		if (f != nullptr && f->frame != BADNODE) { //f->analyzed_sp()
			struc_t* frame = get_frame(f);
			func.extraPop += get_frame_retsize(f);
			//ea_t firstarg = frame->members[frame->memqty - 1].eoff - f->argsize;
			//unsigned long long offset = frame->members[i].soff - firstarg;
			ea_t firstarg = frame_off_args(f);
			//generalized for any arguments not found in the frame
			for (int i = 0; i < func.syminfo.size(); i++) {
				if (func.syminfo[i].space == "stack") {
					func.syminfo[i].offset += firstarg - frame_off_retaddr(f);
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
					it = std::find_if(func.syminfo.begin(), func.syminfo.end(), [offset](SymInfo it) { return it.pi.name == "" && it.space == "stack" && it.offset == offset; });
				}
				//unsigned long long reloffs = frame->members[i].get_soff() - frame_off_retaddr(f); // - f->frsize - f->frregs; //frame seems to always be relative to return address
				if (it != func.syminfo.end()) {
					if (it->pi.name == "") it->pi.name = nm;
					it->space = "stack";
					it->offset = offset;
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
				func.syminfo.push_back(SymInfo{ {nm, typeChain}, "stack", frame->members[i].get_soff() - frame_off_retaddr(f), get_member_size(&frame->members[i]),
					is_funcarg_off(f, frame->members[i].get_soff()) ? (int)curArgs++ : -1 }); //frame->members[i].get_soff() >= firstarg or f->frsize - f->frregs
			}
		} else {
			unsigned long long retSize = 0;
			if (f != nullptr && f->is_far()) retSize += ph.segreg_size;
			else if (f == nullptr && bFuncTinfo) {
				tinfo_t ti;
				func_type_data_t ftd;
				getFuncByGuess(offset, ti);
				ti.get_func_details(&ftd);
				if (ftd.get_call_method() == FTI_FARCALL || ftd.get_call_method() == FTI_DEFCALL && is_code_far(inf.cc.cm)) retSize += ph.segreg_size;
			}
			//bitrange_t bits;
			//const char* regnm = get_reg_info(ash.a_curip, &bits); //use default processor name
			//reg_info_t ri;
			//parse_reg_name(&ri, regnm == nullptr ? ash.a_curip : regnm);
			//retSize += ri.size;
			retSize += f == nullptr ? inf.is_64bit() ? 8 : (inf.is_32bit() ? 4 : 2) : get_func_bytes(f); //per documentation of get_frame_retsize
			//type arguments need stack adjustment but there is no frame?
			for (int i = 0; i < func.syminfo.size(); i++) {
				if (func.syminfo[i].space == "stack") {
					func.syminfo[i].offset += retSize;
				}
			}
			//only need to figure out return address size and adjust extraPop which depending on type info or not may affect the model
			//it would be really nice to have context for which cref was used!
			if (func.extraPop == -1) {
				std::map<sval_t, int> histogram;
				for (ea_t ea = get_first_cref_to(offset); ea != BADADDR;) {
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
					ea = get_next_cref_to(offset, ea);
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
			//if (ftd.get_call_method() == FTI_FARCALL || ftd.get_call_method() == FTI_DEFCALL && is_code_far(inf.cc.cm)) {
			//if (f->is_far() || imports.find(offset) != imports.end()) {
				//ph.max_ptr_size(); - for some reason is 48 bits on 64-bit systems?
				//ph.get_stkarg_offset();
				//ph.segreg_size
				//syminfo[i].offset += ph.segreg_size + (inf.is_64bit() ? 8 : inf.is_32bit() == 1 ? 4 : 2);
				//} else if (ftd.get_call_method() == FTI_NEARCALL || ftd.get_call_method() == FTI_DEFCALL && !is_code_far(inf.cc.cm)) {
			//} else {
				//syminfo[i].offset += (inf.is_64bit() ? 8 : inf.is_32bit() == 1 ? 4 : 2);
				//} else if (ftd.get_call_method() == FTI_INTCALL) {
				//syminfo[i].offset += ph.segreg_size + (inf.is_64bit() ? 8 : inf.is_32bit() == 1 ? 4 : 2); //far pointer to old stack, e/rflags, error code
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
						"register", (unsigned long long)regNameToIndexIda(f->regvars[i].canon), (unsigned long long)ri.size, -1, RangeInfo{ base, f->regvars[i].start_ea, f->regvars[i].end_ea } });
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
	void IdaCallback::getMappedSymbol(std::string base, unsigned long long offset, MappedSymbolInfo& msi)
	{
		executeOnMainThread([this, &msi, base, offset]() {
			msi.kind = KIND_HOLE;
			if (base == "register") {} else if (base == "ram") {
				if (imports.find(offset) != imports.end()) {
					usedImports[offset] = true;
					//Ghidra currently has 2 bugs one with restoring CS register if not simulating same segment fixups and the other calling back for external ref function info
						//16-bit apps have functions defined for imports also...
					if (isX86() && !inf.is_32bit() && !inf.is_64bit() && get_func(offset) != nullptr) msi.kind = KIND_FUNCTION;
					else
						msi.kind = KIND_EXTERNALREFERENCE;
				} else {
					if (get_func(offset) != nullptr) {
						msi.kind = KIND_FUNCTION;
					} else {
						flags_t fl = get_flags(offset);
						//if (is_func(fl)) msi.kind = KIND_FUNCTION; //function start only
						//if (exists_fixup(offset)) msi.kind = KIND_EXTERNALREFERENCE; //fixups is not really a determining criterion in exported functions
						if (is_data(fl) || is_unknown(fl) && is_mapped(offset)) msi.kind = KIND_DATA;
						else if (has_name(fl) || has_dummy_name(fl)) msi.kind = KIND_LABEL; //dummy name/label for code only is_code(fl)...
						//if (is_unknown(fl) && !is_mapped(offset))
					}
				}
			} else if (base == "stack") {}
			if (msi.kind == KIND_HOLE) {
				//flags_t f = get_flags(offset);
				//no information about a segment should still assume writable
				//volatile for unmapped memory should be based on the life of the system
				//volatile for mapped memory should be based on the life of the program
				//volatile for stack and registers should be based on the life of a function
				//const and unique would not be volatile ever, though a join space could be comprised of members who are volatile
				if (base == "ram") {
					getMemoryInfo(offset, &msi.readonly, &msi.volatil);
				} else {//if (base == "register" || base == "stack") {
					msi.readonly = false;
					msi.volatil = base != "register"; //far more conservative to assume not volatile until figure out how to determine this - perhaps pspec must explicitly define
				}
			} else if (msi.kind == KIND_LABEL) {
				if (base == "ram") {
					getMemoryInfo(offset, &msi.readonly, &msi.volatil);
					msi.name = getSymbolName(offset);
				} else {
					msi.readonly = false;
					msi.volatil = base != "register";
				}
			} else if (msi.kind == KIND_DATA) {
				if (base == "ram") {
					msi.name = lookupDataInfo(offset, &msi.readonly, &msi.volatil, msi.typeChain);
				} else {
					msi.readonly = false;
					msi.volatil = base != "register";
				}
			} else if (msi.kind == KIND_EXTERNALREFERENCE) {
				if (base == "ram") {
					if (imports[offset].name != "") msi.name = imports[offset].name;
					else msi.name = getSymbolName(offset);
				}
				/*fixup_data_t fd;
				get_fixup(&fd, offset);
				qstring buf;
				get_fixup_desc(&buf, offset, fd);
				buf.c_str();*/
			} else if (msi.kind == KIND_FUNCTION) {
				//get_func_name(&name, f->start_ea);
				func_t* f = get_func(offset);
				msi.entryPoint = (f != nullptr) ? f->start_ea : offset;
				if (msi.entryPoint != offset) {
					if (f != nullptr) {
						rangeset_t rangeset;
						if (get_func_ranges(&rangeset, f) != BADADDR) {
							for (rangeset_t::iterator it = rangeset.begin(); it != rangeset.end(); it++) {
								msi.ranges.push_back(RangeInfo{ base, it->start_ea, it->end_ea });
							}
						}
					}
				} else {
					if (f != nullptr) {
						if (offset == f->start_ea && definedFuncs.find(offset) == definedFuncs.end() && imports.find(offset) == imports.end()) usedFuncs[offset] = true;
						//find_func_bounds(f, FIND_FUNC_NORMAL); //size minimally 1 or maximally must be contiguous block from entry point
						//but func_t already contiguous and the tails account for the rest so end_ea and size are a valid way dont need to find this or first from func ranges should do
						//*size = f->size();
						rangeset_t rangeset;
						msi.size = get_func_ranges(&rangeset, f) == BADADDR ? f->size() : rangeset.begin()->size();
					} else msi.size = 1; //Ghidra uses 1 always despite its commentary
					getFuncInfo(base, offset, f, msi.name, msi.func);
					funcProtoInfos[msi.entryPoint] = FuncInfo{ msi.name.c_str(), msi.func.model, msi.func.retType, msi.func.syminfo };
				}
			}
		});
	}
	void IdaCallback::getFuncTypeInfo(tinfo_t & ti, bool paramOnly, unsigned long long* extraPop, bool* isNoReturn, bool* dotdotdot, bool* hasThis, std::string& model, SymInfo* retType, std::vector<SymInfo>& syminfo)
	{
		asize_t retSize = 0;
		func_type_data_t ftd;
		if (ti.is_func() && ti.get_func_details(&ftd)) { //GTD_NO_ARGLOCS
			*dotdotdot = ftd.is_vararg_cc();
			*hasThis = (ftd.get_cc() & CM_CC_MASK) == CM_CC_THISCALL;
			*isNoReturn = (ftd.flags & FTI_NORET) != 0;
			//switch (get_cc(inf.cc.cm)) {
			//switch (get_cc(guess_func_cc(fd, ti.calc_purged_bytes(), CC_CDECL_OK | CC_ALLOW_ARGPERM | CC_ALLOW_REGHOLES | CC_HAS_ELLIPSIS))) {
			model = ccToStr(get_cc(ftd.get_cc()), ftd.get_call_method());
			for (int i = 0; i < ftd.size(); i++) {
				unsigned long long offs;
				//int typ = coreTypeLookup((int)ftd[i].type.get_size(), getMetaTypeInfo(ftd[i].type));
				std::vector<JoinEntryInfo> joins;
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
				syminfo.push_back(SymInfo{ {ftd[i].name.size() == 0 && space == "register" ? "in_" + std::string(qs.c_str()) : ftd[i].name.c_str(), typeChain },
					space, offs, ftd[i].type.get_size(), i, {}, joins });
			}
			retType->space = arglocToAddr(ftd.retloc, &retType->offset, retType->joins, paramOnly);
			retType->size = ftd.rettype.get_size();
			if (retType->size == BADSIZE) retType->size = 0; //0 is void
			//qstring qs;
			//ftd.rettype.get_type_name(&qs);
			getType(ftd.rettype, retType->pi.ti, false);
			*extraPop = ti.calc_purged_bytes();
			//ti.is_vararg_cc();
			//local variables still need to be processed
		} else {
			*extraPop = -1; //need size of return address which is also popped
			argloc_t al;
			const tinfo_t t(inf.is_64bit() ? BT_INT64 : (inf.is_32bit() ? BT_INT32 : BT_INT16)); //or use BT_INT8 - 1 byte like Ghidra?
#ifdef __X64__
			if (ph.calc_retloc(&al, t, inf.cc.cm) == 1)
#else
			if (ph.notify(ph.calc_retloc3, &t, inf.cc.cm, &al) == 2)
#endif
				retType->space = arglocToAddr(al, &retType->offset, retType->joins, false);
			else {
				retType->space = "register"; // "ram";
				qstring qs;
				get_reg_name(&qs, 0, t.get_size());
				retType->offset = regNameToIndexIda(qs.empty() ? ph.reg_names[0] : qs.c_str());
				retType->size = t.get_size();
			}
			std::string metaType = "unknown"; //still unknown really
			int typ = DecompInterface::coreTypeLookup(retType->size, metaType);
			retType->pi.ti.push_back(TypeInfo{ typ == -1 ? "undefined" : defaultCoreTypes[typ].name.c_str() , retType->size, metaType });
			coreTypeUsed[typ == -1 ? 1 : typ] = true;
			model = "unknown";
		}
	}
	bool IdaCallback::getFuncTypeInfoByAddr(ea_t ea, unsigned long long* extraPop, bool* isNoReturn, bool* dotdotdot, bool* hasThis, std::string& model, SymInfo* retType, std::vector<SymInfo>& syminfo)
	{
		tinfo_t ti;
		bool bFuncTinfo = getFuncByGuess(ea, ti);
		getFuncTypeInfo(ti, false, extraPop, isNoReturn, dotdotdot, hasThis, model, retType, syminfo);
		return bFuncTinfo;
	}
	bool IdaCallback::checkPointer(unsigned long long offset, std::vector<TypeInfo>& typeChain, std::vector<ea_t> & deps)
	{
		//ea_t ea = offset;
		//deref_ptr2(get_idati(), &ea, nullptr);
		tinfo_t ti;
		if (exists_fixup(offset)) {//need to detect pointers
			fixup_data_t fd;
			get_fixup(&fd, offset);
			//uval_t u = get_fixup_value(offset, fd.get_type()); //terminate circular types with pointer chain situation
#ifdef __X64__
			//if (!fd.is_extdef()) return false;
			uval_t u = fd.get_base(); //get_fixup_extdef_ea
#else
			//if ((fd.type & FIXUP_EXTDEF) != 0) return false;
			uval_t u = get_fixup_base(offset, &fd); // (fd.type & FIXUP_REL) != 0 ? 0 : (fd.sel != BADSEL ? sel2ea(fd.sel) : 0); //get_fixup_extdef_ea
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
						unsigned long long ep;
						bool isNoReturn, dotdotdot, hasThis;
						std::string model;
						SymInfo retType;
						std::vector<SymInfo> syminfo;
						getFuncTypeInfo(ti, false, &ep, &isNoReturn, &dotdotdot, &hasThis, model, &retType, syminfo);
						qstring name;
						//get_func_name(&name, f->start_ea);
						name = get_short_name(offset, GN_STRICT);
						if (name.size() != 0) name = name.substr(0, name.find("(", 0));
						else get_name(&name, offset);
						funcProtoInfos[u] = FuncInfo{ name.c_str(), model, retType, syminfo };
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
			unsigned long long size = get_item_size(offset);
			//if (is_unknown(offset)) size = next_head(offset, BADADDR) - offset;
			bool foundPtr = false;
			if ((inf.cc.cm & CM_MASK) == CM_N32_F48 && size == 4) {
				uval_t u;
				if (!get_data_value(&u, offset, size)) return false;
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
							unsigned long long ep;
							bool isNoReturn, dotdotdot, hasThis;
							std::string model;
							SymInfo retType;
							std::vector<SymInfo> syminfo;
							getFuncTypeInfo(ti, false, &ep, &isNoReturn, &dotdotdot, &hasThis, model, &retType, syminfo);
							qstring name;
							//get_func_name(&name, f->start_ea);
							name = get_short_name(offset, GN_STRICT);
							if (name.size() != 0) name = name.substr(0, name.find("(", 0));
							else get_name(&name, offset);
							funcProtoInfos[u] = FuncInfo{ name.c_str(), model, retType, syminfo };
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
		} else if (tc[idx].metaType == "array") {
			unsigned long long elSize = tc[idx + 1].size;
			if (elSize == -1) {
				tinfo_t ti;
				if (ti.get_named_type(get_idati(), tc[idx + 1].typeName.c_str())) {
					elSize = ti.get_size();
				}
			}
			if (tc[idx].size == 0) { //0 length array can only be alone or very last member of structure which recursively chains up to a top structure
				tc[idx].size = get_item_size(ea);
				tc[idx].arraySize = tc[idx].size / elSize;
			} else {
				//could be an array of structures, arrays, or pointers
				if (!is_strlit(get_flags(ea)) || (elSize != sizeof(char) && elSize != sizeof(wchar16_t) && elSize != sizeof(wchar32_t))) { //must prevent auto detection here
					for (int i = 0; i < tc[idx].arraySize; i++) {
						consumeTypeInfo(idx + 1, tc, ea + elSize * i, deps);
					}
				}
			}
		} else if (tc[idx].metaType == "struct") {
			for (int i = 0; i < tc[idx].structMembers.size(); i++) {
				consumeTypeInfo(0, tc[idx].structMembers[i].ti, ea + tc[idx].structMembers[i].offset, deps);
			}
		} else if (is_strlit(get_flags(ea)) && tc[idx].metaType != "array" && (tc[idx].size == sizeof(char) || tc[idx].size == sizeof(wchar16_t) || tc[idx].size == sizeof(wchar32_t)) &&
			get_item_size(ea) % tc[idx].size == 0) {
			//probably need more heuristics than just to verify the type is char, wchar_t or wchar16/32
			tc.insert(tc.begin() + idx, TypeInfo{ "", get_item_size(ea), "array", false, false, false, get_item_size(ea) / tc[idx].size });
			//msg("wrong string is not array\n");
		}
	}
	std::string IdaCallback::lookupDataInfo(unsigned long long offset, bool* readonly, bool* volatil, std::vector<TypeInfo>& typeChain)
	{
		unsigned long long size = (is_unknown(get_flags(offset))) ? calc_max_item_end(offset) - offset : get_item_size(offset);
		tinfo_t ti;
		std::vector<ea_t> deps;
		//is_strlit(get_flags(offset))
		if (!is_unknown(get_flags(offset)) && get_tinfo(&ti, offset)) {
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
			int typ = DecompInterface::coreTypeLookup(size, metaType);
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
		usedData[offset] = typeChain;
		dependentData[offset] = deps;
		getMemoryInfo(offset, readonly, volatil);
		return getSymbolName(offset);
	}

	void IdaCallback::getExternInfo(std::string base, unsigned long long offset, std::string& callName, std::string& modName, FuncProtoInfo& func)
	{
		executeOnMainThread([this, base, offset, &callName, &modName, &func]() {
			//ids folder contains some basic information for common imports and its in comments in the disassembly but how to properly access the info?
			modName = modNames[imports[offset].idx];
			func_t* f = get_func(offset);
			getFuncInfo(base, offset, f, callName, func);
		});
	}
	void IdaCallback::getType(tinfo_t ti, std::vector<TypeInfo>& typeChain, bool bOuterMost)
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
			}
		} 
		typeinf.metaType = getMetaTypeInfo(ti);
		typeinf.size = ti.is_func() || ti.is_funcptr() ? 1 : ti.get_size();
		if (typeinf.size == BADSIZE) typeinf.size = 0; //0 is void
		typeinf.isEnum = ti.is_enum();

		//get_str_type() != -1 && (get_str_type() & STRWIDTH_2B);
		typeinf.isUtf = false;
		typeinf.isChar = ti.is_char();
		qstring qs;
		ti.get_type_name(&qs);
		typeinf.typeName = qs.c_str();
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
			for (int i = 0; i < udt.size(); i++) {
				std::vector<TypeInfo> memberType;
				getType(udt[i].type, memberType, false);
				if (udt[i].type.is_bitfield() || (udt[i].offset & 7) != 0) bHasBitField = true; //mark but scan for better decompiled type prolog
				typeinf.structMembers.push_back(StructMemberInfo{ udt[i].name.c_str(), udt[i].offset / 8, memberType }); //if not byte multiple bits, round down to nearest byte, what to do as Ghidra uses bytes?
			}
			if (udt.is_union || bHasBitField) {
				typeinf.structMembers.clear();
				if (udt.total_size <= 8) {
					typeinf.metaType = "unknown";
				} else {
					typeinf.metaType = "array";
					typeinf.arraySize = udt.total_size;
					typeChain.push_back(TypeInfo{ "byte", 1, "uint" });
				}
			}
			typeChain.push_back(typeinf);
		} else if (typeinf.metaType == "array") {
			array_type_data_t ai;
			ti.get_array_details(&ai);
			typeinf.arraySize = ai.nelems;
			typeChain.push_back(typeinf);
			getType(ai.elem_type, typeChain, false);
		} else if (typeinf.metaType == "code") {
			asize_t retSize = 0;
			func_type_data_t ftd;
			if (ti.is_funcptr()) {
				ptr_type_data_t pt;
				ti.get_ptr_details(&pt);
				ti = pt.obj_type;
			}
			getFuncTypeInfo(ti, true, &typeinf.funcInfo.extraPop, &typeinf.funcInfo.isNoReturn, &typeinf.funcInfo.dotdotdot,
				&typeinf.funcInfo.hasThis, typeinf.funcInfo.model, &typeinf.funcInfo.retType, typeinf.funcInfo.syminfo);
			typeChain.push_back(typeinf);
		} else { //size greater than 16 needs to be converted to array, really should be 8 as no core types exist
			int typ = DecompInterface::coreTypeLookup(typeinf.size, typeinf.metaType);
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
		else if (color == "const") return std::string({ COLOR_ON, COLOR_NUMBER }) + str + std::string({ COLOR_OFF, COLOR_NUMBER });
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
	void IdaCallback::getComments(std::string base, unsigned long long offset, std::vector<CommentInfo>& comments)
	{
		executeOnMainThread([&base, &offset, &comments]() {
			//default options only allow warning, warningheader and user2
			func_t* f = get_func(offset);
			if (f != nullptr) {
				qstring qs;
				if (get_func_cmt(&qs, f, false) != -1)
					comments.push_back(CommentInfo{ base, offset, "warningheader", qs.c_str() });
				if (get_func_cmt(&qs, f, true) != -1)
					comments.push_back(CommentInfo{ base, offset, "warningheader", qs.c_str() });
			}
			rangeset_t rangeset;
			get_func_ranges(&rangeset, f);
			for (rangeset_t::iterator it = rangeset.begin(); it != rangeset.end(); it++) { //there should be a better way to do this besides going through individual instructions?
				for (ea_t offs = it->start_ea; offs < it->end_ea; offs += get_item_size(offs)) { //is_code(offs) should always be true
					//color_t clr; //get_any_indented_cmt is missing from library
					qstring qs;
					if (get_cmt(&qs, offs, false) != -1)
						comments.push_back(CommentInfo{ base, offs, "user1", qs.c_str() });
					if (get_cmt(&qs, offs, true) != -1)
						comments.push_back(CommentInfo{ base, offs, "user1", qs.c_str() });
				}
			}
		});
	}
	std::string IdaCallback::getSymbol(std::string base, unsigned long long offset)
	{
		std::string name;
		if (base == "ram") {
			executeOnMainThread([&name, base, offset]() {
				name = getSymbolName(offset);
			});
		}
		return name;
	}

	std::string IdaCallback::dumpIdaInfo()
	{
		std::string str = "IDA SDK selected decompilation useful reported information dump follows:\n\n";
		str = "Processor: " + std::string(inf.procname) +
			" Is 32-bit: " + std::string(inf.is_32bit() ? "yes" : "no") +
			" Is 64-bit: " + std::string(inf.is_64bit() ? "yes" : "no") +
			" Is big-endian: " + std::string(inf.is_be() ? "yes" : "no") +
			" MaxPtrSize: " + std::to_string(ph.max_ptr_size()) +
			" SegRegSize: " + std::to_string(ph.segreg_size) +
			" SegmBitness: " + std::to_string(ph.get_segm_bitness()) +
			" StkArgOffs: " + std::to_string(ph.get_stkarg_offset()) +
			" Use64: " + std::to_string(ph.use64()) +
			" Use32: " + std::to_string(ph.use32()) +
			" DefCCModl: 0x" + to_string((uint)inf.cc.cm, std::hex) +
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
			getn_selector(&base, &ea, i);
			segment_t* seg = get_segm_by_sel(base);
			qstring qs;
			get_segm_name(&qs, seg);
			str += "Selector Base: 0x" + to_string(base, std::hex) + " Addr: 0x" + to_string(ea, std::hex) + " Seg: " + std::string(qs.c_str()) + "\n";
		}
		if (num != 0) str += "\n";
		num = get_segm_qty();
		str += "Number of segments: " + std::to_string(num) + "\n";
		for (size_t i = 0; i < num; i++) {
			segment_t* seg = getnseg(i);
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
		for (size_t i = 0; i < idati->nbases; i++) {
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

	/*std::string IdaCallback::initForType(tinfo_t ti, ea_t addr)
{
std::string str;
if (ti.is_udt()) {
	udt_type_data_t utd;
	ti.get_udt_details(&utd);
	str += "{ ";
	for (int i = 0; i < utd.size(); i++) {
		if (i != 0) str += ", ";
		str += initForType(utd[i].type, addr + utd[i].offset / 8);
	}
	str += " }";
} else if (is_strlit(get_flags(addr))) { //preempt integer and array handling
	//asize_t len = get_item_size(addr);
	//single byte formats: STRTYPE_TERMCHR, STRTYPE_C, STRTYPE_PASCAL, STRTYPE_LEN2, STRTYPE_LEN4
	//double byte formats: STRTYPE_C_16, STRTYPE_PASCAL_16, STRTYPE_LEN4_16
	//int len = get_max_strlit_length(addr, ti.get_size() == 1 ? STRTYPE_C : (ti.get_size() == 2 ? STRTYPE_C_16 : STRTYPE_C_32), ALOPT_IGNHEADS | ALOPT_IGNPRINT);
	qstring qs;
	get_strlit_contents(&qs, addr, -1, get_str_type(addr), nullptr, STRCONV_ESCAPE); //u8 for UTF-8 with char and u for UTF-16 with char16_t
	str = std::string(ti.get_size() == sizeof(wchar_t) ? "L" : (ti.get_size() == sizeof(char32_t) ? "U" : "")) + "\"" + std::string(qs.c_str()) + "\"";
} else if (ti.is_array()) {
	array_type_data_t atd;
	ti.get_array_details(&atd);
	str += "{ ";
	for (uint32 i = 0; i < atd.nelems; i++) {
		if (i != 0) str += ", ";
		str += initForType(atd.elem_type, addr + atd.elem_type.get_size() * i);
	}
	str += " }";
} else if (ti.is_char() || is_type_int(ti.get_realtype()) && ti.is_signed()) {
	long long num = 0;
	get_bytes(&num, ti.get_size(), addr);
	//heuristics for printing hex vs decimal?
	str = "0x" + to_string(num, std::hex);
} else if (is_type_int(ti.get_realtype())) {
	unsigned long long num = 0;
	get_bytes(&num, ti.get_size(), addr);
	//heuristics for printing hex vs decimal?
	str = "0x" + to_string(num, std::hex);
} else if (ti.is_bool()) {
	str = get_byte(addr) != 0 ? "true" : "false";
} else if (ti.is_floating()) {
	if (ti.is_float()) {
		float f;
		get_bytes(&f, ti.get_size(), addr);
		char buf[33];
		qsnprintf(buf, sizeof(buf), "%.*g", FLT_DECIMAL_DIG, f);
	} else if (ti.is_double()) {
		double f;
		get_bytes(&f, ti.get_size(), addr);
		char buf[33];
		qsnprintf(buf, sizeof(buf), "%.*lg", DBL_DECIMAL_DIG, f);
	}*/ /*else if (ti.is_ldouble()) {
		long double f;
		get_bytes(&f, sz, it->first);
		char buf[33];
		qsnprintf(buf, sizeof(buf), "%.*Lg", LDBL_DECIMAL_DIG, f); LDBL_DECIMAL_DIG = 21 for 80-bit/10-byte?
	}*/
	/*} else if (ti.is_ptr()) {
		uval_t u;
		get_data_value(&u, addr, ti.get_size());
		//tinfo_t tif;
		//get_tinfo(&tif, u);
		qstring qs = get_name(u);
		str = "&" + std::string(qs.c_str());//initForType(tif, addr);
	}
	return str;
}*/

	void checkForwardDecl(tinfo_t ti, const std::map<std::string, bool>& alreadyDefined, std::map<std::string, bool>& needDecl)
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
			if (alreadyDefined.find(qs.c_str()) != alreadyDefined.end()) continue;
			else needDecl[qs.c_str()] = true;
			for (size_t i = 0; i < utd.size(); i++) {
				utd[i].type.get_type_name(&qs);
				if (qs.size() == 0) {
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
			if (!usedT[st]) continue;;
			usedT[st] = false;
			tinfo_t ti;
			ti.get_named_type(get_idati(), st.c_str());
			std::stack<tinfo_t> t;
			t.push(ti);
			bool firstScan = true;
			while (!t.empty()) {
				ti = t.top();
				t.pop();
				qstring qs;
				ti.get_type_name(&qs);
				if (qs.size() != 0 && usedT.find(qs.c_str()) != usedT.end() && !usedT[qs.c_str()] && !firstScan) continue;
				else if (/*!ti.is_anonymous_udt() &&*/ qs.size() != 0) buildDependents(qs.c_str(), usedT, isDependee);
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
				firstScan = false;
			}
		}
	}

	std::string printTypeForCode(const std::vector<TypeInfo>& types, std::string name)
	{
		std::string out;
		std::stack<std::pair<std::string, int>> s;
		s.push(std::pair<std::string, int>(name, 0));
		while (!s.empty()) {
			std::pair<std::string, int> item = s.top();
			s.pop();
			if (item.second < 0) { //post order time where index is complement
				const TypeInfo& ti = types.at(~item.second);
				if (ti.metaType == "ptr") {
					out += std::string(item.first.size() != 0 || ti.isReadOnly ? "* " : "*") + (ti.isReadOnly ? (item.first.size() != 0 ? "const " : "const") : "") + item.first;
				} else if (ti.metaType == "array") {
					out += (item.first.size() != 0 ? " " : "") + item.first + "[" + std::to_string(ti.arraySize) + "]";
				}
				continue;
			}
			const TypeInfo& ti = types.at(item.second);
			if (ti.typeName.size() != 0) out += (ti.isReadOnly ? "const " : "") + ti.typeName + (item.first.size() != 0 ? " " : "") + item.first;
			else if (ti.metaType == "ptr") {
				s.push(std::pair<std::string, int>(item.first, ~item.second));
				s.push(std::pair<std::string, int>("", item.second + 1));
			} else if (ti.metaType == "array") {
				s.push(std::pair<std::string, int>(item.first, ~item.second));
				s.push(std::pair<std::string, int>("", item.second + 1));
			} else if (ti.metaType == "code") {
				std::string str;
				for (size_t i = 0; i < ti.funcInfo.syminfo.size(); i++) {
					if (i != 0) str += ", ";
					str += printTypeForCode(ti.funcInfo.syminfo[i].pi.ti, ti.funcInfo.syminfo[i].pi.name);
				}
				out += printTypeForCode(ti.funcInfo.retType.pi.ti, "") + " (" + (ti.funcInfo.model != "default" && ti.funcInfo.model != "unknown" ? ti.funcInfo.model + " " : "") + "*" + item.first + ")(" + str + ")";
			} else if (ti.metaType == "struct") {
				std::string str;
				for (size_t i = 0; i < ti.structMembers.size(); i++) {
					str += "  " + printTypeForCode(ti.structMembers[i].ti, ti.structMembers[i].name) + ";\n";
				}
				out += "struct {\n" + str + "} " + item.first;
			}
		}
		return out;
	}

	void refTypes(const std::vector<TypeInfo>& types/*tinfo_t ti*/, std::map<std::string, bool>& usedT)
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

	std::string IdaCallback::initForType(const std::vector<TypeInfo>& types/*tinfo_t ti*/, ea_t addr)
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
				str = initForType(nextChain, item.first);
				//msg("%s %s\n", ti.typeName.c_str(), ti.metaType.c_str());
			} else if (ti.metaType == "struct") {
				str = "{ ";
				for (size_t i = 0; i < ti.structMembers.size(); i++) {
					if (i != 0) str += ", ";
					str += initForType(ti.structMembers[i].ti, item.first + ti.structMembers[i].offset);
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
				//} else if (ti.is_array()) {
			} else if (ti.metaType == "array") {
				str = "{ ";
				s.push(std::pair<ea_t, int>(item.first, -1));
				for (uint32 i = ti.arraySize - 1; true; i--) {
					s.push(std::pair<ea_t, int>(item.first + (ea_t)types.at(item.second + 1).size * i, item.second + 1));
					if (i != 0) s.push(std::pair<ea_t, int>(item.first, -2));
					else break;
				}
			} else if (ti.metaType == "char" || ti.metaType == "int" || ti.metaType == "unknown") {
				long long num = 0;
				get_bytes(&num, ti.size, item.first);
				//heuristics for printing hex vs decimal?
				str = "0x" + to_string(num, std::hex);
			} else if (ti.metaType == "uint") {
				unsigned long long num = 0;
				get_bytes(&num, ti.size, item.first);
				//heuristics for printing hex vs decimal?
				str = "0x" + to_string(num, std::hex);
			} else if (ti.metaType == "bool") {
				str = get_byte(item.first) != 0 ? "true" : "false";
			} else if (ti.metaType == "float") {
				if (ti.size == sizeof(float)) {
					float f;
					get_bytes(&f, ti.size, item.first);
					char buf[33];
					qsnprintf(buf, sizeof(buf), "%.*g", FLT_DECIMAL_DIG, f);
					str = buf;
				} else if (ti.size == sizeof(double)) {
					double f;
					get_bytes(&f, ti.size, item.first);
					char buf[33];
					qsnprintf(buf, sizeof(buf), "%.*lg", DBL_DECIMAL_DIG, f);
					str = buf;
				} /*else if (ti.is_ldouble()) {
					long double f;
					get_bytes(&f, sz, it->first);
					char buf[33];
					qsnprintf(buf, sizeof(buf), "%.*Lg", LDBL_DECIMAL_DIG, f); LDBL_DECIMAL_DIG = 21 for 80-bit/10-byte?
				}*/ else {
					msg("%s %s\n", ti.typeName.c_str(), ti.metaType.c_str());
				}
			} else if (ti.metaType == "ptr") { //code would have to be a function pointer
				//} else if (ti.is_ptr()) {
				uval_t u;
				get_data_value(&u, item.first, ti.size);
				//tinfo_t tif;
				//get_tinfo(&tif, u);
				qstring qs = get_name(u);
				str = "&" + std::string(qs.c_str());//initForType(tif, item.first);
			}
			out += str;
		}
		return out;
	}

	//void IdaCallback::dfsVisitType(tinfo_t ti, std::vector<std::string>& sortedTypes, bool firstScan)
	//{
	//	qstring qs;
	//	ti.get_type_name(&qs);
	//	if (qs.size() != 0 && !usedTypes[qs.c_str()] && !firstScan) return;
	//	else if (/*!ti.is_anonymous_udt() &&*/ qs.size() != 0) dfsVisit(qs.c_str(), sortedTypes);
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
		//post order traversal can be considerd to have 3 states - discovered, undiscovered and visited
		if (!unvisited[str]) return;
		s.push(str);
		unvisited[str] = false;
		while (s.size() != 0) {
			std::string v = s.top();
			bool hasUnvisitChdrn = false;
			tinfo_t ti;
			ti.get_named_type(get_idati(), v.c_str());
			std::stack<tinfo_t> t;
			bool firstScan = true;
			t.push(ti);
			while (t.size() != 0) {
				ti = t.top();
				t.pop();
				qstring qs;
				ti.get_type_name(&qs);
				if (qs.size() != 0 && !unvisited[qs.c_str()] && !firstScan) continue;
				else if (/*!ti.is_anonymous_udt() &&*/ qs.size() != 0) {
					if (unvisited[qs.c_str()] && !firstScan) {
						unvisited[qs.c_str()] = false; s.push(qs.c_str()); hasUnvisitChdrn = true;
					}
				}
				if (ti.is_typeref()) {
					qs.clear();
					ti.get_next_type_name(&qs);
					if (qs.size() != 0) {
						if (unvisited[qs.c_str()]) {
							unvisited[qs.c_str()] = false; s.push(qs.c_str()); hasUnvisitChdrn = true;
						}
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
					for (size_t i = 0; i < utd.size(); i++)
						t.push(utd[i].type);
				} else if (ti.is_func()) { //funcptr would be handled from is_ptr
					func_type_data_t ftd;
					ti.get_func_details(&ftd);
					for (size_t j = 0; j < ftd.size(); j++)
						t.push(ftd[j].type);
					t.push(ftd.rettype);
				}
				firstScan = false;
			}
			if (!hasUnvisitChdrn) {
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
		std::map<ea_t, std::vector<TypeInfo>> & usdData,
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
			for (int i = 0; i < vec.size(); i++) {
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

	void checkFwdData(ea_t ea, const std::map<ea_t, std::vector<ea_t>> & depData, 
		const std::map<ea_t, bool>& alreadyDefined, std::map<ea_t, bool>& needDecl)
	{
		std::stack<ea_t> s;
		s.push(ea);
		while (s.size() != 0) {
			ea_t e = s.top();
			s.pop();
			if (alreadyDefined.find(e) != alreadyDefined.end()) continue;
			else needDecl[e] = true;
			const std::vector<ea_t>& vec = depData.at(e);
			for (size_t i = 0; i < vec.size(); i++) {
				s.push(vec[i]);
			}
		}
	}

	std::string IdaCallback::getHeaderDefFromAnalysis(bool allImports)
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

		definitions += "//Default calling convention set to: " + (di->customCallStyle.empty() ? ccToStr(inf.cc.cm, 0, true) : di->customCallStyle) + "\n\n"; //should really use callback interface to get cspec variant

		std::map<ea_t, bool> dataDependee;
		std::map<ea_t, bool> usedRefData;
		std::map<ea_t, bool> usedRefFuncs;
		for (std::map<ea_t, std::vector<TypeInfo>>::iterator it = usedData.begin(); it != usedData.end(); it++) {
			if (finalData.find(get_name(it->first).c_str()) != finalData.end()) {
				addAllDataRefs(it->first, dependentData, usedData, usedRefData, dataDependee, usedRefFuncs);
			}
		}
		for (std::map<ea_t, bool>::iterator it = usedImports.begin(); it != usedImports.end(); it++) {
			qs = get_short_name(it->first, GN_STRICT);
			if (qs.size() != 0) qs = qs.substr(0, qs.find("(", 0));
			else get_name(&qs, it->first);
			if (finalFuncs.find(qs.c_str()) != finalFuncs.end()) usedRefFuncs[it->first] = true;
		}
		for (std::map<ea_t, bool>::iterator it = usedFuncs.begin(); it != usedFuncs.end(); it++) {
			get_long_name(&qs, it->first, GN_STRICT);
			//std::string str = demangle_name(get_name(it->first).c_str(), MNG_LONG_FORM).c_str();
			if (qs.size() == 0) get_name(&qs, it->first);
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
				refTypes(funcProtoInfos[it->first].retType.pi.ti, usedRefTypes);
				for (size_t i = 0; i < funcProtoInfos[it->first].syminfo.size(); i++) {
					refTypes(funcProtoInfos[it->first].syminfo[i].pi.ti, usedRefTypes);
				}
			} //this case no longer can occur
			//get types of return and argument values and add ref to them
		}
		for (std::map<ea_t, bool>::iterator it = usedRefData.begin(); it != usedRefData.end(); it++) {
			refTypes(usedData[it->first], usedRefTypes);
		}

		for (int i = 0; i < numDefCoreTypes; i++) {
			if (usedRefTypes.find(defaultCoreTypes[i].name) != usedRefTypes.end()) {
				coreTypeUsed[i] = true;
				usedRefTypes.erase(defaultCoreTypes[i].name);
			}
			if (coreTypeUsed[i] && szCoreTypeDefs[i] != nullptr) {
				definitions += "typedef " + std::string(szCoreTypeDefs[i]) + " " + defaultCoreTypes[i].name + ";\n";
			}
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
		//print_decls(); //could print all suitable for header file
		for (std::vector<std::string>::iterator it = sortedTypes.begin(); it != sortedTypes.end(); it++) {
			tinfo_t ti;
			ti.get_named_type(get_idati(), it->c_str());
			qstring qt = it->c_str();
			//ti.get_type_name(&qt);
			qstring qs;
			alreadyDefined[*it] = true;
			if (ti.is_udt()) {
				std::map<std::string, bool> notDefined;
				checkForwardDecl(ti, alreadyDefined, notDefined);
				for (std::map<std::string, bool>::iterator iter = notDefined.begin(); iter != notDefined.end(); iter++) {
					tinfo_t t;
					t.get_named_type(get_idati(), iter->first.c_str());
					//t.print(&qs, NULL, PRTYPE_1LINE | PRTYPE_TYPE);
					if (usedRefTypes.find(*it) == usedRefTypes.end()) definitions += "//";
					definitions += std::string(t.is_union() ? "union " : "struct ") + iter->first + ";\n";
				}
			} else if (ti.is_func()) { //ti.is_funcptr()
				//function definitions need the entire definition printed but should be tracked separately
			}
			if (ti.is_forward_decl()) {
				if (usedRefTypes.find(*it) == usedRefTypes.end()) definitions += "//";
				definitions += std::string(ti.is_decl_union() ? "union " : "struct ") + " " + std::string(qt.c_str()) + ";\n";
			} else {
				//ida has a bug where only the first anonymous member of a union is printed anonymously...
				ti.print(&qs, NULL, (ti.is_udt() ? PRTYPE_MULTI : PRTYPE_1LINE) | PRTYPE_DEF | PRTYPE_TYPE | PRTYPE_RESTORE);
				if (usedRefTypes.find(*it) == usedRefTypes.end()) definitions += "/*";
				definitions += std::string((ti.is_udt() ? qs.rtrim('\n') : qs).c_str()) + " " + std::string(qt.c_str()) + ";";
				if (usedRefTypes.find(*it) == usedRefTypes.end()) definitions += "*/";
				definitions += "\n";
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
		if (definitions.size() != 0) definitions += "\n";
		//these are sorted by library and have a comment to remind that libraries are link time imported
		for (size_t i = 0; i < modNames.size(); i++) {
			if (impByMod[i].size() != 0) definitions += "//" + modNames[i] + '\n';
			for (std::vector<ea_t>::iterator it = impByMod[i].begin(); it != impByMod[i].end(); it++) {
				tinfo_t tif;
				qstring qs;
				if (getFuncByGuess(*it, tif)) print_type(&qs, *it, PRTYPE_1LINE);
				if (qs.size() == 0) get_long_name(&qs, *it, GN_STRICT);
				//std::string str = demangle_name(imports[*it].name.c_str(), MNG_LONG_FORM).c_str();
				if (qs.size() == 0) get_name(&qs, *it);
				//str = get_name(*it).c_str();				
				if (qs.find("(", 0) == qstring::npos) qs = qstring("void ") + qs + "(void)";
				//most of the names are not mangled, and thus do not have the needed C declaration information attached
				if (usedRefFuncs.find(*it) == usedRefFuncs.end()) definitions += "//";
				definitions += "extern " + std::string(qs.c_str()) + ";\n";
			}
			if (impByMod[i].size() != 0) definitions += '\n';
		}
		for (std::map<ea_t, bool>::iterator it = usedFuncs.begin(); it != usedFuncs.end(); it++) {
			if (imports.find(it->first) != imports.end()) continue;
			if (usedRefFuncs.find(it->first) == usedRefFuncs.end()) definitions += "//";
			if (funcProtos.find(it->first) != funcProtos.end()) definitions += funcProtos[it->first];
			else if (funcProtoInfos.find(it->first) != funcProtoInfos.end()) {
				//does model go before or after return type - seems to be compiler specific what about C language spec?
				definitions += printTypeForCode(funcProtoInfos[it->first].retType.pi.ti, "") + (funcProtoInfos[it->first].model != "default" && funcProtoInfos[it->first].model != "unknown" ? " " + funcProtoInfos[it->first].model : "") + " " + funcProtoInfos[it->first].name + "(";
				bool bFirst = true;
				for (int i = 0; i < funcProtoInfos[it->first].syminfo.size(); i++) {
					if (funcProtoInfos[it->first].syminfo[i].argIndex != -1) {
						if (!bFirst) definitions += ", ";
						bFirst = false;
						definitions += printTypeForCode(funcProtoInfos[it->first].syminfo[i].pi.ti, funcProtoInfos[it->first].syminfo[i].pi.name);
					}
				}
				definitions += ")";
			} else { //only function pointers currently but after fixed this case should never occur
				tinfo_t tif;
				qstring qs; //first check decompiler prototypes, then type info, then demangler, then void <name>(void)
				if (getFuncByGuess(it->first, tif)) print_type(&qs, it->first, PRTYPE_1LINE);
				if (qs.size() == 0) get_long_name(&qs, it->first, GN_STRICT);
				//std::string str = demangle_name(get_name(it->first).c_str(), MNG_LONG_FORM).c_str();
				if (qs.size() == 0) get_name(&qs, it->first);
				if (qs.find("(", 0) == qstring::npos) qs = qstring("void ") + qs + "(void)";
				//most of the names are not mangled, and thus do not have the needed C declaration information attached
				definitions += qs.c_str();
			}
			definitions += ";\n";
		}
		if (usedFuncs.size() != 0) definitions += "\n";
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
				if (usedRefData.find(*it) == usedRefData.end()) definitions += "//";
				definitions += "extern " + printTypeForCode(usedData[iter->first], std::string(get_name(iter->first).c_str())) + ";\n";
			}
			if (usedRefData.find(*it) == usedRefData.end()) definitions += "//";
			qs = get_name(*it);
			if (qs.size() == 0) {
				std::string str = to_string(*it, std::hex);
				qs = ("ram0x" + std::string(8 - str.size(), '0') + str).c_str();
			}
			definitions += printTypeForCode(usedData[*it], qs.c_str());
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
			if (bInitData) definitions += " = " + initForType(usedData[*it], *it);
			definitions += ";\n";
		}
		if (usedData.size() != 0) definitions += "\n";

		//definitions += '\n';
		return definitions;
	}
	void IdaCallback::analysisDump(std::string& definitions, std::string& idaInfo)
	{
		executeOnMainThread([this, &definitions, &idaInfo]() {
			definitions = getHeaderDefFromAnalysis(di->decompiledFunction == nullptr);
			idaInfo = dumpIdaInfo();
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
		usedImports.clear();
		usedTypes.clear();
		usedData.clear();
		definedFuncs.clear();
		usedFuncs.clear();
		funcProtos.clear();
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
			get_import_module_name(&qs, i);
			modNames.push_back(qs.c_str());
			imp.cur = i;
			//causing Ida Pro to error with internal error 835, 836 - because not in main thread
			enum_import_names(i, &EnumImportNames, &imp);
		}
		for (std::map<ea_t, ImportInfo>::iterator it = imports.begin(); it != imports.end(); it++) {
			importNames[it->second.name] = true;
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
		const char* nm = first_named_type(nullptr, NTF_TYPE);
		while (nm != nullptr) {
			tinfo_t ti;
			ti.get_named_type(nullptr, nm);
			getType(ti, typeDatabase[nm]);
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
				for (int k = 0; k < get_sreg_ranges_qty(i + ph.reg_first_sreg); k++) {
					sreg_range_t out;
					getn_sreg_range(&out, i + ph.reg_first_sreg, k);
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
	std::string IdaCallback::tryDecomp(DecMode dec, ea_t ea, std::string funcName, std::string& display, std::string& err)
	{
		std::string code, funcProto;
		definedFuncs[ea] = true;
		try {
			if (di->decompPid == 0) decInt->registerProgram();
			code = decInt->doDecompile(dec, "ram", ea, display, funcProto);
			if (funcProto.size() != 0) {
				funcProtos[ea] = funcProto;
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
		return code;
	}

	//graph.hpp and a graph emitter should take care of the AST information...

std::string tryDecomp(RdGlobalInfo* di, DecMode dec, ea_t ea, std::string & display, bool & bSucc)
{
	std::string code, err;
	std::string fn = di->idacb->allFuncNames[ea];
	INFO_MSG("Decompiling function: " << fn.c_str() << " @ " << std::hex << ea << std::endl);
	code = di->idacb->tryDecomp(dec, ea, fn.c_str(), display, err);
	if (err.size() != 0) { INFO_MSG(err); }
	else bSucc = true;
	return code;
}

bool detectProcCompiler(RdGlobalInfo* di, std::string& pspec, std::string & cspec, std::string& sleighfilename)
{
	std::vector<int>::iterator it = di->toolMap[inf.procname].begin();
	for (; it != di->toolMap[inf.procname].end(); it++) {
		if ((di->li[*it].size == 32 && inf.is_32bit() && !inf.is_64bit() || di->li[*it].size == 64 && inf.is_64bit() || di->li[*it].size != 32 && di->li[*it].size != 64 && !inf.is_32bit() && !inf.is_64bit()) &&
			di->li[*it].bigEndian == inf.is_be() || di->toolMap[inf.procname].size() == 1) { //should only be one match though, and parameters should always match
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
	if (it == di->toolMap[inf.procname].end()) {
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
	if (di->idacb->decInt == nullptr) di->idacb->decInt = new DecompInterface();
	Options opt = defaultOptions;
	opt.protoEval = di->customCallStyle.empty() ? ccToStr(inf.cc.cm, 0, true) : di->customCallStyle; //default calling convention can be very important such as on x86-16
	opt.decompileUnreachable = (di->alysChecks & 1) != 0;
	opt.ignoreUnimplemented = (di->alysChecks & 2) != 0;
	opt.inferConstPtr = (di->alysChecks & 4) != 0;
	opt.readonly = (di->alysChecks & 8) != 0;
	opt.decompileDoublePrecis = (di->alysChecks & 16) != 0;
	opt.conditionalexe = (di->alysChecks & 32) != 0;
	opt.inPlaceOps = (di->alysChecks & 64) != 0;
	opt.commentIndent = di->cmtLevel;
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
	opt.maxLineWidth = di->maxChars;
	opt.indentIncrement = di->numChars;
	std::vector<CoreType> cts(&defaultCoreTypes[0], &defaultCoreTypes[numDefCoreTypes]);
	try {
		di->idacb->decInt->setup(di->idacb, di->idacb->sleighfilename, di->idacb->pspec, di->idacb->cspec, cts, opt, di->timeout, di->maxPayload);
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
		int successes = 0, total = 0;
		for (size_t i = 0; i < num; i++) {
			std::string disp;
			total++;
			bool bSucc = false;
			code += tryDecomp(di, defaultDecMode, di->idacb->allFuncs[i], disp, bSucc);
			if (di->exiting) return;
			if (bSucc) successes++;
			display += disp;
		}
		INFO_MSG("Decompilation completed: " << successes << " successfully decompiled out of " << total << " (" << (100.0 * (successes / (double)total)) << "%) in " <<
			std::dec << (time(NULL) - startTime) << " seconds\n");
	} else {
		bool bSucc = false;
		code = tryDecomp(di, defaultDecMode, (unsigned long long)di->decompiledFunction->start_ea, display, bSucc);
		if (di->exiting) return;
		INFO_MSG("Decompilation completed: " << di->idacb->allFuncNames[di->decompiledFunction->start_ea] << " in " << std::dec << (time(NULL) - startTime) << " seconds\n");
	}

	if (code.size() == 0) {
		WARNING_GUI("Decompilation FAILED.\n");
		di->decompSuccess = false;
		di->outputFile.clear();
		return;
	}

	std::string definitions, idaInfo;
	di->idacb->analysisDump(definitions, idaInfo);
	code = definitions + code + "\n/*\n" + idaInfo + "\n*/\n";
	display = definitions + display + "\n/*\n" + idaInfo + "\n*/\n";

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
	}

	di->decompSuccess = true;
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