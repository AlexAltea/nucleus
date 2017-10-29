/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_ppu_thread.h"
#include "nucleus/cpu/cpu_guest.h"
#include "nucleus/cpu/frontend/ppu/ppu_thread.h"
#include "nucleus/emulator.h"
#include "../lv2.h"

namespace sys {

HLE_FUNCTION(sys_ppu_thread_create, BE<U64>* thread_id, sys_ppu_thread_attr_t* attr, U64 arg, U64 unk0, S32 prio, U32 stacksize, U64 flags, S08* threadname) {
    const U32 entry_pc = kernel.memory->read32(attr->entry);
    const U32 entry_rtoc = kernel.memory->read32(attr->entry + 4);

    auto* cpu = dynamic_cast<cpu::GuestCPU*>(kernel.getEmulator()->cpu.get());

    // Create PPU thread
    auto* ppu_thread = new sys_ppu_thread_t();
    ppu_thread->stack.size = stacksize;
    ppu_thread->stack.addr = kernel.memory->getSegment(mem::SEG_STACK).alloc(stacksize, 0x100);
    ppu_thread->thread = static_cast<cpu::frontend::ppu::PPUThread*>(cpu->addThread(cpu::THREAD_TYPE_PPU));

    // Set PPU thread initial UISA general-purpose registers
    auto* state = ppu_thread->thread->state.get();
    state->r[0] = entry_pc;
    state->r[1] = ppu_thread->stack.addr + stacksize - 0x200;
    state->r[2] = entry_rtoc;
    state->r[3] = arg;
    state->r[4] = state->r[1] - 0x80;
    state->r[5] = state->r[4] - 0x70;
    state->r[12] = kernel.proc.param.malloc_pagesize;
    state->r[13] = kernel.memory->getSegment(mem::SEG_USER_MEMORY).getBaseAddr() + 0x7060; // TLS

    // Set other UISA registers
    state->pc = entry_pc;
    state->setCR(0x22000082);
    state->tb.TBL = 1;
    state->tb.TBU = 1;

    *thread_id = kernel.objects.add(ppu_thread, SYS_PPU_THREAD_OBJECT);
    return CELL_OK;
}

HLE_FUNCTION(sys_ppu_thread_exit, S32 errorcode) {
    // TODO: Delete stack

    auto* thread = cpu::CPU::getCurrentThread();
    thread->stop();
    return CELL_OK;
}

HLE_FUNCTION(sys_ppu_thread_get_priority, U64 thread_id, BE<S32>* prio) {
    auto* ppu_thread = kernel.objects.get<sys_ppu_thread_t>(thread_id);

    // Check requisites
    if (prio == kernel.memory->ptr(0)) {
        return CELL_EFAULT;
    }
    if (!ppu_thread) {
        return CELL_ESRCH;
    }

    // TODO: *prio = thread->prio;
    return CELL_OK;
}

HLE_FUNCTION(sys_ppu_thread_get_stack_information, sys_ppu_thread_stack_t* sp) {
    auto* thread = cpu::CPU::getCurrentThread();

    // TODO: ?
    sp->addr = 0;
    sp->size = 4096;
    return CELL_OK;
}

HLE_FUNCTION(sys_ppu_thread_join, U64 thread_id, BE<U64>* vptr) {
    auto* ppu_thread = kernel.objects.get<sys_ppu_thread_t>(thread_id);

    // Check requisites
    if (!ppu_thread) {
        return CELL_ESRCH;
    }

    ppu_thread->thread->join();
    return CELL_OK;
}

HLE_FUNCTION(sys_ppu_thread_start, U64 thread_id) {
    auto* ppu_thread = kernel.objects.get<sys_ppu_thread_t>(thread_id);

    // Check requisites
    if (!ppu_thread) {
        return CELL_ESRCH;
    }

    ppu_thread->thread->start();
    return CELL_OK;
}

}  // namespace sys
