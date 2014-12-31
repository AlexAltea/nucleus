/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

struct sys_ss_psid_t
{

};

// SysCalls
//s32 sys_ss_get_console_id();
s32 sys_ss_access_control_engine(u32 pid, u64 arg2, u64 arg3);
//s32 sys_ss_get_open_psid();