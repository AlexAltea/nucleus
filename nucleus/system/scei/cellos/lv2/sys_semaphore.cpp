/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_semaphore.h"
#include "sys_mutex.h"
#include "../lv2.h"
#include "nucleus/logger/logger.h"

namespace sys {

HLE_FUNCTION(sys_semaphore_create, BE<U32>* sem_id, sys_semaphore_attribute_t* attr, S32 initial_count, S32 max_count) {
    // Check requisites
    if (sem_id == kernel.memory->ptr(0) || attr == kernel.memory->ptr(0)) {
        return CELL_EFAULT;
    }
    if (attr->pshared != SYS_SYNC_PROCESS_SHARED && attr->pshared != SYS_SYNC_NOT_PROCESS_SHARED) {
        return CELL_EINVAL;
    }
    if ((initial_count < 0 || max_count < initial_count) || max_count <= 0) {
        return CELL_EINVAL;
    }

    // Check for process-shareable flag
    if (attr->pshared == SYS_SYNC_PROCESS_SHARED) {
        logger.warning(LOG_HLE, "Process-shareable semaphores are not supported");
    }

    // Create semaphore
    auto* semaphore = new sys_semaphore_t();
    semaphore->max_count = max_count;
    semaphore->count = initial_count;
    semaphore->attr = *attr;

    *sem_id = kernel.objects.add(semaphore, SYS_SEMAPHORE_OBJECT);
    return CELL_OK;
}

HLE_FUNCTION(sys_semaphore_destroy, U32 sem_id) {
    if (!kernel.objects.remove(sem_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

HLE_FUNCTION(sys_semaphore_get_value, U32 sem_id, BE<S32>* val) {
    auto* semaphore = kernel.objects.get<sys_semaphore_t>(sem_id);

    // Check requisites
    if (!semaphore) {
        return CELL_ESRCH;
    }
    if (val == kernel.memory->ptr(0)) {
        return CELL_EFAULT;
    }

    *val = semaphore->count;
    return CELL_OK;
}

HLE_FUNCTION(sys_semaphore_post, U32 sem_id, S32 val) {
    auto* semaphore = kernel.objects.get<sys_semaphore_t>(sem_id);

    // Check requisites
    if (!semaphore) {
        return CELL_ESRCH;
    }
    if (val < 0) {
        return CELL_EINVAL;
    }
    if (semaphore->count + val > semaphore->max_count) {
        return CELL_EBUSY;
    }

    // Wake up the threads
    std::unique_lock<std::mutex> lock(semaphore->mutex);
    while (val) {
        semaphore->count++;
        semaphore->cv.notify_one();
        val--;
    }
    return CELL_OK;
}

HLE_FUNCTION(sys_semaphore_trywait, U32 sem_id) {
    auto* semaphore = kernel.objects.get<sys_semaphore_t>(sem_id);

    // Check requisites
    if (!semaphore) {
        return CELL_ESRCH;
    }

    // If semaphore count is positive, decrement it and continue
    std::unique_lock<std::mutex> lock(semaphore->mutex);
    if (semaphore->count > 0) {
        semaphore->count--;
        return CELL_OK;
    }
    return CELL_EBUSY;
}

HLE_FUNCTION(sys_semaphore_wait, U32 sem_id, U64 timeout) {
    auto* semaphore = kernel.objects.get<sys_semaphore_t>(sem_id);

    // Check requisites
    if (!semaphore) {
        return CELL_ESRCH;
    }

    std::unique_lock<std::mutex> lock(semaphore->mutex);

    if (semaphore->count <= 0) {
        // Wait until condition or timeout is met
        if (timeout == 0) {
            semaphore->cv.wait(lock, [&]{ return semaphore->count > 0; });
        } else {
            auto rel_time = std::chrono::microseconds(timeout);
            if (!semaphore->cv.wait_for(lock, rel_time, [&]{ return semaphore->count > 0; })) {
                return CELL_ETIMEDOUT;
            }
        }
    }

    semaphore->count--;
    return CELL_OK;
}

}  // namespace sys
