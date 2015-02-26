/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/config.h"

class Callback
{
    u32 m_addr;

public:
    Callback(u32 addr) : m_addr(addr) {}

    template<typename... T>
    void call(T... args)
    {
        if (config.ppuTranslator == PPU_TRANSLATOR_INTERPRETER) {
            const u32 pc = nucleus.memory.read32(m_addr);
            const u32 rtoc = nucleus.memory.read32(m_addr + 4);

            auto* thread = (cpu::ppu::Thread*)nucleus.cell.getCurrentThread();
            auto* state = thread->state;

            const u32 old_lr = state->lr;
            const u32 old_pc = state->pc;
            const u32 old_rtoc = state->gpr[2];
            state->lr = 0;
            state->pc = pc;
            state->gpr[2] = rtoc;

            // Run the function stored in the address
            thread->task();

            state->lr = old_lr;
            state->pc = old_pc;
            state->gpr[2] = old_rtoc;
        }

        if (config.ppuTranslator == PPU_TRANSLATOR_RECOMPILER) {
            // TODO
        }
    }
};
