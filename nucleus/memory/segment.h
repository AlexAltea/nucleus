/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include <mutex>
#include <vector>

struct MemoryBlock
{
    U32 addr;
    U32 size;
    void* realaddr;

    MemoryBlock(U32 block_addr, U32 block_size);
};

class MemorySegment
{
    U32 m_start;
    U32 m_size;
    std::mutex m_mutex;
    std::vector<MemoryBlock> m_allocated;

public:
    MemorySegment();
    MemorySegment(U32 start, U32 size);
    ~MemorySegment();

    void init(U32 start, U32 size);
    void close();

    U32 alloc(U32 size, U32 align=1);
    U32 allocFixed(U32 addr, U32 size);
    bool free(U32 addr);

    bool isValid(U32 addr);
    U32 getTotalMemory() const;
    U32 getUsedMemory() const;
    U32 getBaseAddr() const;
};
