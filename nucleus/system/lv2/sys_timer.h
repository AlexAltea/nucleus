/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace sys {

// Classes
struct sys_timer_information_t
{
    S64 next_expiration_time;
    U64 period;
    U32 timer_state;
    U32 pad;
};

// SysCalls
S32 sys_timer_create(BE<U32>* timer_id);
S32 sys_timer_destroy(U32 timer_id);
S32 sys_timer_get_information(U32 timer_id, sys_timer_information_t* info);
S32 sys_timer_start(U32 timer_id, S64 basetime, U64 period);
S32 sys_timer_stop(U32 timer_id);
S32 sys_timer_connect_event_queue(U32 timer_id, U32 queue_id, U64 name, U64 data1, U64 data2);
S32 sys_timer_disconnect_event_queue(U32 timer_id);
S32 sys_timer_usleep(U64 sleep_time);
S32 sys_timer_sleep(U32 sleep_time);

}  // namespace sys
