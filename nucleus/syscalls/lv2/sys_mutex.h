/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

struct sys_mutex_attribute_t
{
    be_t<u32> protocol;
    be_t<u32> recursive;
    be_t<u32> pshared;
    be_t<u32> adaptive;
    be_t<u64> ipc_key;
    be_t<s32> flags;
    s8 padding[4];
    s8 name[8];
};

// SysCalls
s32 sys_mutex_create(be_t<u32>* mutex_id, sys_mutex_attribute_t* attr);
s32 sys_mutex_destroy(u32 mutex_id);
s32 sys_mutex_lock(u32 mutex_id, u64 timeout);
s32 sys_mutex_trylock(u32 mutex_id);
s32 sys_mutex_unlock(u32 mutex_id);
