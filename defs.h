/**
 * @file idaplugin/defs.h
 * @brief Plugin-global definitions and includes.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#ifndef IDAPLUGIN_DEFS_H
#define IDAPLUGIN_DEFS_H

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <list>
#include <map>
#include <sstream>

//defined in project for 64 vs 32 bit addressing for ghidradec64.dll vs ghidradec.dll

// IDA SDK includes.
//
#include <ida.hpp> // this must be included before other idasdk headers
#include <auto.hpp>
#include <bytes.hpp>
#include <demangle.hpp>
#include <diskio.hpp>
#include <frame.hpp>
#include <funcs.hpp>
#include <graph.hpp>
#include <idp.hpp>
#include <kernwin.hpp>
#include <lines.hpp>
#include <loader.hpp>
#include <segment.hpp>
#include <strlist.hpp>
#include <struct.hpp>
#include <typeinf.hpp>
#include <ua.hpp>
#include <xref.hpp>

#include "sleighinterface.h"

#if IDA_SDK_VERSION < 750
#define PLUGIN_MULTI 0
#define is_idaqt is_idaq
#define WOPN_DP_TAB WOPN_TAB
#define ACTION_DESC_LITERAL_PLUGMOD(t,u,v,w,x,y,z) ACTION_DESC_LITERAL(t,u,v,x,y,z)
#define inf_filetype inf.filetype
#define inf_start_ea inf.start_ea
#define inf_min_ea inf.min_ea
#define inf_max_ea inf.max_ea
#define inf_procname std::string(inf.procname)
#define inf_is_32bit inf.is_32bit
#define inf_is_64bit inf.is_64bit
#define inf_cc_cm inf.cc.cm
#define inf_is_be inf.is_be
#else
#define PLUGIN_SKIP  0
#define PLUGIN_KEEP 2
inline const char* get_reg_info(const char* regname, bitrange_t* bitrange)
{
	return get_ph()->get_reg_info(regname, bitrange);
}
inline bool is_funcarg_off(const func_t* pfn, uval_t frameoff)
{
	return get_ph()->is_funcarg_off(pfn, frameoff);
}
inline sval_t lvar_off(const func_t* pfn, uval_t frameoff)
{
	return get_ph()->lvar_off(pfn, frameoff);
}
#define ph (*get_ph())
#define is_idaqt is_idaq()
#define inf_filetype inf_get_filetype()
#define inf_start_ea inf_get_start_ea()
#define inf_min_ea inf_get_min_ea()
#define inf_max_ea inf_get_max_ea()
inline std::string qstring_to_string(qstring qs) {
	return std::string(qs.c_str());
}
#define inf_procname qstring_to_string(inf_get_procname())
#define inf_cc_cm inf_get_cc_cm()
#endif

#if !defined(__X64__) || IDA_SDK_VERSION < 720
struct stkpnt_t
{
	ea_t ea;              // linear address
	sval_t spd;           // here we keep a cumulative difference from [BP-frsize]
	bool operator < (const stkpnt_t& r) const { return ea < r.ea; }
};
#endif
#ifndef __X64__

//strlist.hpp missing #pragma pack(pop) and #endif
#include <vector>
#include <entry.hpp>
#include <fixup.hpp>
#include <help.h>

//#define TWidget TWinControl
#define TWidget TCustomControl
#define find_widget(x) find_tform(x)
#define close_widget(x, y) close_tform((Forms::TForm*)x, y)
#define hook_to_notification_point(x, y, z) hook_to_notification_point(x, (hook_cb_t*)y, z)
#define unhook_from_notification_point(x, y) unhook_from_notification_point(x, (hook_cb_t*)y)
#define create_code_viewer(x) create_code_viewer(NULL, x, 0)
#define create_empty_widget(x) (TWidget*)create_tform(x, NULL)
#define create_graph_viewer(u, v, w, x, y, z) create_graph_viewer((Forms::TForm*)z, v, w, x, y)
#define ui_populating_widget_popup ui_populating_tform_popup
#define ask_form AskUsingForm_c
#define ask_file askfile2_c
#define ask_buttons askbuttons_c
#define get_func_name get_func_name2
#define start_ea startEA
#define end_ea endEA
#define min_ea minEA
#define max_ea maxEA
//#define start_ea beginEA
#define rangeset_t areaset_t
#define procname procName
#define regs_num regsNum
#define reg_names regNames
#define reg_code_sreg regCodeSreg
#define is_be() mf
#define max_ptr_size() max_ptr_size
#define get_member_name get_member_name2
#define get_visible_segm_name get_segm_name
#define parse_reg_name(x, y) parse_reg_name(y, x)
#define getn_selector(x, y, z) getn_selector(z, x, y)
#define get_name get_true_name
#define get_tinfo(x, y) get_tinfo2(y, x)
#define guess_tinfo(x, y) guess_tinfo2(y, x)
#define apply_tinfo apply_tinfo2
#define set_node_info set_node_info2
#define add_regarg add_regarg2
#define get_idainfo_by_type(v, w, x, y, z) get_idainfo_by_type3(y, v, w, x, z)
#define define_stkvar add_stkvar2
#define get_data_value(x, y, z) get_data_value(y, x, z)
#define find_udt_member(x, y) find_udt_member(y, x)
#define print_type print_type3
#define is_mapped isEnabled
#define is_loaded isLoaded
#define is_data isData
#define is_align isAlign
#define is_byte isByte
#define is_code isCode
#define is_defarg1 isDefArg1
#define is_dword isDwrd
#define is_head isHead
#define is_oword isOwrd
#define is_pack_real isPackReal
#define is_qword isQwrd
#define is_tbyte isTbyt
#define is_word isWord
#define is_yword isYwrd
#define is_strlit isASCII
#define get_flags get_flags_novalue
#define get_full_flags getFlags
#define get_fixup(x, y) get_fixup(y, x)
#define auto_is_ok autoIsOk
#define save_database save_database_ex
#define BTMT_BOOL8 0
#define NIF_FLAGS 0
#define STRCONV_ESCAPE ACFOPT_ESCAPE
#define PRTYPE_RESTORE 0
#define LP_HIDE_WINDOW 0
#define WOPN_MENU FORM_MENU
#define WOPN_NOT_CLOSED_BY_ESC FORM_NOT_CLOSED_BY_ESC
#define WOPN_TAB FORM_TAB
#define is_int is_type_int
#define apply_cdecl apply_cdecl2
#define demangle_name demangle_name2
#define attach_action_to_popup(x, y, z) attach_action_to_popup((Forms::TForm*)x, y, z)
#define search_path(w, x, y, z) search_path(y, w, x, z)
#define sreg_range_t segreg_area_t
#define reg_first_sreg regFirstSreg
#define reg_last_sreg regLastSreg
#define get_sreg_ranges_qty get_srareas_qty2
#define getn_sreg_range getn_srarea2
#define get_stkarg_offset() get_stkarg_offset2

//-------------------------------------------------------------------------
struct graph_location_info_t
{
	double zoom;          // zoom level, 1.0 == 100%, 0 means auto position
	double orgx;          // graph origin, x coord
	double orgy;          // graph origin, y coord
	graph_location_info_t(void) : zoom(0), orgx(0), orgy(0) {}
	bool operator == (const graph_location_info_t& r) const
	{
		return zoom == r.zoom && orgx == r.orgx && orgy == r.orgy;
	}  //-V550 An odd precise comparison: zoom == r.zoom
	bool operator != (const graph_location_info_t& r) const
	{
		return !(*this == r);
	}
	void serialize(bytevec_t* out) const;
	bool deserialize(const uchar** pptr, const uchar* end);
};

//-------------------------------------------------------------------------
inline void graph_location_info_t::serialize(bytevec_t* out) const
{
	CASSERT(sizeof(graph_location_info_t) == 3 * 8);
	out->append(this, sizeof(graph_location_info_t));
}

//-------------------------------------------------------------------------
inline bool graph_location_info_t::deserialize(const uchar** pptr, const uchar* end)
{
	return unpack_obj(pptr, end, this, sizeof(graph_location_info_t)) != NULL;
}

//-------------------------------------------------------------------------
struct renderer_info_pos_t // out of renderer_info_t, to enable SWiG parsing
{
	int node;
	short cx;
	short cy;

	renderer_info_pos_t() : node(-1), cx(-1), cy(-1) {}
	bool operator == (const renderer_info_pos_t& r) const
	{
		return node == r.node && cx == r.cx && cy == r.cy;
	}
	bool operator != (const renderer_info_pos_t& r) const
	{
		return !(*this == r);
	}
	void serialize(bytevec_t* out) const;
	bool deserialize(const uchar** pptr, const uchar* end);
};

//-------------------------------------------------------------------------
inline void renderer_info_pos_t::serialize(bytevec_t* out) const
{
	append_dd(*out, node);
	append_dw(*out, cx);
	append_dw(*out, cy);
}

//-------------------------------------------------------------------------
inline bool renderer_info_pos_t::deserialize(const uchar** pptr, const uchar* end)
{
	node = unpack_dd(pptr, end);
	cx = unpack_dw(pptr, end);
	if (*pptr >= end)
		return false;
	cy = unpack_dw(pptr, end);
	return true;
}

struct renderer_info_t
{
	renderer_info_t() { clear(); }
	graph_location_info_t gli;
	typedef renderer_info_pos_t pos_t;
	pos_t pos;
	tcc_renderer_type_t rtype;

	bool operator == (const renderer_info_t& r) const
	{
		return rtype == r.rtype && pos == r.pos && gli == r.gli;
	}
	bool operator != (const renderer_info_t& r) const
	{
		return !(*this == r);
	}

	void clear()
	{
		memset(this, 0, sizeof(*this));
		rtype = TCCRT_INVALID;
	}
};

class lochist_t;
struct lochist_entry_t;

#define DEFINE_LOCHIST_T_HELPERS(decl) \
  decl void lochist_t_register_live(lochist_t &);            \
  decl void lochist_t_deregister_live(lochist_t &);          \
  decl bool lochist_t_init     (lochist_t &, const char *, const place_t &, void *, uint32); \
  decl void lochist_t_jump     (lochist_t &, bool try_to_unhide, const lochist_entry_t &e); \
  decl bool lochist_t_fwd      (lochist_t &, uint32 cnt, bool try_to_unhide);   \
  decl bool lochist_t_back     (lochist_t &, uint32 cnt, bool try_to_unhide);   \
  decl bool lochist_t_seek     (lochist_t &, uint32 index, bool try_to_unhide, bool apply_cur); \
  decl const lochist_entry_t *lochist_t_get_current(const lochist_t &);                  \
  decl uint32 lochist_t_current_index(const lochist_t &);        \
  decl void lochist_t_set      (lochist_t &, uint32, const lochist_entry_t &); \
  decl bool lochist_t_get      (lochist_entry_t *, const lochist_t &, uint32); \
  decl uint32 lochist_t_size   (const lochist_t &);\
  decl void lochist_t_save     (const lochist_t &); \
  decl void lochist_t_clear    (lochist_t &);

DECLARE_TYPE_AS_MOVABLE(lochist_t);
struct lochist_entry_t
{
	renderer_info_t rinfo;
	place_t* plce;

	lochist_entry_t() : plce(NULL) {}
	lochist_entry_t(const place_t* p, const renderer_info_t& r)
		: rinfo(r), plce((place_t*)p)
	{
		if (plce != NULL)
			plce = plce->clone();
	}
#ifndef SWIG
	lochist_entry_t(const lochist_t& s);
#endif // SWIG
	lochist_entry_t(const lochist_entry_t& other) : plce(NULL) { *this = other; }
	~lochist_entry_t() { clear(); }
	const renderer_info_t& renderer_info() const { return rinfo; }
	const place_t* place() const { return plce; }

	renderer_info_t& renderer_info() { return rinfo; }
	place_t* place() { return plce; }
	void set_place(const place_t* p) { clear(); plce = p == NULL ? NULL : p->clone(); }
	void set_place(const place_t& p) { set_place(&p); }

	bool is_valid() const { return plce != NULL; }

	lochist_entry_t& operator=(const lochist_entry_t& r)
	{
		clear();
		(*this).rinfo = r.rinfo;
		if (r.plce != NULL)
			plce = r.plce->clone();
		return *this;
	}

	bool operator==(const lochist_entry_t& r) const
	{
		return !is_valid() ? !r.is_valid() : (l_compare(plce, r.plce) == 0 && rinfo == r.rinfo);
	}
	bool operator!=(const lochist_entry_t& r) const
	{
		return !(*this == r);
	}

	void acquire_place(place_t* p)
	{
		clear(); plce = p;
	}

private:
	void clear()
	{
		if (plce != NULL)
			qfree(plce);
	}

	friend class lochist_t;
	DEFINE_LOCHIST_T_HELPERS(friend)
};
DECLARE_TYPE_AS_MOVABLE(lochist_entry_t);

class lochist_t
{
	void* ud;

	DEFINE_LOCHIST_T_HELPERS(friend)

		lochist_entry_t cur;
	netnode node;

#define LHF_HISTORY_DISABLED (1 << 0) // enable history?
	uint32 flags;

public:
	lochist_t() : flags(0) { lochist_t_register_live(*this); }
	~lochist_t() { lochist_t_deregister_live(*this); }
	bool is_history_enabled() const { return (flags & LHF_HISTORY_DISABLED) == 0; }
	int get_place_id() const
	{
		const place_t* p = cur.place();
		return p == NULL ? -1 : p->lnnum; //p->id();
	}
	bool init(const char* stream_name, const place_t* _defpos, void* _ud, uint32 _flags)
	{
		return lochist_t_init(*this, stream_name, *_defpos, _ud, _flags);
	}

	nodeidx_t netcode() const
	{
		return node;
	}

	void jump(bool try_to_unhide, const lochist_entry_t& e)
	{
		lochist_t_jump(*this, try_to_unhide, e);
	}

	uint32 current_index() const
	{
		return lochist_t_current_index(*this);
	}

	bool seek(uint32 index, bool try_to_unhide)
	{
		return lochist_t_seek(*this, index, try_to_unhide, true);
	}

	bool fwd(uint32 cnt, bool try_to_unhide)
	{
		return lochist_t_fwd(*this, cnt, try_to_unhide);
	}

	bool back(uint32 cnt, bool try_to_unhide)
	{
		return lochist_t_back(*this, cnt, try_to_unhide);
	}

	void save() const
	{
		lochist_t_save(*this);
	}

	void clear()
	{
		lochist_t_clear(*this);
	}

	const lochist_entry_t& get_current() const
	{
		return *lochist_t_get_current(*this);
	}

	void set_current(const lochist_entry_t& e)
	{
		return set(current_index(), e);
	}

	void set(uint32 index, const lochist_entry_t& e)
	{
		lochist_t_set(*this, index, e);
	}

	bool get(lochist_entry_t* out, uint32 index) const
	{
		return lochist_t_get(out, *this, index);
	}

	uint32 size(void) const
	{
		return lochist_t_size(*this);
	}

	const place_t* get_template_place() const
	{
		return cur.place();
	}
};

enum locchange_reason_t
{
	lcr_unknown,
	lcr_goto,
	lcr_user_switch, // user pressed <Space>
	lcr_auto_switch, // automatic switch
	lcr_jump,
	lcr_navigate,    // navigate back & forward
	lcr_scroll,      // user used scrollbars
	lcr_internal,    // misc. other reasons
};
#define LCMD_SYNC (1 << 0)
class locchange_md_t // location change metadata
{
protected:
	uchar cb;
	uchar r;
	uchar f;
	uchar reserved;

public:
	locchange_md_t(locchange_reason_t _reason, bool _sync)
		: cb(sizeof(*this)), r(uchar(_reason)), f(_sync ? LCMD_SYNC : 0), reserved(0) {}
	locchange_reason_t reason() const { return locchange_reason_t(r); }
	bool is_sync() const { return (f & LCMD_SYNC) != 0; }
};
CASSERT(sizeof(locchange_md_t) == sizeof(uint32));
DECLARE_TYPE_AS_MOVABLE(locchange_md_t);

typedef void idaapi custom_viewer_adjust_place_t(TWidget* v, lochist_entry_t* loc, void* ud);
typedef int idaapi custom_viewer_get_place_xcoord_t(TWidget* v, const place_t* pline, const place_t* pitem, void* ud);
typedef void idaapi custom_viewer_location_changed_t(
	TWidget* v,
	const lochist_entry_t* was,
	const lochist_entry_t* now,
	const locchange_md_t& md,
	void* ud);
typedef int idaapi custom_viewer_can_navigate_t(
	TWidget* v,
	const lochist_entry_t* now,
	const locchange_md_t& md,
	void* ud);
#ifndef SWIG
// Handlers to be used with create_custom_viewer()
class custom_viewer_handlers_t
{
	int cb;
public:
	custom_viewer_handlers_t(
		custom_viewer_keydown_t* _keyboard = NULL,
		custom_viewer_popup_t* _popup = NULL,
		custom_viewer_mouse_moved_t* _mouse_moved = NULL,
		custom_viewer_click_t* _click = NULL,
		custom_viewer_dblclick_t* _dblclick = NULL,
		custom_viewer_curpos_t* _curpos = NULL,
		custom_viewer_close_t* _close = NULL,
		custom_viewer_help_t* _help = NULL,
		custom_viewer_adjust_place_t* _adjust_place = NULL,
		custom_viewer_get_place_xcoord_t* _get_place_xcoord = NULL,
		custom_viewer_location_changed_t* _location_changed = NULL,
		custom_viewer_can_navigate_t* _can_navigate = NULL)
		: cb(sizeof(*this)),
		keyboard(_keyboard),
		popup(_popup),
		mouse_moved(_mouse_moved),
		click(_click),
		dblclick(_dblclick),
		curpos(_curpos),
		close(_close),
		help(_help),
		adjust_place(_adjust_place),
		get_place_xcoord(_get_place_xcoord),
		location_changed(_location_changed),
		can_navigate(_can_navigate)
	{}
	custom_viewer_keydown_t* keyboard;
	custom_viewer_popup_t* popup;
	custom_viewer_mouse_moved_t* mouse_moved;
	custom_viewer_click_t* click;
	custom_viewer_dblclick_t* dblclick;
	custom_viewer_curpos_t* curpos;
	custom_viewer_close_t* close;
	custom_viewer_help_t* help;
	custom_viewer_adjust_place_t* adjust_place;
	custom_viewer_get_place_xcoord_t* get_place_xcoord;
	custom_viewer_location_changed_t* location_changed;
	custom_viewer_can_navigate_t* can_navigate;
};
#endif // SWIG
enum path_type_t
{
	PATH_TYPE_CMD,  ///< full path to the file specified in the command line
	PATH_TYPE_IDB,  ///< full path of IDB file
	PATH_TYPE_ID0,  ///< full path of ID0 file
};
inline const char* get_path(path_type_t pt)
{
	if (pt == PATH_TYPE_CMD) return command_line_file;
	else if (pt == PATH_TYPE_IDB) return database_idb;
	else if (pt == PATH_TYPE_ID0) return database_id0;
	else return nullptr;
}
inline TWidget* create_custom_viewer(
	const char* title,
	const place_t* minplace,
	const place_t* maxplace,
	const place_t* curplace,
	const renderer_info_t* rinfo,
	void* ud,
	const custom_viewer_handlers_t* cvhandlers,
	void* cvhandlers_ud,
	TWidget* parent = NULL)
{
	//some cvhandlers did not exist so they are not registered
	TCustomControl* widget = create_custom_viewer(title, (Controls::TWinControl*)parent, minplace, maxplace, curplace, rinfo == nullptr ? 0 : (int)rinfo->pos.cy, ud);
	set_custom_viewer_handlers(widget, cvhandlers->keyboard, cvhandlers->popup, cvhandlers->mouse_moved,
		cvhandlers->dblclick, cvhandlers->curpos, cvhandlers->close, cvhandlers_ud);
	return (TWidget*)widget;
}

inline void display_widget(TWidget* widget, int options)
{
	return open_tform((Forms::TForm*)widget, options);
}
inline bool print_insn_mnem(qstring* out, ea_t ea)
{
	char b[MAXSTR];
	if (ua_mnem(ea, b, MAXSTR) != nullptr) {
		*out = b;
		return true;
	}
	return false;
}
AS_PRINTF(3, 4) inline bool ask_str(qstring* str, int hist, const char* format, ...)
{
	va_list va;
	va_start(va, format);
	bool result = vaskqstr(str, format, va);
	va_end(va);
	return result;
}
AS_PRINTF(2, 3) inline int ask_yn(int deflt, const char* format, ...)
{
	va_list va;
	va_start(va, format);
	int code = askyn_cv(deflt, format, va);
	va_end(va);
	return code;
}
inline ea_t get_func_ranges(rangeset_t* ranges, func_t* pfn)
{
	area_t limits;
	if (get_func_limits(pfn, &limits)) {
		ranges->add(pfn->startEA, pfn->endEA);
		for (int i = 0; i < pfn->tailqty; i++)
			ranges->add(pfn->tails[i]);
		//ranges->add(limits);
		return limits.endEA;
	} else return BADADDR;
}
inline bool get_highlight(qstring* out_str, TWidget* viewer, uint32* out_flags)
{
	if (get_current_viewer() == viewer) {
		char b[MAXSTR];
		bool res = get_highlighted_identifier(b, MAXSTR, 0); //*out_flags
		if (res) *out_str = b;
		return res;
	} else return false;
}
inline const char* get_reg_info(const char* regname, bitrange_t* bitrange)
{
	return get_reg_info2(regname, bitrange);
}
/// Demangle a name.
inline qstring idaapi demangle_name(
	const char* name,
	uint32 disable_mask,
	demreq_type_t demreq = DQT_FULL)
{
	qstring out;
	demangle_name(&out, name, disable_mask, demreq);
	return out;
}
inline bool is_func(flags_t F)
{
	return isFunc(F);
}
AS_PRINTF(4, 5) inline bool ask_text(
	qstring* answer,
	size_t max_size,
	const char* defval,
	const char* format,
	...)
{
	va_list va;
	std::vector<char> buf;
	buf.resize(max_size);
	va_start(va, format);
	bool result = vasktext(max_size, buf.data(), defval, format, va);
	va_end(va);
	if (result)* answer = buf.data();
	return result;
}
typedef uint16 fixup_type_t;
inline int calc_fixup_size(fixup_type_t type) //ida64.dll v7.x - derived from table in disassembled listing
{
//#define FIXUP_CUSTOM    0xF     ///< fixup is processed by processor module
	//cannot get size of custom fixups in SDK so return -1 - missing get_fixup_handler() - perhaps netnode
	const int fixups[] = { 1, 2, 2, 4, 4, 6, 1, 2, 1, 2, 4, 4, 8, -1, -1, -1 };
	//const int fixups[] = { 8, 16, 16, 32, 32, 48, 8, 16, 8, 16, ph.high_fixup_bits, 32 - ph.high_fixup_bits, 64, -1, -1, -1 };
	return fixups[type & FIXUP_MASK];
}
inline uval_t get_fixup_value(ea_t ea, fixup_type_t type) //ida64.dll v7.x - derived from table in disassembled listing
{
	//need to somehow validate these are correct
	//fixup_data_t fd = get_fixup(type, ea);
	//fd.off; fd.sel; fd.displacement;
	//sel2ea = get_segm_by_sel()->startEA
	switch (type & FIXUP_MASK) {
	case FIXUP_OFF8:
		return (char)get_byte(ea);
	case FIXUP_OFF16:
		return (short)get_word(ea);
	case FIXUP_SEG16:
		return sel2ea(get_word(ea)) * 16;
	case FIXUP_PTR16:
		return sel2ea(get_word(ea + 2)) * 16 + get_word(ea);
	case FIXUP_OFF32:
		return (long)get_long(ea);
	case FIXUP_PTR32:
		return sel2ea(get_word(ea + 4)) * 16 + get_long(ea);
	case FIXUP_HI8:
		return get_byte(ea) << 8;
	case FIXUP_HI16:
		return get_word(ea) << 16;
	case FIXUP_LOW8:
		return get_byte(ea);
	case FIXUP_LOW16:
		return get_word(ea);
	case FIXUP_VHIGH:
		return (get_long(ea) & ((1 << (32 - ph.high_fixup_bits)) - 1)) << ph.high_fixup_bits;
	case FIXUP_VLOW:
		return get_long(ea) & ((1 << ph.high_fixup_bits) - 1);
	case FIXUP_OFF64:
		return (uval_t)get_qword(ea);
	case FIXUP_CUSTOM:
		break; //no get_fixup_handler() function so no way to handle easily - perhaps netnode
	}
	return 0;
}
inline const char* get_fixup_desc(
	qstring* buf,
	ea_t source,
	const fixup_data_t& fd)
{
	char b[MAXSTR];
	char* res = get_fixup_desc(source, &fd, b, MAXSTR);
	if (res != nullptr)* buf = b;
	return res;
}
inline ssize_t get_segm_name(qstring* buf, const segment_t* s, int flags = 0)
{
	char b[MAXSTR];
	ssize_t res = get_true_segm_name(s, b, MAXSTR);
	*buf = b;
	return res;
}
inline int32 get_idasgn_desc(
	qstring* signame,
	qstring* optlibs,
	int n)
{
	char b[MAXSTR], o[MAXSTR];
	int32 res = get_idasgn_desc(n, b, MAXSTR, o, MAXSTR);
	if (res != 0) {
		*signame = b;
		*optlibs = o;
	}
	return res;
}
#include <locale>
#include <codecvt>
inline ssize_t get_strlit_contents(
	qstring* utf8,
	ea_t ea,
	size_t len,
	int32 type,
	size_t* maxcps = NULL,
	int flags = 0)
{
	if (len == -1) {
		if (is_strlit(get_flags(ea)))
			len = (size_t)get_item_size(ea) * bytesize(ea);
		else
			len = get_max_ascii_length(ea, ACFOPT_UTF8, ALOPT_IGNHEADS);
	}
	std::vector<char> b;
	b.resize(len);
	size_t sz = 0;
	bool res = get_ascii_contents2(ea, len, type, b.data(), len, &sz, ACFOPT_UTF8 | ((flags & STRCONV_ESCAPE) != 0 ? ACFOPT_ESCAPE : 0)); //0 for truncated
	//flags == STRCONV_REPLCHAR;
	//flags == STRCONV_INCLLEN;
	*utf8 = qstring(b.data(), b.size());
	if (maxcps) {
#if _MSC_VER >= 1900
		*maxcps = std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t>().from_bytes(utf8->c_str()).size();
#else
		*maxcps = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(utf8->c_str()).size();
#endif
	}
	return sz;
}
inline ssize_t get_entry_name(qstring* buf, uval_t ord)
{
	char b[MAXSTR];
	ssize_t res = get_entry_name(ord, b, MAXSTR);
	if (res != 0) *buf = b;
	return res;
}
inline ssize_t get_reg_name(qstring* buf, int reg, size_t width, int reghi = -1)
{
	char b[MAXSTR];
	ssize_t res = get_reg_name(reg, width, b, MAXSTR, reghi);
	if (res != 0) *buf = b;
	return res;
}
inline ssize_t get_bytes(
	void* buf,
	ssize_t size,
	ea_t ea,
	int gmb_flags = 0,
	void* mask = NULL)
{
	return get_many_bytes(ea, buf, size) ? size : 0;
}
inline bool get_import_module_name(qstring* buf, int mod_index)
{
	char b[MAXSTR];
	bool res = get_import_module_name(mod_index, b, MAXSTR);
	if (res)* buf = b;
	return res;
}
inline ssize_t tag_remove(qstring* buf, const qstring& str, int init_level = 0)
{
	char b[MAXSTR];
	return tag_remove(str.c_str(), b, MAXSTR);
}
inline ssize_t tag_remove(qstring* buf, int init_level = 0)
{
	if (buf->empty())
		return 0;
	return tag_remove(buf, buf->begin(), init_level);
}
inline ssize_t get_idasgn_title(
	qstring* buf,
	const char* name)
{
	char b[MAXSTR];
	char* res = get_idasgn_title(name, b, MAXSTR);
	if (res != nullptr) {
		*buf = b;
		return buf->size();
	} else return 0;
}
inline ssize_t get_func_cmt(qstring* buf, const func_t* pfn, bool repeatable)
{
	char* ch = get_func_cmt((func_t*)pfn, repeatable);
	if (ch != nullptr) {
		*buf = ch;
		qfree(ch);
		return buf->size();
	} else return 0;
}
inline ssize_t get_segment_cmt(qstring* buf, const segment_t* s, bool repeatable)
{
	char* ch = get_segment_cmt(s, repeatable);
	if (ch != nullptr) {
		*buf = ch;
		qfree(ch);
		return buf->size();
	} else return 0;
}
inline ssize_t get_segm_class(qstring* buf, const segment_t* s)
{
	char b[MAXSTR];
	ssize_t res = get_segm_class(s, b, MAXSTR);
	if (res != 0) *buf = b;
	return res;
}
inline bool exists_fixup(ea_t source)
{
	fixup_data_t fd;
	return get_fixup(&fd, source); //reversed arguments due to macro above
}
inline const til_t* get_idati(void)
{
	return idati;
}
inline ssize_t netnode_qvalstr(nodeidx_t num, qstring* buf)
{
	char b[MAXSTR];
	ssize_t res = netnode_valstr(num, b, MAXSTR);
	if (res) *buf = b;
	return res;
}
inline ssize_t get_cmt(qstring* buf, ea_t ea, bool rptble)
{
	char b[MAXSTR];
	ssize_t res = get_cmt(ea, rptble, b, MAXSTR);
	if (res)* buf = b;
	return res;
}
inline bool is_custom(flags_t F) { return isCustom(F); }
inline bool is_double(flags_t F) { return isDouble(F); }
inline bool is_float(flags_t F) { return isFloat(F); }
inline bool is_unknown(flags_t F) { return isUnknown(F); }
inline bool is_struct(flags_t F) { return isStruct(F); }
#endif
#if IDA_SDK_VERSION < 720
#define TXTF_LINENUMBERS 0
inline bool is_anonymous_udt(const tinfo_t& ti)
{
	//taken from ida64.dll get_property_tinfo GTA_IS_ANON_UDT=282=0x11a
	if (ti.is_enum()) return false; //also not reserved
	if (ti.is_bitfield()) return false;
	qstring qs;
	ti.get_type_name(&qs);
	//ti.get_ordinal() / get_numbered_type_name / create_numbered_type_name
	if (qs.size() == 0) return false;
	if (qs[0] == '$') return true;
	const char* ptr = strrchr(qs.c_str(), ':');
	if (ptr == nullptr) return false;
	return (ptr[1] == '$');
}
#else
inline bool is_anonymous_udt(const tinfo_t& ti)
{
	return ti.is_anonymous_udt();
}
#endif

// RetDec includes.
//
#include "retdec/config/config.h"
#include "retdec/utils/address.h"
#include "retdec/utils/filesystem_path.h"
#include "retdec/utils/os.h"
#include "retdec/utils/time.h"

namespace idaplugin {

// idaapi is defined by IDA SDK, but if we do this, eclipse won't complain
// even if it do not find the definition.
//
#ifndef idaapi
	#define idaapi
#endif

// General print msg macros.
//
#ifdef _DEBUG
#define PRINT_DEBUG   true
#else
#define PRINT_DEBUG   false
#endif
#define PRINT_ERROR   false
#define PRINT_WARNING true
#define PRINT_INFO    true

#define DBG_MSG(body)     if (PRINT_DEBUG)   { std::stringstream ss; ss << std::showbase << body; msg("%s", ss.str().c_str()); }
/// Use this only for non-critical error messages.
#define ERROR_MSG(body)   if (PRINT_ERROR)   { std::stringstream ss; ss << std::showbase << "[GhidraDec error]  :\t" << body; msg("%s", ss.str().c_str()); }
/// Use this only for user info warnings.
#define WARNING_MSG(body) if (PRINT_WARNING) { std::stringstream ss; ss << std::showbase << "[GhidraDec warning]:\t" << body; msg("%s", ss.str().c_str()); }
/// Use this to inform user.
#define INFO_MSG(body)    if (PRINT_INFO)    { std::stringstream ss; ss << std::showbase << "[GhidraDec info]   :\t" << body; msg("%s", ss.str().c_str()); }

/// Use instead of IDA SDK's warning() function.
#define WARNING_GUI(body) { std::stringstream ss; ss << std::showbase << body; warning("%s", ss.str().c_str()); }

#define RELEASE_VERSION "1.4"

class FunctionInfo
{
	public:
		std::string code;
		std::vector<std::tuple<std::vector<unsigned int>, std::string, unsigned int>> blockGraph;
		strvec_t idaCode;
};

// Helper functions.
//
int runCommand(
		const std::string& cmd,
		const std::string& args,
		intptr_t* pid = nullptr,
		void** hdl = nullptr,
		int* readpipe = nullptr,
		int* writepipe = nullptr,
		bool showWarnings = false);

class RdGlobalInfo;
void stopDecompilation(RdGlobalInfo*, bool, bool, bool);

#define PLUGIN_NAME "Ghidra Decompiler Plugin"
#define PLUGIN_PRODUCER "Gregory Morse"
#define PLUGIN_COPYRIGHT "Copyright 2019 " PLUGIN_PRODUCER
#define PLUGIN_URL "https://forum.exetools.com/"
#define PLUGIN_HOTKEY "Ctrl-g"

class GhidraDec;
class IdaCallback;
/**
 * General information used by this plugin.
 */
