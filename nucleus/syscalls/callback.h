/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

class Callback
{
    u32 m_addr;

public:
    Callback(u32 addr) : m_addr(addr) {}

    template<typename... T>
    void call(T... args)
    {
        const u32 pc = nucleus.memory.read32(m_addr);
        const u32 rtoc = nucleus.memory.read32(m_addr + 4);

        auto* thread = (PPUThread*)nucleus.cell.getCurrentThread();
        const u32 old_lr = thread->lr;
        const u32 old_pc = thread->pc;
        const u32 old_rtoc = thread->rtoc;
        thread->lr = 0;
        thread->pc = pc;
        thread->rtoc = rtoc;
        thread->gpr[2] = rtoc;

        // Run the function stored in the address
        thread->task();

        thread->lr = old_lr;
        thread->pc = old_pc;
        thread->rtoc = old_rtoc;
        thread->gpr[2] = old_rtoc;
    }
};
