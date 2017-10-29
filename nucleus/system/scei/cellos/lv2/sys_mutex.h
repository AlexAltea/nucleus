/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "../hle_macro.h"

#include <mutex>

namespace sys {

// Constants (some are used by other thread synchronization primitives)
enum {
    SYS_SYNC_PROCESS_SHARED      = 0x100,
    SYS_SYNC_NOT_PROCESS_SHARED  = 0x200,
};

struct sys_mutex_attribute_t
{
    BE<U32> protocol;
    BE<U32> recursive;
    BE<U32> pshared;
    BE<U32> adaptive;
    BE<U64> ipc_key;
    BE<S32> flags;
    S08 padding[4];
    S08 name[8];
};

struct sys_mutex_t
{
#if defined(NUCLEUS_TARGET_ANDROID) && defined(NUCLEUS_COMPILER_GCC)
    std::mutex mutex;
#else
    std::timed_mutex mutex;
#endif
    sys_mutex_attribute_t attr;
};

// SysCalls
HLE_FUNCTION(sys_mutex_create, BE<U32>* mutex_id, sys_mutex_attribute_t* attr);
HLE_FUNCTION(sys_mutex_destroy, U32 mutex_id);
HLE_FUNCTION(sys_mutex_lock, U32 mutex_id, U64 timeout);
HLE_FUNCTION(sys_mutex_trylock, U32 mutex_id);
HLE_FUNCTION(sys_mutex_unlock, U32 mutex_id);

}  // namespace sys
