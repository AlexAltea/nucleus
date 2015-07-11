/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <mutex>

namespace sys {

struct sys_lwmutex_attribute_t
{
    BE<U32> protocol;
    BE<U32> recursive;
    U8 name[8];
};

// Auxiliary classes
struct sys_lwmutex_t
{
    std::timed_mutex lwmutex;
    sys_lwmutex_attribute_t attr;
};

// SysCalls
S32 sys_lwmutex_create(BE<U32>* lwmutex_id, sys_lwmutex_attribute_t* attr);
S32 sys_lwmutex_destroy(U32 lwmutex_id);
S32 sys_lwmutex_lock(U32 lwmutex_id, U64 timeout);
S32 sys_lwmutex_trylock(U32 lwmutex_id);
S32 sys_lwmutex_unlock(U32 lwmutex_id);

}  // namespace sys
