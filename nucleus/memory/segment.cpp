/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "segment.h"
#include "nucleus/emulator.h"

#if defined(NUCLEUS_WIN)
#include <Windows.h>
#elif defined(NUCLEUS_LINUX) || defined(NUCLEUS_MACOS)
#include <sys/mman.h>
#endif

#include <cstring>

// Get real size for 4K pages
#define PAGE_4K(x) (((x) + 4095) & ~(4095))

// Memory blocks
MemoryBlock::MemoryBlock(u32 block_addr, u32 block_size)
{
    // Initialize members
    addr = block_addr;
    size = PAGE_4K(block_size);
    realaddr = (void*)((u64)nucleus.memory.getBaseAddr() + block_addr);

#if defined(NUCLEUS_WIN)
    if (VirtualAlloc(realaddr, size, MEM_COMMIT, PAGE_READWRITE) != realaddr) {
#elif defined(NUCLEUS_LINUX) || defined(NUCLEUS_MACOS)
    if (::mprotect(realaddr, block_size, PROT_READ | PROT_WRITE)) {
#endif
        // Error
        realaddr = nullptr;
        return;
    }

    //Memory.RegisterPages(_addr, PAGE_4K(_size)); // TODO
    memset(realaddr, 0, size);
}

// Memory segments
MemorySegment::MemorySegment()
{
}

MemorySegment::MemorySegment(u32 start, u32 size)
{
    init(start, size);
}

MemorySegment::~MemorySegment()
{
    close();
}

void MemorySegment::init(u32 start, u32 size)
{
    close();
    m_start = start;
    m_size = size;
}

void MemorySegment::close()
{
}

u32 MemorySegment::alloc(u32 size, u32 align)
{
    size = PAGE_4K(size);
    u32 exsize;

    if (align <= 4096) {
        align = 0;
        exsize = size;
    }
    else {
        align &= ~4095;
        exsize = size + align - 1;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    for (u32 addr = m_start; addr <= m_start + m_size - exsize;) {
        for (const auto& block : m_allocated) {
            if ((block.addr <= addr && addr < block.addr + block.size) ||
                (addr <= block.addr && block.addr < addr + exsize)) {
                addr = block.addr + block.size;
                continue;
            }
        }

        if (align) {
            addr = (addr + (align - 1)) & ~(align - 1);
        }

        m_allocated.emplace_back(addr, size);
        return addr;
    }

    return 0;
}

bool MemorySegment::allocFixed(u32 addr, u32 size)
{
    size = PAGE_4K(size + (addr & 4095)); // Align size
    addr &= ~4095; // Align start address

    if (!isValid(addr) || !isValid(addr+size-1)) {
        return false;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    for (const auto& block : m_allocated) {
        if (block.addr <= addr && addr < block.addr + block.size) { // TODO: Check (addr <= block.addr && block.addr < addr + exsize) ?
            return false;
        }
    }

    m_allocated.emplace_back(addr, size);
    return true;
}

void MemorySegment::free(u32 addr)
{
}

bool MemorySegment::isValid(u32 addr)
{
    if (addr < m_start || addr >= m_start + m_size) {
        return false;
    }
    return true;
}

u32 MemorySegment::getBaseAddr()
{
    return m_start;
}
