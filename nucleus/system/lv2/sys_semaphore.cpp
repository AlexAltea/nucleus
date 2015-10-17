/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_semaphore.h"
#include "sys_mutex.h"
#include "nucleus/emulator.h"
#include "nucleus/logger/logger.h"
#include "nucleus/system/lv2.h"

namespace sys {

S32 sys_semaphore_create(BE<U32>* sem_id, sys_semaphore_attribute_t* attr, S32 initial_count, S32 max_count) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    // Check requisites
    if (sem_id == memory->ptr(0) || attr == memory->ptr(0)) {
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

    *sem_id = lv2.objects.add(semaphore, SYS_SEMAPHORE_OBJECT);
    return CELL_OK;
}

S32 sys_semaphore_destroy(U32 sem_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    if (!lv2.objects.remove(sem_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

S32 sys_semaphore_get_value(U32 sem_id, BE<S32>* val) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* semaphore = lv2.objects.get<sys_semaphore_t>(sem_id);

    // Check requisites
    if (!semaphore) {
        return CELL_ESRCH;
    }
    if (val == memory->ptr(0)) {
        return CELL_EFAULT;
    }

    *val = semaphore->count;
    return CELL_OK;
}

S32 sys_semaphore_post(U32 sem_id, S32 val) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* semaphore = lv2.objects.get<sys_semaphore_t>(sem_id);

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

S32 sys_semaphore_trywait(U32 sem_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* semaphore = lv2.objects.get<sys_semaphore_t>(sem_id);

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

S32 sys_semaphore_wait(U32 sem_id, U64 timeout) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* semaphore = lv2.objects.get<sys_semaphore_t>(sem_id);

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
