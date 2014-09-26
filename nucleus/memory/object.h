/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/emulator.h"

#include <cstring>

/**
 * Memory allocator
 */
template<typename T>
class vm_var
{
    u32 m_addr;
    u32 m_size;

public:
    // Initialize the variable by specifying the size of the array
    vm_var(u32 count=1, u32 align=1)
    {
        m_size = count * sizeof(T);
        m_addr = nucleus.memory.alloc(m_size, align);
    }

    // Initialize the variable by specifying a string
    vm_var(const std::string& str, u32 align=1)
    {
        m_size = str.size() + 1;
        m_addr = nucleus.memory.alloc(m_size, align);
        memcpy(nucleus.memory.ptr(m_addr), str.data(), m_size);
    }

    T* ptr() { return nucleus.memory.ptr<T>(m_addr); }
    T& ref() { return nucleus.memory.ref<T>(m_addr); }

    u32 addr() { return m_addr; }
    u32 size() { return m_size; }
};
