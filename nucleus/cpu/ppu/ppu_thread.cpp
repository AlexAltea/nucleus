/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_thread.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/ppu/interpreter/ppu_interpreter.h"

PPUThread::PPUThread()
{
    m_translator = new PPUInterpreter(*this);

    // Initialize stack of size 0x10000
    m_stackAddr = nucleus.memory(SEG_STACK).alloc(0x10000, 0x100);
    m_stackPointer = m_stackAddr + 0x10000;

    // Initialize registers (TODO: All of this might be wrong)
    gpr[0] = pc;
    gpr[1] = m_stackPointer;
    gpr[2] = rtoc;
    gpr[3] = 0;
    gpr[4] = nucleus.memory.alloc(4,4);
    gpr[5] = gpr[4] + 0x10;
    gpr[12] = nucleus.lv2.proc_param.malloc_pagesize;
    gpr[13] = nucleus.memory(SEG_USER_MEMORY).getBaseAddr() + 0x7060; // TLS
    cr.CR = 0x22000082;
    tbl = 1;
    tbu = 1;
    nucleus.memory.write32(gpr[4], 0);
}

PPUThread::~PPUThread()
{
    // Destroy stack
    nucleus.memory(SEG_STACK).free(m_stackAddr);
}