class RdGlobalInfo
{
	public:
		RdGlobalInfo(GhidraDec* pm);

	// General plugin information.
	//
	public:
		GhidraDec* pm;
		std::string pluginName             = PLUGIN_NAME;
		std::string pluginID               = "ghidra.decompiler";
		std::string pluginProducer         = PLUGIN_PRODUCER;
		std::string pluginCopyright        = PLUGIN_COPYRIGHT;
		std::string pluginEmail            = "gregory.morse@live.com";
		std::string pluginURL              = PLUGIN_URL;
		std::string pluginContact          = PLUGIN_URL "\nEMAIL: " + pluginEmail;
		std::string pluginVersion          = RELEASE_VERSION;
		std::string pluginHotkey           = PLUGIN_HOTKEY;
		std::string pluginBuildDate        = retdec::utils::getCurrentDate();
		addon_info_t pluginInfo; ///< Plugin (addon) information showed in the About box.
		int pluginRegNumber         = -1;

	// General information common for all decompilations or viewers.
	//
	public:
		std::string workDir;
		std::string workIdb;
		std::string inputPath;
		std::string inputName;
		/// Retargetable decompilation DB file name.
		std::string dbFile;
		retdec::config::Config configDB;
		//std::string mode;
		//std::string architecture;
		//std::string endian;
		//retdec::utils::Address rawEntryPoint;
		//retdec::utils::Address rawSectionVma;

