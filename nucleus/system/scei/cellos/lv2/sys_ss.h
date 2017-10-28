/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "../lv2_macro.h"

namespace sys {

struct sys_ss_psid_t {
};

// SysCalls
LV2_SYSCALL(sys_ss_get_console_id);
LV2_SYSCALL(sys_ss_access_control_engine, U32 pid, U64 arg2, U64 arg3);
LV2_SYSCALL(sys_ss_get_open_psid);

}  // namespace sys
