/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_ppu_thread.h"
#include "nucleus/system/lv2.h"
#include "nucleus/emulator.h"

namespace sys {

S32 sys_ppu_thread_create(BE<U64>* thread_id, sys_ppu_thread_attr_t* attr, U64 arg, U64 unk0, S32 prio, U32 stacksize, U64 flags, S8* threadname)
{
    U32 entry = attr->entry;
    U32 tls_addr = attr->tls_addr;

    // Create PPU thread
    auto* thread = (cpu::ppu::Thread*)nucleus.cell.addThread(CELL_THREAD_PPU, entry);
    auto* state = thread->state;
    thread->prio = prio;
    state->gpr[3] = arg;

    *thread_id = nucleus.lv2.objects.add(thread, SYS_PPU_THREAD_OBJECT);
    return CELL_OK;
}

S32 sys_ppu_thread_exit(S32 errorcode)
{
    auto* thread = nucleus.cell.getCurrentThread();
    thread->stop();
    return CELL_OK;
}

S32 sys_ppu_thread_get_priority(U64 thread_id, BE<S32>* prio)
{
    auto* thread = nucleus.lv2.objects.get<cpu::ppu::Thread>(thread_id);

    // Check requisites
    if (prio == nucleus.memory.ptr(0)) {
        return CELL_EFAULT;
    }
    if (!thread) {
        return CELL_ESRCH;
    }

    *prio = thread->prio;
    return CELL_OK;
}

S32 sys_ppu_thread_get_stack_information(sys_ppu_thread_stack_t* sp)
{
    auto* thread = nucleus.cell.getCurrentThread();

    // TODO: ?
    sp->addr = 0;
    sp->size = 4096;
    return CELL_OK;
}

S32 sys_ppu_thread_join(U64 thread_id, BE<U64>* vptr)
{
    auto* thread = nucleus.lv2.objects.get<cpu::ppu::Thread>(thread_id);

    // Check requisites
    if (!thread) {
        return CELL_ESRCH;
    }

    thread->join();
    return CELL_OK;
}

S32 sys_ppu_thread_start(U64 thread_id)
{
    auto* thread = nucleus.lv2.objects.get<cpu::ppu::Thread>(thread_id);

    // Check requisites
    if (!thread) {
        return CELL_ESRCH;
    }

    thread->start();
    return CELL_OK;
}

}  // namespace sys
