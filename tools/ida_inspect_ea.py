import json
import sys
from pathlib import Path

import ida_auto
import ida_bytes
import ida_funcs
import ida_lines
import ida_name
import ida_ua
import idaapi
import idc


def parse_ea(value):
    return int(value, 0)


def item_bytes(ea, size):
    data = ida_bytes.get_bytes(ea, size) or b""
    return data.hex(" ")


def line(ea):
    text = ida_lines.generate_disasm_line(ea, 0) or ""
    return ida_lines.tag_remove(text)


def inspect_ea(ea, radius):
    start = max(idaapi.inf_get_min_ea(), ea - radius)
    end = min(idaapi.inf_get_max_ea(), ea + radius)
    func = ida_funcs.get_func(ea)
    result = {
        "ea": f"0x{ea:x}",
        "name": ida_name.get_name(ea),
        "flags": f"0x{ida_bytes.get_full_flags(ea):x}",
        "is_code": bool(ida_bytes.is_code(ida_bytes.get_full_flags(ea))),
        "is_data": bool(ida_bytes.is_data(ida_bytes.get_full_flags(ea))),
        "is_unknown": bool(ida_bytes.is_unknown(ida_bytes.get_full_flags(ea))),
        "function": None,
        "items": [],
    }
    if func:
        result["function"] = {
            "start": f"0x{func.start_ea:x}",
            "end": f"0x{func.end_ea:x}",
            "size": int(func.size()),
            "name": ida_funcs.get_func_name(func.start_ea),
            "does_return": bool(func.does_return()),
        }

    cur = ida_bytes.prev_head(ea, idaapi.inf_get_min_ea())
    if cur == idaapi.BADADDR or cur < start:
        cur = ida_bytes.next_head(start - 1, idaapi.inf_get_max_ea())
    while cur != idaapi.BADADDR and cur <= end:
        flags = ida_bytes.get_full_flags(cur)
        item_size = ida_bytes.get_item_size(cur)
        insn = ida_ua.insn_t()
        decoded = ida_ua.decode_insn(insn, cur)
        result["items"].append({
            "ea": f"0x{cur:x}",
            "flags": f"0x{flags:x}",
            "size": int(item_size),
            "is_code": bool(ida_bytes.is_code(flags)),
            "is_data": bool(ida_bytes.is_data(flags)),
            "is_unknown": bool(ida_bytes.is_unknown(flags)),
            "decode_size": int(decoded or 0),
            "bytes": item_bytes(cur, max(1, min(item_size or 1, 16))),
            "line": line(cur),
        })
        nxt = ida_bytes.next_head(cur, idaapi.inf_get_max_ea())
        if nxt == idaapi.BADADDR or nxt <= cur:
            break
        cur = nxt
    return result


def main():
    ida_auto.auto_wait()
    args = list(getattr(idc, "ARGV", sys.argv))[1:]
    output = None
    radius = 32
    eas = []
    idx = 0
    while idx < len(args):
        arg = args[idx]
        if arg == "--out":
            idx += 1
            output = Path(args[idx])
        elif arg == "--radius":
            idx += 1
            radius = int(args[idx], 0)
        else:
            eas.append(parse_ea(arg))
        idx += 1

    data = [inspect_ea(ea, radius) for ea in eas]
    text = json.dumps(data, indent=2)
    if output:
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(text, encoding="utf-8")
    else:
        print(text)
    idaapi.qexit(0)


main()
