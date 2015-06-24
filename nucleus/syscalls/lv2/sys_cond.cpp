/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_cond.h"
#include "sys_mutex.h"
#include "nucleus/syscalls/lv2.h"
#include "nucleus/emulator.h"

s32 sys_cond_create(be_t<u32>* cond_id, u32 mutex_id, sys_cond_attribute_t* attr)
{
    auto* mutex = nucleus.lv2.objects.get<sys_mutex_t>(mutex_id);

    // Check requisites
    if (!mutex) {
        return CELL_ESRCH;
    }
    if (cond_id == nucleus.memory.ptr(0) || attr == nucleus.memory.ptr(0)) {
        return CELL_EFAULT;
    }
    if (attr->pshared != SYS_SYNC_PROCESS_SHARED && attr->pshared != SYS_SYNC_NOT_PROCESS_SHARED) {
        return CELL_EINVAL;
    }

    // Check for process-shareable flag
    if (attr->pshared == SYS_SYNC_PROCESS_SHARED) {
        nucleus.log.warning(LOG_HLE, "Process-shareable semaphores are not supported");
    }

    // Create condition variable
    auto* cond = new sys_cond_t();
    cond->mutex = mutex;
    cond->attr = *attr;

    *cond_id = nucleus.lv2.objects.add(cond, SYS_COND_OBJECT);
    return CELL_OK;
}

s32 sys_cond_destroy(u32 cond_id)
{
    if (!nucleus.lv2.objects.remove(cond_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

s32 sys_cond_signal(u32 cond_id)
{
    auto* cond = nucleus.lv2.objects.get<sys_cond_t>(cond_id);

    // Check requisites
    if (!cond) {
        return CELL_ESRCH;
    }

    cond->cv.notify_one();
    return CELL_OK;
}

s32 sys_cond_signal_all(u32 cond_id)
{
    auto* cond = nucleus.lv2.objects.get<sys_cond_t>(cond_id);

    // Check requisites
    if (!cond) {
        return CELL_ESRCH;
    }

    cond->cv.notify_all();
    return CELL_OK;
}

s32 sys_cond_signal_to(u32 cond_id, u32 thread_id)
{
    auto* cond = nucleus.lv2.objects.get<sys_cond_t>(cond_id);

    // Check requisites
    if (!cond) {
        return CELL_ESRCH;
    }

    // TODO: This is wrong
    cond->cv.notify_all();
    return CELL_OK;
}

s32 sys_cond_wait(u32 cond_id, u64 timeout)
{
    auto* cond = nucleus.lv2.objects.get<sys_cond_t>(cond_id);

    // Check requisites
    if (!cond) {
        return CELL_ESRCH;
    }

    // Maximum value is: 2^48-1
    if (timeout > 0xFFFFFFFFFFFFULL) {
        timeout = 0xFFFFFFFFFFFFULL;
    }

    std::unique_lock<std::mutex> lock((std::mutex&)cond->mutex->mutex);
    if (timeout == 0) {
        cond->cv.wait(lock);
    } else {
        auto rel_time = std::chrono::microseconds(timeout);
        cond->cv.wait_for(lock, rel_time);
    }

    return CELL_OK;
}
