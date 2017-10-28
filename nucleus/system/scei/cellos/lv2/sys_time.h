/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "../lv2_macro.h"

namespace sys {

// SysCalls
LV2_SYSCALL(sys_time_get_timezone, BE<U32>* timezone, BE<U32>* summertime);
LV2_SYSCALL(sys_time_get_current_time, BE<U64>* sec, BE<U64>* nsec);
LV2_SYSCALL(sys_time_get_timebase_frequency);

}  // namespace sys
