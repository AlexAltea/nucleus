/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_mutex.h"
#include "nucleus/system/scei/cellos/lv2.h"
#include "nucleus/emulator.h"

namespace sys {

S32 sys_mutex_create(BE<U32>* mutex_id, sys_mutex_attribute_t* attr) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    // Check requisites
    if (mutex_id == nucleus.memory->ptr(0) || attr == nucleus.memory->ptr(0)) {
        return CELL_EFAULT;
    }
    if (attr->pshared != SYS_SYNC_PROCESS_SHARED && attr->pshared != SYS_SYNC_NOT_PROCESS_SHARED) {
        return CELL_EINVAL;
    }

    // Create mutex
    auto* mutex = new sys_mutex_t();
    mutex->attr = *attr;

    *mutex_id = lv2.objects.add(mutex, SYS_MUTEX_OBJECT);
    return CELL_OK;
}

S32 sys_mutex_destroy(U32 mutex_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    if (!lv2.objects.remove(mutex_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

S32 sys_mutex_lock(U32 mutex_id, U64 timeout) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* mutex = lv2.objects.get<sys_mutex_t>(mutex_id);

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
#if defined(NUCLEUS_PLATFORM_ANDROID) && defined(NUCLEUS_COMPILER_GCC)
        // TODO: Reimplement try_lock_for
        mutex->mutex.try_lock();
#else
        mutex->mutex.try_lock_for(rel_time);
#endif    
    }
    return CELL_OK;
}

S32 sys_mutex_trylock(U32 mutex_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* mutex = lv2.objects.get<sys_mutex_t>(mutex_id);

    // Check requisites
    if (!mutex) {
        return CELL_ESRCH;
    }

    mutex->mutex.try_lock();
    return CELL_OK;
}

S32 sys_mutex_unlock(U32 mutex_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* mutex = lv2.objects.get<sys_mutex_t>(mutex_id);

    // Check requisites
    if (!mutex) {
        return CELL_ESRCH;
    }

    mutex->mutex.unlock();
    return CELL_OK;
}

}  // namespace sys
