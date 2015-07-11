/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/system/lv2/sys_synchronization.h"

#include <condition_variable>
#include <mutex>
#include <queue>

namespace sys {

// Constants
enum {
    SYS_EVENT_FLAG_WAIT_AND       = 0x01,
    SYS_EVENT_FLAG_WAIT_OR        = 0x02,
    SYS_EVENT_FLAG_WAIT_CLEAR     = 0x10,
    SYS_EVENT_FLAG_WAIT_CLEAR_ALL = 0x20,

    SYS_EVENT_PORT_LOCAL          = 0x01,

    SYS_PPU_QUEUE                 = 0x01,
    SYS_SPU_QUEUE                 = 0x02,
};

// Classes
struct sys_event_t
{
    BE<U64> source;
    BE<U64> data1;
    BE<U64> data2;
    BE<U64> data3;
};

struct sys_event_flag_attr_t
{
    BE<U32> protocol;
    BE<U32> pshared;
    BE<U64> ipc_key;
    BE<S32> flags;
    BE<S32> type;
    U8 name[8];
};

struct sys_event_queue_attr_t
{
    BE<U32> protocol;
    BE<S32> type;
    U8 name[8];
};

// Auxiliary classes
struct sys_event_flag_t
{
    std::mutex mutex;
    std::condition_variable cv;
    sys_event_flag_attr_t attr;
    U64 value;
};

struct sys_event_queue_t
{
    std::mutex mutex;
    std::condition_variable cv;
    std::queue<sys_event_t> queue;
    sys_event_queue_attr_t attr;
};

struct sys_event_port_t
{
    sys_event_queue_t* equeue = nullptr;
    U32 type;
    union {
        U8 name[8];
        U64 name_value;
    };
};

// SysCalls
S32 sys_event_flag_create(BE<U32>* eflag_id, sys_event_flag_attr_t* attr, U64 init);
S32 sys_event_flag_destroy(U32 eflag_id);
S32 sys_event_flag_wait(U32 eflag_id, U64 bitptn, U32 mode, BE<U64>* result, U64 timeout);
S32 sys_event_flag_trywait(U32 eflag_id, U64 bitptn, U32 mode, BE<U64>* result);
S32 sys_event_flag_set(U32 eflag_id, U64 bitptn);
S32 sys_event_flag_clear(U32 eflag_id, U64 bitptn);
S32 sys_event_flag_cancel(U32 eflag_id, BE<U32>* num);
S32 sys_event_flag_get(U32 eflag_id, BE<U64>* flags);

S32 sys_event_port_create(BE<U32>* eport_id, S32 port_type, U64 name);
S32 sys_event_port_destroy(U32 eport_id);
S32 sys_event_port_connect_local(U32 eport_id, U32 equeue_id);
S32 sys_event_port_disconnect(U32 eport_id);
S32 sys_event_port_send(U32 eport_id, U64 data1, U64 data2, U64 data3);

S32 sys_event_queue_create(BE<U32>* equeue_id, sys_event_queue_attr_t* attr, U64 event_queue_key, S32 size);
S32 sys_event_queue_destroy(U32 equeue_id, S32 mode);
S32 sys_event_queue_receive(U32 equeue_id, sys_event_t* dummy_event, U64 timeout);
S32 sys_event_queue_tryreceive(U32 equeue_id, sys_event_t* event_array, S32 size, BE<S32>* number);
S32 sys_event_queue_drain(U32 equeue_id);

}  // namespace sys
