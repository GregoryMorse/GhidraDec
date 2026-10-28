#ifndef GHIDRADEC_FUNCTION_MAPPING_H
#define GHIDRADEC_FUNCTION_MAPPING_H

#include "sleighinterface.h"

struct FunctionQueryMapping
{
    bool symbol;
    unsigned long long size;
    unsigned long long holeEnd;
};

// Match Ghidra 12.2 DecompileCallback.encodeFunction, without spanning chunks.
inline FunctionQueryMapping functionQueryMapping(const MappedSymbolInfo& info, const AddrInfo& query)
{
    for (const auto& range : info.ranges) {
        if (range.space != query.space || query.offset < range.beginoffset || query.offset > range.endoffset)
            continue;
        if (info.entryPoint >= range.beginoffset && info.entryPoint <= range.endoffset) {
            if (query.offset < info.entryPoint)
                return {false, 0, info.entryPoint - 1};
            if (query.offset - info.entryPoint < 8)
                return {true, range.endoffset - info.entryPoint >= 7 ? 8 : range.endoffset - info.entryPoint + 1, query.offset};
        }
        return {false, 0, range.endoffset};
    }
    // Preserve callable orphan/import entries for hosts without chunk metadata.
    return {query.offset == info.entryPoint, 1, query.offset};
}

#endif
