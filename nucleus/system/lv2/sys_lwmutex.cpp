/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_lwmutex.h"
#include "nucleus/system/lv2.h"
#include "nucleus/emulator.h"

namespace sys {

S32 sys_lwmutex_create(BE<U32>* lwmutex_id, sys_lwmutex_attribute_t* attr) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    // Check requisites
    if (lwmutex_id == memory->ptr(0) || attr == memory->ptr(0)) {
        return CELL_EFAULT;
    }

    // Create lightweight mutex
    auto* lwmutex = new sys_lwmutex_t();
    //lwmutex->attr = *attr; It causes a segfault upon startup

    *lwmutex_id = lv2.objects.add(lwmutex, SYS_LWMUTEX_OBJECT);
    return CELL_OK;
}

S32 sys_lwmutex_destroy(U32 lwmutex_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    if (!lv2.objects.remove(lwmutex_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

S32 sys_lwmutex_lock(U32 lwmutex_id, U64 timeout) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* lwmutex = lv2.objects.get<sys_lwmutex_t>(lwmutex_id);

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

S32 sys_lwmutex_trylock(U32 lwmutex_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* lwmutex = lv2.objects.get<sys_lwmutex_t>(lwmutex_id);

    // Check requisites
    if (!lwmutex) {
        return CELL_ESRCH;
    }

    lwmutex->lwmutex.try_lock();
    return CELL_OK;
}

S32 sys_lwmutex_unlock(U32 lwmutex_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* lwmutex = lv2.objects.get<sys_lwmutex_t>(lwmutex_id);

    // Check requisites
    if (!lwmutex) {
        return CELL_ESRCH;
    }

    lwmutex->lwmutex.unlock();
    return CELL_OK;
}

}  // namespace sys
