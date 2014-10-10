/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include <mutex>
#include <vector>

struct MemoryBlock
{
    u32 addr;
    u32 size;
    void* realaddr;

    MemoryBlock(u32 block_addr, u32 block_size);
};

class MemorySegment
{
    u32 m_start;
    u32 m_size;
    std::mutex m_mutex;
    std::vector<MemoryBlock> m_allocated;

public:
    MemorySegment();
    MemorySegment(u32 start, u32 size);
    ~MemorySegment();

    void init(u32 start, u32 size);
    void close();

    u32 alloc(u32 size, u32 align=1);
    bool allocFixed(u32 addr, u32 size);
    void free(u32 addr);

    bool isValid(u32 addr);
    u32 getTotalMemory() const;
    u32 getUsedMemory() const;
    u32 getBaseAddr() const;
};
