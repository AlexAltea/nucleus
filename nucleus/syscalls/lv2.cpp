/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "lv2.h"
#include "nucleus/memory/object.h"

#include "lv2/sys_process.h"
#include "lv2/sys_prx.h"
#include "lv2/sys_tty.h"

bool LV2::init()
{
    // Initialize syscall table
    m_syscalls[0x003] = {wrap(sys_process_exit), LV2_NONE};
    m_syscalls[0x016] = {wrap(sys_process_exit), LV2_NONE};
    m_syscalls[0x193] = {wrap(sys_tty_write), LV2_NONE};

    // Load and start liblv2.sprx module
    s32 moduleId = sys_prx_load_module("dev_flash/sys/external/liblv2.sprx", 0, 0);
    if (moduleId <= CELL_OK) {
        nucleus.log.error(LOG_HLE, "You need to provide the /dev_flash/sys/external/ firmware files.");
        return false;
    }
    sys_prx_start_module(moduleId, 0, 0, 0, 0, 0);
    return true;
}

void LV2::call(PPUThread& thread)
{
    const u32 id = thread.gpr[11];

    if (!m_syscalls[id].func) {
        nucleus.log.warning(LOG_HLE, "LV2 Syscall %d (0x%x) called", id, id);
        return;
    }

    m_syscalls[id].func->call(thread);
}
