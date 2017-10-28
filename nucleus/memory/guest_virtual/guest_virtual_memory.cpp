/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "guest_virtual_memory.h"
#include "nucleus/logger/logger.h"

#ifdef NUCLEUS_TARGET_WINDOWS
#include <Windows.h>
#endif
#ifdef NUCLEUS_TARGET_LINUX
#include <sys/mman.h>
#endif
#ifdef NUCLEUS_TARGET_OSX
#include <sys/mman.h>
#define MAP_ANONYMOUS MAP_ANON
#endif

namespace mem {

GuestVirtualMemory::GuestVirtualMemory(Size amount) {
    // Reserve 4 GB of memory for any 32-bit pointer in the PS3 memory
#if defined(NUCLEUS_TARGET_UWP)
    m_base = nullptr;
#elif defined(NUCLEUS_TARGET_WINDOWS)
    m_base = VirtualAlloc(nullptr, amount, MEM_RESERVE, PAGE_NOACCESS);
#elif defined(NUCLEUS_TARGET_LINUX) || defined(NUCLEUS_TARGET_OSX)
    m_base = ::mmap(nullptr, amount, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
#endif

    // Check errors
    if (m_base == 0 || m_base == (void*)-1) {
        logger.error(LOG_MEMORY, "Could not reserve memory");
    }

    // Initialize segments
    m_segments[SEG_MAIN_MEMORY].init(this, 0x00010000, 0x2FFF0000);
    m_segments[SEG_USER_MEMORY].init(this, 0x10000000, 0x10000000);
    m_segments[SEG_RSX_MAP_MEMORY].init(this, 0x40000000, 0x10000000);
    m_segments[SEG_MMAPPER_MEMORY].init(this, 0xB0000000, 0x10000000);
    m_segments[SEG_RSX_LOCAL_MEMORY].init(this, 0xC0000000, 0x10000000);
    m_segments[SEG_STACK].init(this, 0xD0000000, 0x10000000);
    m_segments[SEG_SPU].init(this, 0xF0000000, 0x10000000);

    // Allocate SPU-related memory
    m_segments[SEG_SPU].alloc(0x10000000);
}

Memory::~Memory() {
    bool success;
#if defined(NUCLEUS_TARGET_UWP)
    success = false;
#elif defined(NUCLEUS_TARGET_WINDOWS)
    success = SUCCEEDED(VirtualFree(m_base, 0, MEM_RELEASE));
#elif defined(NUCLEUS_TARGET_LINUX) || defined(NUCLEUS_TARGET_OSX)
    success = munmap(m_base, 0x100000000ULL);
#endif
    if (!success) {
        logger.error(LOG_MEMORY, "Could not release memory");
    }
}

U32 GuestVirtualMemory::alloc(U64 size, U32 align) {
    return m_segments[SEG_USER_MEMORY].alloc(size, align);
}

void GuestVirtualMemory::free(U32 addr) {
    m_segments[SEG_USER_MEMORY].free(addr);
}

bool GuestVirtualMemory::check(U32 addr) {
    return true;
}

/**
 * Read memory reversing endianness if necessary
 */
U08 GuestVirtualMemory::read8(U32 addr) {
    return *(U08*)((U64)m_base + addr);
}
U16 GuestVirtualMemory::read16(U32 addr) {
    return SE16(*(U16*)((U64)m_base + addr));
}
U32 GuestVirtualMemory::read32(U32 addr) {
    return SE32(*(U32*)((U64)m_base + addr));
}
U64 GuestVirtualMemory::read64(U32 addr) {
    return SE64(*(U64*)((U64)m_base + addr));
}
U128 GuestVirtualMemory::read128(U32 addr) {
    return SE128(*(U128*)((U64)m_base + addr));
}

/**
 * Write memory reversing endianness if necessary
 */
void GuestVirtualMemory::write8(U32 addr, U08 value) {
    *(U08*)((U64)m_base + addr) = value;
}
void GuestVirtualMemory::write16(U32 addr, U16 value) {
    *(U16*)((U64)m_base + addr) = SE16(value);
}
void GuestVirtualMemory::write32(U32 addr, U32 value) {
    *(U32*)((U64)m_base + addr) = SE32(value);
}
void GuestVirtualMemory::write64(U32 addr, U64 value) {
    *(U64*)((U64)m_base + addr) = SE64(value);
}
void GuestVirtualMemory::write128(U32 addr, U128 value) {
    *(U128*)((U64)m_base + addr) = SE128(value);
}

void GuestVirtualMemory::memcpy_h2g(U64 destination, const void* source, Size num) {
}

void GuestVirtualMemory::memcpy_g2h(void* destination, U64 source, Size num) {
}

void GuestVirtualMemory::memcpy_g2g(U64 destination, U64 source, Size num) {
}

void GuestVirtualMemory::memset(U64 ptr, int value, Size num) {
}

}  // namespace mem
