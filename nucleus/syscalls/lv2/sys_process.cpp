/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_process.h"
#include "nucleus/syscalls/lv2.h"
#include "nucleus/emulator.h"

s32 sys_process_exit(s32 errorcode)
{
    nucleus.task(NUCLEUS_EVENT_STOP);
    return CELL_OK;
}

s32 sys_process_get_paramsfo(u8* buffer)
{
    return CELL_OK;
}
