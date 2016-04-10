/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <condition_variable>
#include <mutex>

namespace sys {

struct sys_semaphore_attribute_t
{
    BE<U32> protocol;
    BE<U32> pshared;
    BE<U64> ipc_key;
    BE<S32> flags;
    BE<U32> pad;
    S08 name[8];
};

// Auxiliary classes
struct sys_semaphore_t
{
    std::mutex mutex;
    std::condition_variable cv;
    sys_semaphore_attribute_t attr;
    S32 max_count;
    S32 count;
};

// SysCalls
S32 sys_semaphore_create(BE<U32>* sem_id, sys_semaphore_attribute_t* attr, S32 initial_count, S32 max_count);
S32 sys_semaphore_destroy(U32 sem_id);
S32 sys_semaphore_get_value(U32 sem_id, BE<S32>* val);
S32 sys_semaphore_post(U32 sem_id, S32 val);
S32 sys_semaphore_trywait(U32 sem_id);
S32 sys_semaphore_wait(U32 sem_id, U64 timeout);

}  // namespace sys