		std::map<func_t*, FunctionInfo> fnc2code;
		std::list<func_t*> navigationList;
		std::list<func_t*>::iterator navigationActual = navigationList.end();

	// One viewer information.
	//
	public:
		const std::string viewerName = "GhidraDec";
		TWidget* custViewer = nullptr;
		TWidget* codeViewer = nullptr;
		TWidget* graphWidget = nullptr;
		graph_viewer_t* graphViewer = nullptr;
		mutable_graph_t* mg = nullptr;
		std::vector<std::string> graphText;

	// One decompilation information.
	//
	public:
		bool isAllDecompilation();
		bool isSelectiveDecompilation();

	public:
		std::string decCmd;
		std::string ranges;
		std::string outputFile;
		bool exiting = false;
		bool decompRunning          = false;
		bool decompSuccess          = false;
		bool decompiledAll          = false;
		IdaCallback* idacb = nullptr;
		int uiExecutingTask = -1;
		qsemaphore_t termSem = nullptr;
		qmutex_t qm = nullptr;
		qthread_t decompThread      = nullptr;
		func_t *decompiledFunction  = nullptr;
		// PID/Handle of launched decompilation process.
		intptr_t decompPid = 0;
		int rdHandle = -1; //qhandle_t
		int wrHandle = -1; //qhandle_t
		void* hDecomp = nullptr;

