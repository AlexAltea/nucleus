/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <condition_variable>
#include <mutex>

struct sys_semaphore_attribute_t
{
    be_t<u32> protocol;
    be_t<u32> pshared;
    be_t<u64> ipc_key;
    be_t<s32> flags;
    be_t<u32> pad;
    s8 name[8];
};

// Auxiliary classes
struct sys_semaphore_t
{
    std::mutex mutex;
    std::condition_variable cv;
    sys_semaphore_attribute_t attr;
    s32 max_count;
    s32 count;
};

// SysCalls
s32 sys_semaphore_create(be_t<u32>* sem_id, sys_semaphore_attribute_t* attr, s32 initial_count, s32 max_count);
s32 sys_semaphore_destroy(u32 sem_id);
s32 sys_semaphore_get_value(u32 sem_id, be_t<s32>* val);
s32 sys_semaphore_post(u32 sem_id, s32 val);
s32 sys_semaphore_trywait(u32 sem_id);
s32 sys_semaphore_wait(u32 sem_id, u64 timeout);
