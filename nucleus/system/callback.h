/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

class Callback
{
    U32 m_addr;

public:
    Callback(U32 addr) : m_addr(addr) {}

    template <typename... T>
    void call(T... args) {
        const U32 pc = nucleus.memory.read32(m_addr);
        const U32 rtoc = nucleus.memory.read32(m_addr + 4);

        auto* thread = (cpu::frontend::ppu::Thread*)nucleus.cell.getCurrentThread();
        auto& state = thread->state;

        const U32 old_lr = state->lr;
        const U32 old_pc = state->pc;
        const U32 old_rtoc = state->r[2];
        state->lr = 0;
        state->pc = pc;
        state->r[2] = rtoc;

        // Run the function stored in the address
        thread->task();

        state->lr = old_lr;
        state->pc = old_pc;
        state->r[2] = old_rtoc;
    }
};
