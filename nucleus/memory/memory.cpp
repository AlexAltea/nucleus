/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "common.h"
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

void memory::init()
{
#if defined(NUCLEUS_WIN)
    m_base = VirtualAlloc(nullptr, 0x100000000ULL, MEM_RESERVE, PAGE_NOACCESS);
#endif
#if defined(NUCLEUS_LINUX) || defined(NUCLEUS_MACOS)
    m_base = ::mmap(nullptr, 0x100000000ULL, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
#endif

    if (m_base == 0 || m_base == (void*)-1) {
    }
}
