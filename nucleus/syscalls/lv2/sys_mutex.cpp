/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_mutex.h"
#include "nucleus/syscalls/lv2.h"
#include "nucleus/emulator.h"

s32 sys_mutex_create(be_t<u32>* mutex_id, sys_mutex_attribute_t* attr)
{
    // Check requisites
    if (mutex_id == nucleus.memory.ptr(0) || attr == nucleus.memory.ptr(0)) {
        return CELL_EFAULT;
    }
    if (attr->pshared != SYS_SYNC_PROCESS_SHARED && attr->pshared != SYS_SYNC_NOT_PROCESS_SHARED) {
        return CELL_EINVAL;
    }

    // Create mutex
    auto* mutex = new sys_mutex_t();
    mutex->attr = *attr;

    *mutex_id = nucleus.lv2.objects.add(mutex, SYS_MUTEX_OBJECT);
    return CELL_OK;
}

s32 sys_mutex_destroy(u32 mutex_id)
{
    if (!nucleus.lv2.objects.remove(mutex_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

s32 sys_mutex_lock(u32 mutex_id, u64 timeout)
{
    auto* mutex = nucleus.lv2.objects.get<sys_mutex_t>(mutex_id);

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
        mutex->mutex.try_lock_for(rel_time);
    }
    return CELL_OK;
}

s32 sys_mutex_trylock(u32 mutex_id)
{
    auto* mutex = nucleus.lv2.objects.get<sys_mutex_t>(mutex_id);

    // Check requisites
    if (!mutex) {
        return CELL_ESRCH;
    }

    mutex->mutex.try_lock();
    return CELL_OK;
}

s32 sys_mutex_unlock(u32 mutex_id)
{
    auto* mutex = nucleus.lv2.objects.get<sys_mutex_t>(mutex_id);

    // Check requisites
    if (!mutex) {
        return CELL_ESRCH;
    }

    mutex->mutex.unlock();
    return CELL_OK;
}