	// Plugin configuration information.
	//
	public:
		bool isDecompilerInSpecifiedPath() const;
		bool isDecompilerInSystemPath();

		bool configureDecompilation();

		bool isUseThreads() const;
		void setIsUseThreads(bool f);

	public:
#if defined(OS_WINDOWS)
		const std::string decompilerExeName = "decompile.exe";
#else
		const std::string decompilerExeName = "decompile";
#endif
		const std::string pluginConfigFileName = "ghidradec-config.json";
		retdec::utils::FilesystemPath pluginConfigFile;
		std::string decompilerExePath = "";
		/// Path to the Ghidra base folder set by user in configuration menu.
		std::string ghidraPath = "";
		sval_t cacheSize = 10, maxPayload = 50, timeout = 30, cmtLevel = 20, maxChars = 100, numChars = 2;
		int comStyle = 0, intFormat = 2;
		ushort viewFeatures = 1 | 2;
		ushort alysChecks = 1 | 4 | 8 | 16 | 32, dispChecks = 4 | 8 | 128 | 256 | 1024;

		std::vector<LangInfo> li;
		std::map<std::string, std::vector<int>> toolMap; //not safe to make a std::map which allocates memory inside global data structure of DLL since C runtime may not be initialized

		std::string customCallStyle;
		std::string customCspec;
		std::string customPspec;
		std::string customSlafile;

	private:
		/// Only for debugging during development.
		bool useThreads = true;
};

} // namespace idaplugin

#endif
