/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "lv2.h"
#include "nucleus/memory/object.h"

#include "lv2/sys_process.h"
#include "lv2/sys_prx.h"

void LV2::init()
{
    // Initialize syscall table
    //m_syscalls[000] = {wrap<sys_prx_load_module>};

    // Load and start liblv2.sprx module
    vm_var<s8> path("dev_flash/sys/external/liblv2.sprx");
    s32 moduleId = sys_prx_load_module(path, 0, 0);
    if (!moduleId) {
        // TODO: Error
    }
    sys_prx_start_module(moduleId, 0, 0, 0, 0, 0);
}
