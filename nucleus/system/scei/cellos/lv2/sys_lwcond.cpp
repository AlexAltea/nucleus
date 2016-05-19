/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_lwcond.h"
#include "nucleus/emulator.h"
#include "nucleus/logger/logger.h"
#include "nucleus/system/scei/cellos/lv2.h"
#include "nucleus/system/scei/cellos/lv2/sys_lwmutex.h"

namespace sys {

S32 sys_lwcond_create(BE<U32>* lwcond_id, U32 lwmutex_id, sys_lwcond_attribute_t* attr) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* lwmutex = lv2.objects.get<sys_lwmutex_t>(lwmutex_id);

    // Check requisites
    if (!lwmutex) {
        return CELL_ESRCH;
    }
    if ((lwcond_id == nucleus.memory->ptr(0)) || 
        (attr == nucleus.memory->ptr(0))) {
        return CELL_EFAULT;
    }

    // Create condition variable
    auto* lwcond = new sys_lwcond_t();
    lwcond->lwmutex = lwmutex;
    lwcond->attr = *attr;

    *lwcond_id = lv2.objects.add(lwcond, SYS_LWCOND_OBJECT);
    return CELL_OK;
}

S32 sys_lwcond_destroy(U32 lwcond_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    if (!lv2.objects.remove(lwcond_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

S32 sys_lwcond_signal(U32 lwcond_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* lwcond = lv2.objects.get<sys_lwcond_t>(lwcond_id);

    // Check requisites
    if (!lwcond) {
        return CELL_ESRCH;
    }

    lwcond->cv.notify_one();
    return CELL_OK;
}

S32 sys_lwcond_signal_all(U32 lwcond_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* lwcond = lv2.objects.get<sys_lwcond_t>(lwcond_id);

    // Check requisites
    if (!lwcond) {
        return CELL_ESRCH;
    }

    lwcond->cv.notify_all();
    return CELL_OK;
}

S32 sys_lwcond_signal_to(U32 lwcond_id, U32 thread_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* lwcond = lv2.objects.get<sys_lwcond_t>(lwcond_id);

    // Check requisites
    if (!lwcond) {
        return CELL_ESRCH;
    }

    // TODO: This is wrong
    lwcond->cv.notify_all();
    return CELL_OK;
}

S32 sys_lwcond_wait(U32 lwcond_id, U64 timeout) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* lwcond = lv2.objects.get<sys_lwcond_t>(lwcond_id);

    // Check requisites
    if (!lwcond) {
        return CELL_ESRCH;
    }

    // Maximum value is: 2^48-1
    if (timeout > 0xFFFFFFFFFFFFULL) {
        timeout = 0xFFFFFFFFFFFFULL;
    }

    std::unique_lock<std::mutex> lock((std::mutex&)lwcond->lwmutex->lwmutex);
    if (timeout == 0) {
        lwcond->cv.wait(lock);
    } else {
        auto rel_time = std::chrono::microseconds(timeout);
        lwcond->cv.wait_for(lock, rel_time);
    }

    return CELL_OK;
}

}  // namespace sys
