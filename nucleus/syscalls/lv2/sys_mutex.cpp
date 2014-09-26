/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_mutex.h"
#include "nucleus/syscalls/lv2.h"

s32 sys_mutex_create(be_t<u32>* mutex_id, sys_mutex_attribute_t* attr)
{
    *mutex_id = 1;
    return CELL_OK;
}

s32 sys_mutex_destroy(u32 mutex_id)
{
    return CELL_OK;
}

s32 sys_mutex_lock(u32 mutex_id, u64 timeout)
{
    return CELL_OK;
}

s32 sys_mutex_trylock(u32 mutex_id)
{
    return CELL_OK;
}

s32 sys_mutex_unlock(u32 mutex_id)
{
    return CELL_OK;
}
