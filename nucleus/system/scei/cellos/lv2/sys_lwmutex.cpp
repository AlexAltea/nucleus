/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_lwmutex.h"
#include "nucleus/system/scei/cellos/kernel.h"
#include "nucleus/emulator.h"

namespace sys {

LV2_SYSCALL(sys_lwmutex_create, BE<U32>* lwmutex_id, sys_lwmutex_attribute_t* attr) {
    // Check requisites
    if (lwmutex_id == nucleus.memory->ptr(0) || attr == nucleus.memory->ptr(0)) {
        return CELL_EFAULT;
    }

    // Create lightweight mutex
    auto* lwmutex = new sys_lwmutex_t();
    //lwmutex->attr = *attr; It causes a segfault upon startup

    *lwmutex_id = kernel.objects.add(lwmutex, SYS_LWMUTEX_OBJECT);
    return CELL_OK;
}

LV2_SYSCALL(sys_lwmutex_destroy, U32 lwmutex_id) {
    if (!kernel.objects.remove(lwmutex_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

LV2_SYSCALL(sys_lwmutex_lock, U32 lwmutex_id, U64 timeout) {
    auto* lwmutex = kernel.objects.get<sys_lwmutex_t>(lwmutex_id);

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
#if defined(NUCLEUS_TARGET_ANDROID) && defined(NUCLEUS_COMPILER_GCC)
        // TODO: Reimplement try_lock_for
        lwmutex->lwmutex.try_lock();
#else
        lwmutex->lwmutex.try_lock_for(rel_time);
#endif
    }
    return CELL_OK;
}

LV2_SYSCALL(sys_lwmutex_trylock, U32 lwmutex_id) {
    auto* lwmutex = kernel.objects.get<sys_lwmutex_t>(lwmutex_id);

    // Check requisites
    if (!lwmutex) {
        return CELL_ESRCH;
    }

    lwmutex->lwmutex.try_lock();
    return CELL_OK;
}

LV2_SYSCALL(sys_lwmutex_unlock, U32 lwmutex_id) {
    auto* lwmutex = kernel.objects.get<sys_lwmutex_t>(lwmutex_id);

    // Check requisites
    if (!lwmutex) {
        return CELL_ESRCH;
    }

    lwmutex->lwmutex.unlock();
    return CELL_OK;
}

}  // namespace sys
