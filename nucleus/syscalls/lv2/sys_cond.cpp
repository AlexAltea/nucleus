/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_cond.h"
#include "nucleus/syscalls/lv2.h"

s32 sys_cond_create(be_t<u32>* cond_id, u32 mutex_id, sys_cond_attribute_t* attr)
{
    *cond_id = 1;
    return CELL_OK;
}

s32 sys_cond_destroy(u32 cond_id)
{
    return CELL_OK;
}

s32 sys_cond_signal(u32 cond_id)
{
    return CELL_OK;
}

s32 sys_cond_signal_all(u32 cond_id)
{
    return CELL_OK;
}

s32 sys_cond_signal_to(u32 cond_id, u32 thread_id)
{
    return CELL_OK;
}

s32 sys_cond_wait(u32 cond_id, u64 timeout)
{
    return CELL_OK;
}
