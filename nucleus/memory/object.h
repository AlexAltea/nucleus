/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/emulator.h"

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
        memcpy(nucleus.memory + m_addr, str.data(), m_size);
    }

    T* ptr() { return (T*)(nucleus.memory + m_addr); }
    T& ref() { return *(T*)(nucleus.memory + m_addr); }

    u32 addr() { return m_addr; }
    u32 size() { return m_size; }
};

/**
 * Pointer to PS3's virtual memory
 */
template<typename T>
class vm_ptr
{
    u32 m_addr;

public:
    vm_ptr(u32 addr) { m_addr = addr; }
    vm_ptr(vm_var<T>& var) { m_addr = var.addr(); }

    vm_ptr operator++(int) { u32 result = m_addr; m_addr += sizeof(T); return make(result); }
    vm_ptr operator--(int) { u32 result = m_addr; m_addr -= sizeof(T); return make(result); }
    vm_ptr operator++() { m_addr += sizeof(T); return *this; }
    vm_ptr operator--() { m_addr -= sizeof(T); return *this; }

    vm_ptr operator+=(s32 count) { m_addr += count * sizeof(AT); return *this; }
	vm_ptr operator-=(s32 count) { m_addr -= count * sizeof(AT); return *this; }

    T* ptr() { return (T*)(nucleus.memory + m_addr); }
    T& ref() { return *(T*)(nucleus.memory + m_addr); }
};
