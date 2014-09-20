/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "lv2.h"
#include "nucleus/memory/object.h"

#include "lv2/sys_process.h"
#include "lv2/sys_prx.h"
#include "lv2/sys_tty.h"

void LV2::init()
{
    // Initialize syscall table
    m_syscalls[0x193] = {wrap(sys_tty_write), LV2_NONE};

    // Load and start liblv2.sprx module
    vm_var<s8> path("dev_flash/sys/external/liblv2.sprx");
    s32 moduleId = sys_prx_load_module(path, 0, 0);
    if (!moduleId) {
        // TODO: Error
    }
    sys_prx_start_module(moduleId, 0, 0, 0, 0, 0);
}

void LV2::call(PPUThread& thread)
{
    const u32 id = thread.gpr[11];

    if (!m_syscalls[id].func) {
        // TODO: Error
        return;
    }

    m_syscalls[id].func->call(thread);
}
