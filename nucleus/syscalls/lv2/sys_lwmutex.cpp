/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_lwmutex.h"
#include "nucleus/syscalls/lv2.h"

s32 sys_lwmutex_create(be_t<u32>* lwmutex_id, sys_lwmutex_attribute_t* attr)
{
    *lwmutex_id = 1;
    return CELL_OK;
}

s32 sys_lwmutex_destroy(u32 lwmutex_id)
{
    return CELL_OK;
}

s32 sys_lwmutex_lock(u32 lwmutex_id, u64 timeout)
{
    return CELL_OK;
}

s32 sys_lwmutex_trylock(u32 lwmutex_id)
{
    return CELL_OK;
}

s32 sys_lwmutex_unlock(u32 lwmutex_id)
{
    return CELL_OK;
}
