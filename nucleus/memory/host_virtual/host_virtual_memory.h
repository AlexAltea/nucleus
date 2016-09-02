/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/memory/memory.h"

namespace mem {

class HostVirtualMemory : public Memory {
public:
    HostVirtualMemory();
    ~HostVirtualMemory();

    virtual U64 alloc(U64 size, U32 align=1) override;
    virtual void free(U64 addr) override;
    virtual bool check(U64 addr) override;

    virtual U08 read8(U64 addr) override;
    virtual U16 read16(U64 addr) override;
    virtual U32 read32(U64 addr) override;
    virtual U64 read64(U64 addr) override;
    virtual U128 read128(U64 addr) override;

    virtual void write8(U64 addr, U08 value) override;
    virtual void write16(U64 addr, U16 value) override;
    virtual void write32(U64 addr, U32 value) override;
    virtual void write64(U64 addr, U64 value) override;
    virtual void write128(U64 addr, U128 value) override;

    virtual void memcpy_h2h(void* destination, const void* source, Size num) override;
    virtual void memcpy_h2g(U64 destination, const void* source, Size num) override;
    virtual void memcpy_g2h(void* destination, U64 source, Size num) override;
    virtual void memcpy_g2g(U64 destination, U64 source, Size num) override;
    virtual void memset(U64 ptr, int value, Size num) override;
};

}  // namespace mem
