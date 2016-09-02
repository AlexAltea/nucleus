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
    SEG_SPU,               // 0xF0000000 to 0xFFFFFFFF

    // Count of memory segments
    _SEG_COUNT,
};

class Memory {
public:
    Memory();
    ~Memory();

    virtual U64 alloc(U64 size, U32 align=1) = 0;
    virtual void free(U64 addr) = 0;
    virtual bool check(U64 addr) = 0;

    virtual U08 read8(U64 addr) = 0;
    virtual U16 read16(U64 addr) = 0;
    virtual U32 read32(U64 addr) = 0;
    virtual U64 read64(U64 addr) = 0;
    virtual U128 read128(U64 addr) = 0;

    virtual void write8(U64 addr, U08 value) = 0;
    virtual void write16(U64 addr, U16 value) = 0;
    virtual void write32(U64 addr, U32 value) = 0;
    virtual void write64(U64 addr, U64 value) = 0;
    virtual void write128(U64 addr, U128 value) = 0;

    virtual void memcpy_h2h(void* destination, const void* source, Size num) = 0;
    virtual void memcpy_h2g(U64 destination, const void* source, Size num) = 0;
    virtual void memcpy_g2h(void* destination, U64 source, Size num)  = 0;
    virtual void memcpy_g2g(U64 destination, U64 source, Size num) = 0;
    virtual void memset(U64 ptr, int value, Size num) = 0;

    void* getBaseAddr() { return m_base; }

    Segment& getSegment(Size id) { return m_segments[id]; }

    template <typename T>
    T& ref(U32 addr) { return *(T*)((U64)m_base + addr); }

    template <typename T=void>
    T* ptr(U32 addr) { return (T*)((U64)m_base + addr); }
};

}  // namespace mem
