/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <condition_variable>
#include <mutex>

// Classes
struct sys_event_t
{
    be_t<u64> source;
    be_t<u64> data1;
    be_t<u64> data2;
    be_t<u64> data3;
};

struct sys_event_flag_attr_t
{
    be_t<u32> protocol;
    be_t<u32> pshared;
    be_t<u64> key;
    be_t<s32> flags;
    be_t<s32> type;
    s8 name[8];
};

struct sys_event_queue_attr_t
{
    be_t<u32> protocol;
    be_t<s32> type;
    s8 name[8];
};

// Auxiliary classes
struct sys_event_queue_t
{
    std::mutex mutex;
    std::condition_variable cv;
    sys_event_queue_attr_t attr;
};

struct sys_event_port_t
{
    sys_event_queue_t* queue = nullptr;
    s8 name[8];
};

// SysCalls
s32 sys_event_flag_create(be_t<u32>* eflag_id, sys_event_flag_attr_t* attr, u64 init);
s32 sys_event_flag_destroy(u32 eflag_id);
s32 sys_event_flag_wait(u32 eflag_id, u64 bitptn, u32 mode, be_t<u64>* result, u64 timeout);
s32 sys_event_flag_trywait(u32 eflag_id, u64 bitptn, u32 mode, be_t<u64>* result);
s32 sys_event_flag_set(u32 eflag_id, u64 bitptn);
s32 sys_event_flag_clear(u32 eflag_id, u64 bitptn);
s32 sys_event_flag_cancel(u32 eflag_id, be_t<u32>* num);
s32 sys_event_flag_get(u32 eflag_id, be_t<u64>* flags);

s32 sys_event_port_create(be_t<u32>* eport_id, s32 port_type, u64 name);
s32 sys_event_port_destroy(u32 eport_id);
s32 sys_event_port_connect_local(u32 eport_id, u32 event_queue_id);
s32 sys_event_port_disconnect(u32 eport_id);
s32 sys_event_port_send(u32 eport_id, u64 data1, u64 data2, u64 data3);

s32 sys_event_queue_create(be_t<u32>* equeue_id, sys_event_queue_attr_t* attr, u64 event_queue_key, s32 size);
s32 sys_event_queue_destroy(u32 equeue_id, s32 mode);
s32 sys_event_queue_receive(u32 equeue_id, sys_event_t* dummy_event, u64 timeout);
s32 sys_event_queue_tryreceive(u32 equeue_id, sys_event_t* event_array, s32 size, be_t<u32>* number);
s32 sys_event_queue_drain(u32 equeue_id);
