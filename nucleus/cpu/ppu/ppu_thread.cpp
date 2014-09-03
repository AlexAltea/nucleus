/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_thread.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/ppu/ppu_interpreter.h"

PPUThread::PPUThread()
{
    /*CloseStack()
{
    if(m_stack_addr)
    {
        Memory.StackMem.Free(m_stack_addr);
        m_stack_addr = 0;
    }

    m_stack_size = 0;
    m_stack_point = 0;
    */

    /*m_sync_wait = 0;
    m_wait_thread_id = -1;

    SetPc(0);
    cycle = 0;
    m_is_branch = false;

    m_status = Stopped;
    m_error = 0;
    */

    // owned_mutexes = 0
    m_translator = new PPUInterpreter(*this);
    reset();
}

void PPUThread::reset()
{
    // Reset registers
    memset(gpr, 0, sizeof(gpr));
    memset(fpr, 0, sizeof(fpr));
    memset(vr, 0, sizeof(vr));
    
    

    // TODO: Other registers
    // PC=0

    // Reset stack
    /*if (m_stack_addr) {
        nucleus.memory(SEG_STACK).free(m_stack_addr);
    }*/
    //m_stack_addr = nucleus.memory(SEG_STACK).alloc(0x10000, 0x100);
    // TODO: stack_point, stack_size
}
