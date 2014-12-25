/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "memory.h"
#include "nucleus/common.h"
#include "nucleus/emulator.h"

#ifdef NUCLEUS_WIN
#include <Windows.h>
#endif
#ifdef NUCLEUS_LINUX
#include <sys/mman.h>
#endif
#ifdef NUCLEUS_MACOS
#include <sys/mman.h>
#define MAP_ANONYMOUS MAP_ANON
#endif

void Memory::init()
{
    // Reserve 4 GB of memory for any 32-bit pointer in the PS3 memory
#if defined(NUCLEUS_WIN)
    m_base = VirtualAlloc(nullptr, 0x100000000ULL, MEM_RESERVE, PAGE_NOACCESS);
#elif defined(NUCLEUS_LINUX) || defined(NUCLEUS_MACOS)
    m_base = ::mmap(nullptr, 0x100000000ULL, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
#endif

    // Check errors
    if (m_base == 0 || m_base == (void*)-1) {
        nucleus.log.error(LOG_MEMORY, "Could not reserve memory");
    }

    // Initialize segments
    m_segments[SEG_MAIN_MEMORY].init(0x00010000, 0x2FFF0000);
    m_segments[SEG_USER_MEMORY].init(0x10000000, 0x10000000);
    m_segments[SEG_RSX_MAP_MEMORY].init(0x40000000, 0x10000000);
    m_segments[SEG_MMAPPER_MEMORY].init(0xB0000000, 0x10000000);
    m_segments[SEG_RSX_LOCAL_MEMORY].init(0xC0000000, 0x10000000);
    m_segments[SEG_STACK].init(0xD0000000, 0x10000000);
}

void Memory::close()
{
#if defined(NUCLEUS_WIN)
    if (!VirtualFree(m_base, 0, MEM_RELEASE)) {
#elif defined(NUCLEUS_LINUX) || defined(NUCLEUS_MACOS)
    if (::munmap(m_base, 0x100000000ULL)) {
#endif
        nucleus.log.error(LOG_MEMORY, "Could not release memory");
    }
}

u32 Memory::alloc(u32 size, u32 align)
{
    return m_segments[SEG_USER_MEMORY].alloc(size, align);
}

void Memory::free(u32 addr)
{
    m_segments[SEG_USER_MEMORY].free(addr);
}

bool Memory::check(u32 addr)
{
    return true;
}

/**
 * Read memory reversing endianness if necessary
 */
u8 Memory::read8(u32 addr)
{
    return *(u8*)((u64)m_base + addr);
}
u16 Memory::read16(u32 addr)
{
    return re16(*(u16*)((u64)m_base + addr));
}
u32 Memory::read32(u32 addr)
{
    return re32(*(u32*)((u64)m_base + addr));
}
u64 Memory::read64(u32 addr)
{
    return re64(*(u64*)((u64)m_base + addr));
}
u128 Memory::read128(u32 addr)
{
    return re128(*(u128*)((u64)m_base + addr));
}
void Memory::readLeft(u8* dst, u32 src, u32 size)
{
    for (u32 i = 0; i < size; i++) {
        dst[size - 1 - i] = read8(src + i);
    }
}
void Memory::readRight(u8* dst, u32 src, u32 size)
{
    for (u32 i = 0; i < size; i++) {
        dst[i] = read8(src + (size - 1 - i));
    }
}

/**
 * Write memory reversing endianness if necessary
 */
void Memory::write8(u32 addr, u8 value)
{
    *(u8*)((u64)m_base + addr) = value;
}
void Memory::write16(u32 addr, u16 value)
{
    *(u16*)((u64)m_base + addr) = re16(value);
}
void Memory::write32(u32 addr, u32 value)
{
    *(u32*)((u64)m_base + addr) = re32(value);
}
void Memory::write64(u32 addr, u64 value)
{
    *(u64*)((u64)m_base + addr) = re64(value);
}
void Memory::write128(u32 addr, u128 value)
{
    *(u128*)((u64)m_base + addr) = re128(value);
}
void Memory::writeLeft(u32 dst, u8* src, u32 size)
{
    for (u32 i = 0; i < size; i++) {
        write8(dst + i, src[size - 1 - i]);
    }
}
void Memory::writeRight(u32 dst, u8* src, u32 size)
{
    for (u32 i = 0; i < size; i++) {
        write8(dst + (size - 1 - i), src[i]);
    }
}
