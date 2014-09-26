/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

// SysCalls
s32 sys_time_get_timezone(be_t<u32>* timezone, be_t<u32>* summertime);
s32 sys_time_get_current_time(be_t<u64>* sec, be_t<u64>* nsec);
u64 sys_time_get_timebase_frequency();
