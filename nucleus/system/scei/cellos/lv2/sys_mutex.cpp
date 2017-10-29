/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_mutex.h"
#include "nucleus/emulator.h"
#include "../lv2.h"

namespace sys {

HLE_FUNCTION(sys_mutex_create, BE<U32>* mutex_id, sys_mutex_attribute_t* attr) {
    // Check requisites
    if (mutex_id == kernel.memory->ptr(0) || attr == kernel.memory->ptr(0)) {
        return CELL_EFAULT;
    }
    if (attr->pshared != SYS_SYNC_PROCESS_SHARED && attr->pshared != SYS_SYNC_NOT_PROCESS_SHARED) {
        return CELL_EINVAL;
    }

    // Create mutex
    auto* mutex = new sys_mutex_t();
    mutex->attr = *attr;

    *mutex_id = kernel.objects.add(mutex, SYS_MUTEX_OBJECT);
    return CELL_OK;
}

HLE_FUNCTION(sys_mutex_destroy, U32 mutex_id) {
    if (!kernel.objects.remove(mutex_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

HLE_FUNCTION(sys_mutex_lock, U32 mutex_id, U64 timeout) {
    auto* mutex = kernel.objects.get<sys_mutex_t>(mutex_id);

    // Check requisites
    if (!mutex) {
        return CELL_ESRCH;
    }

    // Maximum value is: 2^48-1
    if (timeout > 0xFFFFFFFFFFFFULL) {
        timeout = 0xFFFFFFFFFFFFULL;
    }

    // Lock mutex
    if (timeout == 0) {
        mutex->mutex.lock();
    } else {
        auto rel_time = std::chrono::microseconds(timeout);
#if defined(NUCLEUS_TARGET_ANDROID) && defined(NUCLEUS_COMPILER_GCC)
        // TODO: Reimplement try_lock_for
        mutex->mutex.try_lock();
#else
        mutex->mutex.try_lock_for(rel_time);
#endif
    }
    return CELL_OK;
}

HLE_FUNCTION(sys_mutex_trylock, U32 mutex_id) {
    auto* mutex = kernel.objects.get<sys_mutex_t>(mutex_id);

    // Check requisites
    if (!mutex) {
        return CELL_ESRCH;
    }

    mutex->mutex.try_lock();
    return CELL_OK;
}

HLE_FUNCTION(sys_mutex_unlock, U32 mutex_id) {
    auto* mutex = kernel.objects.get<sys_mutex_t>(mutex_id);

    // Check requisites
    if (!mutex) {
        return CELL_ESRCH;
    }

    mutex->mutex.unlock();
    return CELL_OK;
}

}  // namespace sys
