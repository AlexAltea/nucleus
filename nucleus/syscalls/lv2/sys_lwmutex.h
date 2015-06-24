/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <mutex>

struct sys_lwmutex_attribute_t
{
    be_t<u32> protocol;
    be_t<u32> recursive;
    s8 name[8];
};

// Auxiliary classes
struct sys_lwmutex_t
{
    std::timed_mutex lwmutex;
    sys_lwmutex_attribute_t attr;
};

// SysCalls
s32 sys_lwmutex_create(be_t<u32>* lwmutex_id, sys_lwmutex_attribute_t* attr);
s32 sys_lwmutex_destroy(u32 lwmutex_id);
s32 sys_lwmutex_lock(u32 lwmutex_id, u64 timeout);
s32 sys_lwmutex_trylock(u32 lwmutex_id);
s32 sys_lwmutex_unlock(u32 lwmutex_id);
