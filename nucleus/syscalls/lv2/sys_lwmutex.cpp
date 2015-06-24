/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_lwmutex.h"
#include "nucleus/syscalls/lv2.h"
#include "nucleus/emulator.h"

s32 sys_lwmutex_create(be_t<u32>* lwmutex_id, sys_lwmutex_attribute_t* attr)
{
    // Check requisites
    if (lwmutex_id == nucleus.memory.ptr(0) || attr == nucleus.memory.ptr(0)) {
        return CELL_EFAULT;
    }

    // Create lightweight mutex
    auto* lwmutex = new sys_lwmutex_t();
    //lwmutex->attr = *attr; It causes a segfault upon startup

    *lwmutex_id = nucleus.lv2.objects.add(lwmutex, SYS_LWMUTEX_OBJECT);
    return CELL_OK;
}

s32 sys_lwmutex_destroy(u32 lwmutex_id)
{
    if (!nucleus.lv2.objects.remove(lwmutex_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

s32 sys_lwmutex_lock(u32 lwmutex_id, u64 timeout)
{
    auto* lwmutex = nucleus.lv2.objects.get<sys_lwmutex_t>(lwmutex_id);

    // Check requisites
    if (!lwmutex) {
        return CELL_ESRCH;
    }

    // Maximum value is: 2^48-1
    if (timeout > 0xFFFFFFFFFFFFULL) {
        timeout = 0xFFFFFFFFFFFFULL;
    }

    // Lock lightweight mutex
    if (timeout == 0) {
        lwmutex->lwmutex.lock();
    } else {
        auto rel_time = std::chrono::microseconds(timeout);
        lwmutex->lwmutex.try_lock_for(rel_time);
    }
    return CELL_OK;
}

s32 sys_lwmutex_trylock(u32 lwmutex_id)
{
    auto* lwmutex = nucleus.lv2.objects.get<sys_lwmutex_t>(lwmutex_id);

    // Check requisites
    if (!lwmutex) {
        return CELL_ESRCH;
    }

    lwmutex->lwmutex.try_lock();
    return CELL_OK;
}

s32 sys_lwmutex_unlock(u32 lwmutex_id)
{
    auto* lwmutex = nucleus.lv2.objects.get<sys_lwmutex_t>(lwmutex_id);

    // Check requisites
    if (!lwmutex) {
        return CELL_ESRCH;
    }

    lwmutex->lwmutex.unlock();
    return CELL_OK;
}
