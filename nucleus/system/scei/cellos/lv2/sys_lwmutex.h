/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "../lv2_macro.h"

#include <mutex>

namespace sys {

struct sys_lwmutex_attribute_t
{
    BE<U32> protocol;
    BE<U32> recursive;
    S08 name[8];
};

// Auxiliary classes
struct sys_lwmutex_t
{
#if defined(NUCLEUS_TARGET_ANDROID) && defined(NUCLEUS_COMPILER_GCC)
    std::mutex lwmutex;
#else
    std::timed_mutex lwmutex;
#endif
    sys_lwmutex_attribute_t attr;
};

// SysCalls
LV2_SYSCALL(sys_lwmutex_create, BE<U32>* lwmutex_id, sys_lwmutex_attribute_t* attr);
LV2_SYSCALL(sys_lwmutex_destroy, U32 lwmutex_id);
LV2_SYSCALL(sys_lwmutex_lock, U32 lwmutex_id, U64 timeout);
LV2_SYSCALL(sys_lwmutex_trylock, U32 lwmutex_id);
LV2_SYSCALL(sys_lwmutex_unlock, U32 lwmutex_id);

}  // namespace sys
