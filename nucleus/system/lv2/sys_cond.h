/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "sys_mutex.h"

#include <condition_variable>

namespace sys {

// Classes
struct sys_cond_attribute_t
{
    BE<U32> pshared;
    BE<U64> ipc_key;
    BE<S32> flags;
    S8 name[8];
};

// Auxiliary classes
struct sys_cond_t
{
    std::condition_variable cv;
    sys_mutex_t* mutex;
    sys_cond_attribute_t attr;
};

// SysCalls
S32 sys_cond_create(BE<U32>* cond_id, U32 mutex_id, sys_cond_attribute_t* attr);
S32 sys_cond_destroy(U32 cond_id);
S32 sys_cond_wait(U32 cond_id, U64 timeout);
S32 sys_cond_signal(U32 cond_id);
S32 sys_cond_signal_all(U32 cond_id);
S32 sys_cond_signal_to(U32 cond_id, U32 thread_id);

}  // namespace sys
