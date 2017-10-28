/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "../lv2_macro.h"

namespace sys {

// SysCalls
LV2_SYSCALL(sys_config_add_service_listener);
LV2_SYSCALL(sys_config_close);
LV2_SYSCALL(sys_config_open);

}  // namespace sys
