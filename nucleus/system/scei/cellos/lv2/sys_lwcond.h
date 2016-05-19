/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <condition_variable>

namespace sys {

// Forward declarations
struct sys_lwmutex_t;

// Classes
struct sys_lwcond_attribute_t {
    S08 name[8];
};

// Auxiliary classes
struct sys_lwcond_t {
    std::condition_variable cv;
    sys_lwmutex_t* lwmutex;
    sys_lwcond_attribute_t attr;
};

// SysCalls
S32 sys_lwcond_create(BE<U32>* lwcond_id, U32 lwmutex_id, sys_lwcond_attribute_t* attr);
S32 sys_lwcond_destroy(U32 lwcond_id);
S32 sys_lwcond_queue_wait(U32 lwcond_id, U64 timeout);
S32 sys_lwcond_signal(U32 lwcond_id);
S32 sys_lwcond_signal_all(U32 lwcond_id);

}  // namespace sys
