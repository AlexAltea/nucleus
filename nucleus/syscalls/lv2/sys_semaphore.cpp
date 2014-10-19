/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_semaphore.h"
#include "sys_mutex.h"
#include "nucleus/syscalls/lv2.h"
#include "nucleus/emulator.h"

s32 sys_semaphore_create(be_t<u32>* sem_id, sys_semaphore_attribute_t* attr, s32 initial_count, s32 max_count)
{
    // Check requisites
    if (sem_id == nucleus.memory.ptr(0) || attr == nucleus.memory.ptr(0)) {
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
        nucleus.log.warning(LOG_HLE, "Process-shareable semaphores are not supported");
    }

    // Create semaphore
    auto* semaphore = new sys_semaphore_t();
    semaphore->max_count = max_count;
    semaphore->count = initial_count;
    semaphore->attr = *attr;

    *sem_id = nucleus.lv2.objects.add(semaphore, SYS_SEMAPHORE_OBJECT);
    return CELL_OK;
}

s32 sys_semaphore_destroy(u32 sem_id)
{
    if (!nucleus.lv2.objects.remove(sem_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

s32 sys_semaphore_get_value(u32 sem_id, be_t<s32>* val)
{
    auto* semaphore = nucleus.lv2.objects.get<sys_semaphore_t>(sem_id);

    // Check requisites
    if (!semaphore) {
        return CELL_ESRCH;
    }
    if (val == nucleus.memory.ptr(0)) {
        return CELL_EFAULT;
    }

    *val = semaphore->count;
    return CELL_OK;
}

s32 sys_semaphore_post(u32 sem_id, s32 val)
{
    auto* semaphore = nucleus.lv2.objects.get<sys_semaphore_t>(sem_id);

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

s32 sys_semaphore_trywait(u32 sem_id)
{
    auto* semaphore = nucleus.lv2.objects.get<sys_semaphore_t>(sem_id);

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

s32 sys_semaphore_wait(u32 sem_id, u64 timeout)
{
    auto* semaphore = nucleus.lv2.objects.get<sys_semaphore_t>(sem_id);
    
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

    // Wait until condition or timeout is met
    if (timeout == 0) {
        semaphore->cv.wait(lock, [&]{ return semaphore->count > 0; });
    } else {
        auto rel_time = std::chrono::microseconds(timeout);
        semaphore->cv.wait_for(lock, rel_time, [&]{ return semaphore->count > 0; });
    }

    if (semaphore->count == 0) {
        return CELL_ETIMEDOUT;
    }
    semaphore->count--;
    return CELL_OK;
}
