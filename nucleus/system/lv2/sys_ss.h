/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace sys {

struct sys_ss_psid_t {
};

// SysCalls
S32 sys_ss_get_console_id();
S32 sys_ss_access_control_engine(U32 pid, U64 arg2, U64 arg3);
S32 sys_ss_get_open_psid();

}  // namespace sys
