/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/memory/segment.h"

namespace mem {

enum {
    // Memory segments
    SEG_MAIN_MEMORY = 0,   // 0x00010000 to 0x2FFFFFFF
    SEG_USER_MEMORY,       // 0x10000000 to 0x1FFFFFFF
    SEG_RSX_MAP_MEMORY,    // 0x40000000 to 0x4FFFFFFF
    SEG_MMAPPER_MEMORY,    // 0xB0000000 to 0xBFFFFFFF
    SEG_RSX_LOCAL_MEMORY,  // 0xC0000000 to 0xCFFFFFFF
    SEG_STACK,             // 0xD0000000 to 0xDFFFFFFF

    // Count of memory segments
    _SEG_COUNT,
};

class Memory {
    void* m_base;
    Segment m_segments[_SEG_COUNT];

public:
    Memory();
    ~Memory();

    U32 alloc(U32 size, U32 align=1);
    void free(U32 addr);
    bool check(U32 addr);

    U08 read8(U32 addr);
    U16 read16(U32 addr);
    U32 read32(U32 addr);
    U64 read64(U32 addr);
    U128 read128(U32 addr);
    void readLeft(U08* dst, U32 src, U32 size);
    void readRight(U08* dst, U32 src, U32 size);

    void write8(U32 addr, U08 value);
    void write16(U32 addr, U16 value);
    void write32(U32 addr, U32 value);
    void write64(U32 addr, U64 value);
    void write128(U32 addr, U128 value);
    void writeLeft(U32 dst, U08* src, U32 size);
    void writeRight(U32 dst, U08* src, U32 size);

    void* getBaseAddr() { return m_base; }

    Segment& getSegment(size_t id) { return m_segments[id]; }

    template <typename T>
    T& ref(U32 addr) { return *(T*)((U64)m_base + addr); }

    template <typename T=void>
    T* ptr(U32 addr) { return (T*)((U64)m_base + addr); }
};

}  // namespace mem
