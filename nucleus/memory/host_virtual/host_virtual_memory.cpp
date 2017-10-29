/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "host_virtual_memory.h"
#include "nucleus/logger/logger.h"

#include <malloc.h>

namespace mem {

HostVirtualMemory::HostVirtualMemory() {
}
HostVirtualMemory::~HostVirtualMemory() {
}

U64 HostVirtualMemory::alloc(U64 size, U32 alignment) {
#ifdef NUCLEUS_COMPILER_MSVC
    void* address = _aligned_malloc(size, alignment);
#else
    void* address = memalign(size, alignment);
#endif
    return reinterpret_cast<U64>(address);
}

void HostVirtualMemory::free(U64 addr) {
    void* address = reinterpret_cast<void*>(addr);
    ::free(address);
}

bool HostVirtualMemory::check(U64 addr) {
    return (addr != 0);
}

// Memory reads
U08 HostVirtualMemory::read8(U64 addr) {
    return *reinterpret_cast<U08*>(addr);
}
U16 HostVirtualMemory::read16(U64 addr) {
    return *reinterpret_cast<U16*>(addr);
}
U32 HostVirtualMemory::read32(U64 addr) {
    return *reinterpret_cast<U32*>(addr);
}
U64 HostVirtualMemory::read64(U64 addr) {
    return *reinterpret_cast<U64*>(addr);
}
U128 HostVirtualMemory::read128(U64 addr) {
    return *reinterpret_cast<U128*>(addr);
}

// Memory writes
void HostVirtualMemory::write8(U64 addr, U08 value) {
    *reinterpret_cast<U08*>(addr) = value;
}
void HostVirtualMemory::write16(U64 addr, U16 value) {
    *reinterpret_cast<U16*>(addr) = value;
}
void HostVirtualMemory::write32(U64 addr, U32 value) {
    *reinterpret_cast<U32*>(addr) = value;
}
void HostVirtualMemory::write64(U64 addr, U64 value) {
    *reinterpret_cast<U64*>(addr) = value;
}
void HostVirtualMemory::write128(U64 addr, U128 value) {
    *reinterpret_cast<U128*>(addr) = value;
}

void HostVirtualMemory::memcpy_h2g(U64 dst, const void* src_ptr, Size size) {
    void* dst_ptr = reinterpret_cast<void*>(dst);
    ::memcpy(dst_ptr, src_ptr, size);
}

void HostVirtualMemory::memcpy_g2h(void* dst_ptr, U64 src, Size size) {
    void* src_ptr = reinterpret_cast<void*>(src);
    ::memcpy(dst_ptr, src_ptr, size);
}

void HostVirtualMemory::memcpy_g2g(U64 dst, U64 src, Size size) {
    void* dst_ptr = reinterpret_cast<void*>(dst);
    void* src_ptr = reinterpret_cast<void*>(src);
    ::memcpy(dst_ptr, src_ptr, size);
}

void HostVirtualMemory::memset(U64 addr, int value, Size size) {
    void* addr_ptr = reinterpret_cast<void*>(addr);
    ::memset(addr_ptr, value, size);
}

}  // namespace mem
