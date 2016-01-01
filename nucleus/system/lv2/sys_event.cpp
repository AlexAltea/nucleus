/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_event.h"
#include "sys_mutex.h"
#include "nucleus/system/lv2.h"
#include "nucleus/cpu/cpu.h"
#include "nucleus/emulator.h"

// Avoid <Windows.h> macro collisions with std::min and std::max
#if defined(NUCLEUS_PLATFORM_UWP) || defined(NUCLEUS_PLATFORM_WINDOWS)
#undef min
#undef max
#endif

#include <algorithm>

namespace sys {

/**
 * LV2: Event flags
 */
S32 sys_event_flag_create(BE<U32>* eflag_id, sys_event_flag_attr_t* attr, U64 init) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    // Check requisites
    if (eflag_id == nucleus.memory->ptr(0) || attr == nucleus.memory->ptr(0)) {
        return CELL_EFAULT;
    }
    if (attr->pshared != SYS_SYNC_PROCESS_SHARED && attr->pshared != SYS_SYNC_NOT_PROCESS_SHARED) {
        return CELL_EINVAL;
    }

    // Create event flag
    auto* eflag = new sys_event_flag_t();
    eflag->attr = *attr;
    eflag->value = init;

    *eflag_id = lv2.objects.add(eflag, SYS_EVENT_FLAG_OBJECT);
    return CELL_OK;
}

S32 sys_event_flag_destroy(U32 eflag_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    if (!lv2.objects.remove(eflag_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

S32 sys_event_flag_wait(U32 eflag_id, U64 bitptn, U32 mode, BE<U64>* result, U64 timeout) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* eflag = lv2.objects.get<sys_event_flag_t>(eflag_id);

    // Check requisites
    if (!eflag) {
        return CELL_ESRCH;
    }
    if (((mode & 0xF) != SYS_EVENT_FLAG_WAIT_AND) && ((mode & 0xF) != SYS_EVENT_FLAG_WAIT_OR)) {
        return CELL_EINVAL;
    }

    std::unique_lock<std::mutex> lock(eflag->mutex);
    bool validCondition;

    // Check if the condition is met, otherwise wait until condition or timeout happens
    if (((mode & SYS_EVENT_FLAG_WAIT_AND) && ((eflag->value & bitptn) == bitptn)) ||
        ((mode & SYS_EVENT_FLAG_WAIT_OR) && (eflag->value & bitptn))) {
        validCondition = true;
    }
    else if (timeout == 0) {
        validCondition = true;
        if (mode & SYS_EVENT_FLAG_WAIT_AND) {
            eflag->cv.wait(lock, [&]{ return (eflag->value & bitptn) == bitptn; });
        } else {
            eflag->cv.wait(lock, [&]{ return (eflag->value & bitptn); });
        }
    }
    else {
        auto rel_time = std::chrono::microseconds(timeout);
        if (mode & SYS_EVENT_FLAG_WAIT_AND) {
            validCondition = eflag->cv.wait_for(lock, rel_time, [&]{ return (eflag->value & bitptn) == bitptn; });
        } else {
            validCondition = eflag->cv.wait_for(lock, rel_time, [&]{ return (eflag->value & bitptn); });
        }
    }

    // Save value if required and exit if timeout occurred
    if (result !=  nucleus.memory->ptr(0)) {
        *result = eflag->value;
    }
    if (!validCondition) {
        return CELL_ETIMEDOUT;
    }

    // Clear the event flag if required
    if (mode & SYS_EVENT_FLAG_WAIT_CLEAR) {
        eflag->value &= ~bitptn;
    }
    if (mode & SYS_EVENT_FLAG_WAIT_CLEAR_ALL) {
        eflag->value = 0;
    }
    return CELL_OK;
}

S32 sys_event_flag_trywait(U32 eflag_id, U64 bitptn, U32 mode, BE<U64>* result) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* eflag = lv2.objects.get<sys_event_flag_t>(eflag_id);

    // Check requisites
    if (!eflag) {
        return CELL_ESRCH;
    }
    if (((mode & 0xF) != SYS_EVENT_FLAG_WAIT_AND) && ((mode & 0xF) != SYS_EVENT_FLAG_WAIT_OR)) {
        return CELL_EINVAL;
    }

    std::unique_lock<std::mutex> lock(eflag->mutex);

    // Save value if required
    if (result ==  nucleus.memory->ptr(0)) {
        *result = eflag->value;
    }

    // Check condition
    if (((mode & SYS_EVENT_FLAG_WAIT_AND) && ((eflag->value & bitptn) == bitptn)) ||
        ((mode & SYS_EVENT_FLAG_WAIT_OR) && (eflag->value & bitptn))) {

        // Clear the event flag if required
        if (mode & SYS_EVENT_FLAG_WAIT_CLEAR) {
            eflag->value &= ~bitptn;
        }
        if (mode & SYS_EVENT_FLAG_WAIT_CLEAR_ALL) {
            eflag->value = 0;
        }
        return CELL_OK;
    }
    return CELL_EBUSY;
}

S32 sys_event_flag_set(U32 eflag_id, U64 bitptn) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* eflag = lv2.objects.get<sys_event_flag_t>(eflag_id);

    // Check requisites
    if (!eflag) {
        return CELL_ESRCH;
    }

    eflag->value = bitptn;
    eflag->cv.notify_all();
    return CELL_OK;
}

S32 sys_event_flag_clear(U32 eflag_id, U64 bitptn) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* eflag = lv2.objects.get<sys_event_flag_t>(eflag_id);

    // Check requisites
    if (!eflag) {
        return CELL_ESRCH;
    }

    std::unique_lock<std::mutex> lock(eflag->mutex);
    eflag->value &= bitptn;
    return CELL_OK;
}

