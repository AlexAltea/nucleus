/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "segment.h"
#include "nucleus/memory/memory.h"

#if defined(NUCLEUS_PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(NUCLEUS_PLATFORM_LINUX) || defined(NUCLEUS_PLATFORM_OSX)
#include <sys/mman.h>
#endif

#include <cstring>

// Get real size for 4K pages
#define PAGE_4K(x) (((x) + 4095) & ~(4095))

namespace mem {

// Memory blocks
Block::Block(void* baseAddr, U32 blockAddr, U32 blockSize) {
    // Initialize members
    addr = blockAddr;
    size = PAGE_4K(blockSize);
    realaddr = reinterpret_cast<void*>((reinterpret_cast<intptr_t>(baseAddr) + blockAddr));

#if defined(NUCLEUS_PLATFORM_WINDOWS)
    if (VirtualAlloc(realaddr, size, MEM_COMMIT, PAGE_READWRITE) != realaddr) {
#elif defined(NUCLEUS_PLATFORM_LINUX) || defined(NUCLEUS_PLATFORM_OSX)
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
Segment::Segment() {
}

Segment::Segment(Memory* parent, U32 start, U32 size) {
    init(parent, start, size);
}

Segment::~Segment() {
    close();
}

void Segment::init(Memory* parent, U32 start, U32 size) {
    close();
    m_parent = parent;
    m_start = start;
    m_size = size;
}

void Segment::close() {
}

U32 Segment::alloc(U32 size, U32 align) {
    size = PAGE_4K(size);
    U32 exsize;

    if (align <= 4096) {
        align = 0;
        exsize = size;
    } else {
        align &= ~4095;
        exsize = size + align - 1;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    for (U32 addr = m_start; addr <= m_start + m_size - exsize;) {
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

        m_allocated.emplace_back(m_parent->getBaseAddr(), addr, size);
        return addr;
    }

    return 0;
}

U32 Segment::allocFixed(U32 addr, U32 size) {
    size = PAGE_4K(size + (addr & 4095)); // Align size
    addr &= ~4095; // Align start address

    if (!isValid(addr) || !isValid(addr+size-1)) {
        return 0;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    for (const auto& block : m_allocated) {
        if (block.addr <= addr && addr < block.addr + block.size) { // TODO: Check (addr <= block.addr && block.addr < addr + exsize) ?
            return 0;
        }
    }

    m_allocated.emplace_back(m_parent->getBaseAddr(), addr, size);
    return addr;
}

bool Segment::free(U32 addr) {
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto it = m_allocated.begin(); it != m_allocated.end(); it++) {
        if (it->addr == addr) {
            m_allocated.erase(it);
            return true;
        }
    }

    return false;
}

bool Segment::isValid(U32 addr) {
    if (addr < m_start || addr >= m_start + m_size) {
        return false;
    }
    return true;
}

U32 Segment::getTotalMemory() const {
    return m_size;
}

U32 Segment::getUsedMemory() const {
    U32 usedMemory = 0;
    for (const auto& block : m_allocated) {
        usedMemory += block.size;
    }
    return usedMemory;
}

U32 Segment::getBaseAddr() const {
    return m_start;
}

}  // namespace mem
