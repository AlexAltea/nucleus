/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_cond.h"
#include "sys_mutex.h"
#include "nucleus/emulator.h"
#include "nucleus/logger/logger.h"
#include "../lv2.h"

namespace sys {

HLE_FUNCTION(sys_cond_create, BE<U32>* cond_id, U32 mutex_id, sys_cond_attribute_t* attr) {
    auto* mutex = kernel.objects.get<sys_mutex_t>(mutex_id);

    // Check requisites
    if (!mutex) {
        return CELL_ESRCH;
    }
    if (cond_id == kernel.memory->ptr(0) || attr == kernel.memory->ptr(0)) {
        return CELL_EFAULT;
    }
    if (attr->pshared != SYS_SYNC_PROCESS_SHARED && attr->pshared != SYS_SYNC_NOT_PROCESS_SHARED) {
        return CELL_EINVAL;
    }

    // Check for process-shareable flag
    if (attr->pshared == SYS_SYNC_PROCESS_SHARED) {
        logger.warning(LOG_HLE, "Process-shareable semaphores are not supported");
    }

    // Create condition variable
    auto* cond = new sys_cond_t();
    cond->mutex = mutex;
    cond->attr = *attr;

    *cond_id = kernel.objects.add(cond, SYS_COND_OBJECT);
    return CELL_OK;
}

HLE_FUNCTION(sys_cond_destroy, U32 cond_id) {
    if (!kernel.objects.remove(cond_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

HLE_FUNCTION(sys_cond_signal, U32 cond_id) {
    auto* cond = kernel.objects.get<sys_cond_t>(cond_id);

    // Check requisites
    if (!cond) {
        return CELL_ESRCH;
    }

    cond->cv.notify_one();
    return CELL_OK;
}

HLE_FUNCTION(sys_cond_signal_all, U32 cond_id) {
    auto* cond = kernel.objects.get<sys_cond_t>(cond_id);

    // Check requisites
    if (!cond) {
        return CELL_ESRCH;
    }

    cond->cv.notify_all();
    return CELL_OK;
}

HLE_FUNCTION(sys_cond_signal_to, U32 cond_id, U32 thread_id) {
    auto* cond = kernel.objects.get<sys_cond_t>(cond_id);

    // Check requisites
    if (!cond) {
        return CELL_ESRCH;
    }

    // TODO: This is wrong
    cond->cv.notify_all();
    return CELL_OK;
}

HLE_FUNCTION(sys_cond_wait, U32 cond_id, U64 timeout) {
    auto* cond = kernel.objects.get<sys_cond_t>(cond_id);

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

}  // namespace sys