S32 sys_event_flag_cancel(U32 eflag_id, BE<U32>* num) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* eflag = lv2.objects.get<sys_event_flag_t>(eflag_id);

    // Check requisites
    if (!eflag) {
        if (num == nucleus.memory->ptr(0)) {
            num = 0;
        }
        return CELL_ESRCH;
    }

    return CELL_OK;
}

S32 sys_event_flag_get(U32 eflag_id, BE<U64>* flags) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* eflag = lv2.objects.get<sys_event_flag_t>(eflag_id);

    // Check requisites
    if (flags == nucleus.memory->ptr(0)) {
        return CELL_EFAULT;
    }
    if (!eflag) {
        return CELL_ESRCH;
    }

    *flags = eflag->value;
    return CELL_OK;
}

/**
 * LV2: Event ports
 */
S32 sys_event_port_create(BE<U32>* eport_id, S32 port_type, U64 name) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    // Check requisites
    if (eport_id == nucleus.memory->ptr(0)) {
        return CELL_EFAULT;
    }

    // Create event queue
    auto* eport = new sys_event_port_t();
    eport-> type = port_type;
    eport->name_value = name;

    *eport_id = lv2.objects.add(eport, SYS_EVENT_PORT_OBJECT);
    return CELL_OK;
}

S32 sys_event_port_destroy(U32 eport_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    if (!lv2.objects.remove(eport_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

S32 sys_event_port_connect_local(U32 eport_id, U32 equeue_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* eport = lv2.objects.get<sys_event_port_t>(eport_id);
    auto* equeue = lv2.objects.get<sys_event_queue_t>(equeue_id);

    // Check requisites
    if (!eport || !equeue) {
        return CELL_ESRCH;
    }
    if (eport->type != SYS_EVENT_PORT_LOCAL) {
        return CELL_EINVAL;
    }
    if (eport->equeue) {
        return CELL_EISCONN;
    }

    eport->equeue = equeue;
    return CELL_OK;
}

S32 sys_event_port_disconnect(U32 eport_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* eport = lv2.objects.get<sys_event_port_t>(eport_id);

    // Check requisites
    if (!eport) {
        return CELL_ESRCH;
    }

    eport->equeue = nullptr;
    return CELL_OK;
}

S32 sys_event_port_send(U32 eport_id, U64 data1, U64 data2, U64 data3) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* eport = lv2.objects.get<sys_event_port_t>(eport_id);

    // Check requisites
    if (!eport) {
        return CELL_ESRCH;
    }

    sys_event_t evt;
    evt.source = eport->name_value;
    evt.data1 = data1;
    evt.data2 = data2;
    evt.data3 = data3;

    eport->equeue->queue.push(evt);
    eport->equeue->cv.notify_one();
    return CELL_OK;
}

/**
 * LV2: Event queues
 */
S32 sys_event_queue_create(BE<U32>* equeue_id, sys_event_queue_attr_t* attr, U64 event_queue_key, S32 size) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    // Check requisites
    if (equeue_id == nucleus.memory->ptr(0) || attr == nucleus.memory->ptr(0)) {
        return CELL_EFAULT;
    }
    if (size < 1 || size > 127) {
        return CELL_EINVAL;
    }

    // Create event queue
    auto* equeue = new sys_event_queue_t();
    equeue->attr = *attr;

    *equeue_id = lv2.objects.add(equeue, SYS_EVENT_QUEUE_OBJECT);
    return CELL_OK;
}

S32 sys_event_queue_destroy(U32 equeue_id, S32 mode) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    // TODO: What's up with mode?
    if (!lv2.objects.remove(equeue_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

S32 sys_event_queue_receive(U32 equeue_id, sys_event_t* evt, U64 timeout) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* equeue = lv2.objects.get<sys_event_queue_t>(equeue_id);

    // Check requisites
    if (!equeue) {
        return CELL_ESRCH;
    }

    std::unique_lock<std::mutex> lock(equeue->mutex);

    if (equeue->queue.empty()) {
        // Wait until condition or timeout is met
        if (timeout == 0) {
            equeue->cv.wait(lock, [&]{ return equeue->queue.size(); });
        } else {
            auto rel_time = std::chrono::microseconds(timeout);
            if (!equeue->cv.wait_for(lock, rel_time, [&]{ return equeue->queue.size(); })) {
                return CELL_ETIMEDOUT;
            }
        }
    }

    // Get the last event
    *evt = equeue->queue.front();
    equeue->queue.pop();

    // Event data is returned using registers
    auto thread = (cpu::frontend::ppu::PPUThread*)nucleus.cpu->getCurrentThread();
    thread->state->r[4] = evt->source;
    thread->state->r[5] = evt->data1;
    thread->state->r[6] = evt->data2;
    thread->state->r[7] = evt->data3;
    return CELL_OK;
}

S32 sys_event_queue_tryreceive(U32 equeue_id, sys_event_t* event_array, S32 size, BE<S32>* number) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* equeue = lv2.objects.get<sys_event_queue_t>(equeue_id);

    // Check requisites
    if (!equeue) {
        return CELL_ESRCH;
    }

    std::unique_lock<std::mutex> lock(equeue->mutex);

    S32 eventsReceived = std::max((S32)equeue->queue.size(), size);
    *number = eventsReceived;

    for (U32 i = 0; i < eventsReceived; i++) {
        event_array[i] = equeue->queue.front();
        equeue->queue.pop();
    }
    return CELL_OK;
}

S32 sys_event_queue_drain(U32 equeue_id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    auto* equeue = lv2.objects.get<sys_event_queue_t>(equeue_id);

    // Check requisites
    if (!equeue) {
        return CELL_ESRCH;
    }

    return CELL_OK;
}

}  // namespace sys
