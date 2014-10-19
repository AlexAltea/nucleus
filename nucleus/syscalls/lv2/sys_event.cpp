/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_event.h"
#include "nucleus/syscalls/lv2.h"
#include "nucleus/emulator.h"

/**
 * LV2: Event ports
 */
s32 sys_event_port_create(be_t<u32>* eport_id, s32 port_type, u64 name)
{
    // Create event queue
    auto* eport = new sys_event_port_t();

    *eport_id = nucleus.lv2.objects.add(eport, SYS_EVENT_PORT_OBJECT);
    return CELL_OK;
}

s32 sys_event_port_destroy(u32 eport_id)
{
    auto* eport = nucleus.lv2.objects.get<sys_event_queue_t>(eport_id);

    // Check requisites
    if (!eport) {
        return CELL_ESRCH;
    }

    return CELL_OK;
}

s32 sys_event_port_connect_local(u32 eport_id, u32 event_queue_id)
{
    auto* eport = nucleus.lv2.objects.get<sys_event_queue_t>(eport_id);

    // Check requisites
    if (!eport) {
        return CELL_ESRCH;
    }

    return CELL_OK;
}

s32 sys_event_port_disconnect(u32 eport_id)
{
    auto* eport = nucleus.lv2.objects.get<sys_event_queue_t>(eport_id);

    // Check requisites
    if (!eport) {
        return CELL_ESRCH;
    }

    return CELL_OK;
}

s32 sys_event_port_send(u32 eport_id, u64 data1, u64 data2, u64 data3)
{
    auto* eport = nucleus.lv2.objects.get<sys_event_queue_t>(eport_id);

    // Check requisites
    if (!eport) {
        return CELL_ESRCH;
    }

    return CELL_OK;
}

/**
 * LV2: Event queues
 */
s32 sys_event_queue_create(be_t<u32>* equeue_id, sys_event_queue_attr_t* attr, u64 event_queue_key, s32 size)
{
    // Check requisites
    if (size < 1 || size > 127) {
        return CELL_EINVAL;
    }

    // Create event queue
    auto* equeue = new sys_event_queue_t();

    *equeue_id = nucleus.lv2.objects.add(equeue, SYS_EVENT_QUEUE_OBJECT);
    return CELL_OK;
}

s32 sys_event_queue_destroy(u32 equeue_id, s32 mode)
{
    auto* equeue = nucleus.lv2.objects.get<sys_event_queue_t>(equeue_id);

    // Check requisites
    if (!equeue) {
        return CELL_ESRCH;
    }

    return CELL_OK;
}

s32 sys_event_queue_receive(u32 equeue_id, sys_event_t* dummy_event, u64 timeout)
{
    auto* equeue = nucleus.lv2.objects.get<sys_event_queue_t>(equeue_id);

    // Check requisites
    if (!equeue) {
        return CELL_ESRCH;
    }

    return CELL_OK;
}

s32 sys_event_queue_tryreceive(u32 equeue_id, sys_event_t* event_array, s32 size, be_t<u32>* number)
{
    auto* equeue = nucleus.lv2.objects.get<sys_event_queue_t>(equeue_id);

    // Check requisites
    if (!equeue) {
        return CELL_ESRCH;
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
