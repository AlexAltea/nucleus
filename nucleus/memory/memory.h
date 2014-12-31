/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "segment.h"

enum
{
    // Memory segments
    SEG_MAIN_MEMORY = 0,   // 0x00010000 to 0x2FFFFFFF
    SEG_USER_MEMORY,       // 0x10000000 to 0x1FFFFFFF
    SEG_RSX_MAP_MEMORY,    // 0x40000000 to 0x4FFFFFFF
    SEG_MMAPPER_MEMORY,    // 0xB0000000 to 0xBFFFFFFF
    SEG_RSX_LOCAL_MEMORY,  // 0xC0000000 to 0xCFFFFFFF
    SEG_STACK,             // 0xD0000000 to 0xDFFFFFFF

    // Count of memory segments
    SEG_COUNT,
};

class Memory
{
    void* m_base;
    MemorySegment m_segments[SEG_COUNT];

public:
    void init();
    void close();

    u32 alloc(u32 size, u32 align=1);
    void free(u32 addr);
    bool check(u32 addr);

    u8 read8(u32 addr);
    u16 read16(u32 addr);
    u32 read32(u32 addr);
    u64 read64(u32 addr);
    u128 read128(u32 addr);
    void readLeft(u8* dst, u32 src, u32 size);
    void readRight(u8* dst, u32 src, u32 size);

    void write8(u32 addr, u8 value);
    void write16(u32 addr, u16 value);
    void write32(u32 addr, u32 value);
    void write64(u32 addr, u64 value);
    void write128(u32 addr, u128 value);
    void writeLeft(u32 dst, u8* src, u32 size);
    void writeRight(u32 dst, u8* src, u32 size);

    void* getBaseAddr() { return m_base; }

    MemorySegment& operator()(size_t id) { return m_segments[id]; }

    template<typename T>
    T& ref(u32 addr) { return *(T*)((u64)m_base + addr); }

    template<typename T=void>
    T* ptr(u32 addr) { return (T*)((u64)m_base + addr); }
};
