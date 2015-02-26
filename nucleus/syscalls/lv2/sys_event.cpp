/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_event.h"
#include "sys_mutex.h"
#include "nucleus/syscalls/lv2.h"
#include "nucleus/emulator.h"

#include <algorithm>

/**
 * LV2: Event flags
 */
s32 sys_event_flag_create(be_t<u32>* eflag_id, sys_event_flag_attr_t* attr, u64 init)
{
    // Check requisites
    if (eflag_id == nucleus.memory.ptr(0) || attr == nucleus.memory.ptr(0)) {
        return CELL_EFAULT;
    }
    if (attr->pshared != SYS_SYNC_PROCESS_SHARED && attr->pshared != SYS_SYNC_NOT_PROCESS_SHARED) {
        return CELL_EINVAL;
    }

    // Create event flag
    auto* eflag = new sys_event_flag_t();
    eflag->attr = *attr;
    eflag->value = init;

    *eflag_id = nucleus.lv2.objects.add(eflag, SYS_EVENT_FLAG_OBJECT);
    return CELL_OK;
}

s32 sys_event_flag_destroy(u32 eflag_id)
{
    if (!nucleus.lv2.objects.remove(eflag_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

s32 sys_event_flag_wait(u32 eflag_id, u64 bitptn, u32 mode, be_t<u64>* result, u64 timeout)
{
    auto* eflag = nucleus.lv2.objects.get<sys_event_flag_t>(eflag_id);

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
    if (result !=  nucleus.memory.ptr(0)) {
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

s32 sys_event_flag_trywait(u32 eflag_id, u64 bitptn, u32 mode, be_t<u64>* result)
{
    auto* eflag = nucleus.lv2.objects.get<sys_event_flag_t>(eflag_id);

    // Check requisites
    if (!eflag) {
        return CELL_ESRCH;
    }
    if (((mode & 0xF) != SYS_EVENT_FLAG_WAIT_AND) && ((mode & 0xF) != SYS_EVENT_FLAG_WAIT_OR)) {
        return CELL_EINVAL;
    }

    std::unique_lock<std::mutex> lock(eflag->mutex);

    // Save value if required
    if (result ==  nucleus.memory.ptr(0)) {
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

s32 sys_event_flag_set(u32 eflag_id, u64 bitptn)
{
    auto* eflag = nucleus.lv2.objects.get<sys_event_flag_t>(eflag_id);

    // Check requisites
    if (!eflag) {
        return CELL_ESRCH;
    }

    eflag->value = bitptn;
    eflag->cv.notify_all();
    return CELL_OK;
}

s32 sys_event_flag_clear(u32 eflag_id, u64 bitptn)
{
    auto* eflag = nucleus.lv2.objects.get<sys_event_flag_t>(eflag_id);

    // Check requisites
    if (!eflag) {
        return CELL_ESRCH;
    }

    std::unique_lock<std::mutex> lock(eflag->mutex);
    eflag->value &= bitptn;
    return CELL_OK;
}

s32 sys_event_flag_cancel(u32 eflag_id, be_t<u32>* num)
{
    auto* eflag = nucleus.lv2.objects.get<sys_event_flag_t>(eflag_id);

    // Check requisites
    if (!eflag) {
        if (num == nucleus.memory.ptr(0)) {
            num = 0;
        }
        return CELL_ESRCH;
    }

    return CELL_OK;
}

s32 sys_event_flag_get(u32 eflag_id, be_t<u64>* flags)
{
    auto* eflag = nucleus.lv2.objects.get<sys_event_flag_t>(eflag_id);

    // Check requisites
    if (flags == nucleus.memory.ptr(0)) {
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
s32 sys_event_port_create(be_t<u32>* eport_id, s32 port_type, u64 name)
{
    // Check requisites
    if (eport_id == nucleus.memory.ptr(0)) {
        return CELL_EFAULT;
    }

    // Create event queue
    auto* eport = new sys_event_port_t();
    eport-> type = port_type;
    eport->name_value = name;

    *eport_id = nucleus.lv2.objects.add(eport, SYS_EVENT_PORT_OBJECT);
    return CELL_OK;
}

s32 sys_event_port_destroy(u32 eport_id)
{
    if (!nucleus.lv2.objects.remove(eport_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

s32 sys_event_port_connect_local(u32 eport_id, u32 equeue_id)
{
    auto* eport = nucleus.lv2.objects.get<sys_event_port_t>(eport_id);
    auto* equeue = nucleus.lv2.objects.get<sys_event_queue_t>(equeue_id);

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

s32 sys_event_port_disconnect(u32 eport_id)
{
    auto* eport = nucleus.lv2.objects.get<sys_event_port_t>(eport_id);

    // Check requisites
    if (!eport) {
        return CELL_ESRCH;
    }

    eport->equeue = nullptr;
    return CELL_OK;
}

s32 sys_event_port_send(u32 eport_id, u64 data1, u64 data2, u64 data3)
{
    auto* eport = nucleus.lv2.objects.get<sys_event_port_t>(eport_id);

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
s32 sys_event_queue_create(be_t<u32>* equeue_id, sys_event_queue_attr_t* attr, u64 event_queue_key, s32 size)
{
    // Check requisites
    if (equeue_id == nucleus.memory.ptr(0) || attr == nucleus.memory.ptr(0)) {
        return CELL_EFAULT;
    }
    if (size < 1 || size > 127) {
        return CELL_EINVAL;
    }

    // Create event queue
    auto* equeue = new sys_event_queue_t();
    equeue->attr = *attr;

    *equeue_id = nucleus.lv2.objects.add(equeue, SYS_EVENT_QUEUE_OBJECT);
    return CELL_OK;
}

s32 sys_event_queue_destroy(u32 equeue_id, s32 mode)
{
    // TODO: What's up with mode?
    if (!nucleus.lv2.objects.remove(equeue_id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

s32 sys_event_queue_receive(u32 equeue_id, sys_event_t* evt, u64 timeout)
{
    auto* equeue = nucleus.lv2.objects.get<sys_event_queue_t>(equeue_id);

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
    auto* thread = (cpu::ppu::Thread*)nucleus.cell.getCurrentThread();
    auto* state = thread->state;
    state->gpr[4] = evt->source;
    state->gpr[5] = evt->data1;
    state->gpr[6] = evt->data2;
    state->gpr[7] = evt->data3;
    return CELL_OK;
}

s32 sys_event_queue_tryreceive(u32 equeue_id, sys_event_t* event_array, s32 size, be_t<s32>* number)
{
    auto* equeue = nucleus.lv2.objects.get<sys_event_queue_t>(equeue_id);

    // Check requisites
    if (!equeue) {
        return CELL_ESRCH;
    }

    std::unique_lock<std::mutex> lock(equeue->mutex);

    s32 eventsReceived = std::max((s32)equeue->queue.size(), size);
    *number = eventsReceived;

    for (u32 i = 0; i < eventsReceived; i++) {
        event_array[i] = equeue->queue.front();
        equeue->queue.pop();
    }
    return CELL_OK;
}

s32 sys_event_queue_drain(u32 equeue_id)
{
    auto* equeue = nucleus.lv2.objects.get<sys_event_queue_t>(equeue_id);

    // Check requisites
    if (!equeue) {
        return CELL_ESRCH;
    }

    return CELL_OK;
}
