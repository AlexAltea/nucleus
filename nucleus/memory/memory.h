/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace mem {

enum Permission {
    PERM_R = 1 << 0, // Read
    PERM_W = 1 << 1, // Write
    PERM_X = 1 << 2, // Execute
};

class Memory {
public:
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

    virtual void memcpy_h2g(U64 destination, const void* source, Size num) = 0;
    virtual void memcpy_g2h(void* destination, U64 source, Size num)  = 0;
    virtual void memcpy_g2g(U64 destination, U64 source, Size num) = 0;
    virtual void memset(U64 ptr, int value, Size num) = 0;
};

}  // namespace mem
