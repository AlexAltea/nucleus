/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <mutex>
#include <vector>

namespace mem {

// Forward declarations
class GuestVirtualMemory;

struct Block {
    U32 addr;
    U32 size;
    void* realaddr;

    Block(void* baseAddr, U32 blockAddr, U32 blockSize);
};

class Segment {
    GuestVirtualMemory* m_parent;
    U32 m_start;
    U32 m_size;
    std::mutex m_mutex;
    std::vector<Block> m_allocated;

public:
    Segment();
    Segment(GuestVirtualMemory* parent, U32 start, U32 size);
    ~Segment();

    void init(GuestVirtualMemory* parent, U32 start, U32 size);
    void close();

    U32 alloc(U32 size, U32 align=1);
    U32 allocFixed(U32 addr, U32 size);
    bool free(U32 addr);

    bool isValid(U32 addr);
    U32 getTotalMemory() const;
    U32 getUsedMemory() const;
    U32 getBaseAddr() const;
};

}  // namespace mem
