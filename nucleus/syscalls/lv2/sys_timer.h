/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

// Classes
struct sys_timer_information_t
{
    s64 next_expiration_time;
    u64 period;
    u32 timer_state;
    u32 pad;
};

// SysCalls
s32 sys_timer_create(be_t<u32>* timer_id);
s32 sys_timer_destroy(u32 timer_id);
s32 sys_timer_get_information(u32 timer_id, sys_timer_information_t* info);
s32 sys_timer_start(u32 timer_id, s64 basetime, u64 period);
s32 sys_timer_stop(u32 timer_id);
s32 sys_timer_connect_event_queue(u32 timer_id, u32 queue_id, u64 name, u64 data1, u64 data2);
s32 sys_timer_disconnect_event_queue(u32 timer_id);
s32 sys_timer_usleep(u64 sleep_time);
s32 sys_timer_sleep(u32 sleep_time);
