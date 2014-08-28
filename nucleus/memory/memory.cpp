/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "nucleus/common.h"
#include "memory.h"

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
        // TODO
    }

    // Initialize segments
    m_segments[SEG_MAIN_MEMORY].init(0x00010000, 0x2FFF0000);
    m_segments[SEG_PRX_MEMORY].init(0x30000000, 0x10000000);
    m_segments[SEG_RSX_FIFO_MEMORY].init(0x40000000, 0x10000000);
    m_segments[SEG_MMAPPER_MEMORY].init(0xB0000000, 0x10000000);
    m_segments[SEG_RSX_LOCAL_MEMORY].init(0xC0000000, 0x10000000);
    m_segments[SEG_STACK].init(0xD0000000, 0x10000000);
}

void Memory::close()
{
#if defined(NUCLEUS_WIN)
	if (!VirtualFree(m_base, 0, MEM_RELEASE)) {
#elif defined(NUCLEUS_LINUX) || defined(NUCLEUS_MACOS)
    if (::munmap(m_base_addr, 0x100000000)) {
#endif
		// Error
	}
}

bool Memory::check()
{
    return true;
}
