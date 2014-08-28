/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "segment.h"

enum
{
    // Memory segments
    SEG_MAIN_MEMORY = 0,   // 0x00010000 to 0x2FFFFFFF
    SEG_PRX_MEMORY,        // 0x30000000 to 0x3FFFFFFF
    SEG_RSX_FIFO_MEMORY,   // 0x40000000 to 0x4FFFFFFF
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

    bool check();
    bool alloc();
    bool free();

    u8 read8(u32 addr);
    u16 read16(u32 addr);
    u32 read32(u32 addr);
    u64 read64(u32 addr);

    void write8(u32 addr, u8 value);
    void write16(u32 addr, u16 value);
    void write32(u32 addr, u32 value);
    void write64(u32 addr, u64 value);

    void* getBaseAddr() { return m_base; }

    MemorySegment& operator()(size_t id) { return m_segments[id]; }
    void* operator[](u32 addr) { return (void*)((u64)m_base + addr); }
};
