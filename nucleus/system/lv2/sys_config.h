/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace sys {

// SysCalls
S32 sys_config_add_service_listener();
S32 sys_config_close();
S32 sys_config_open();

}  // namespace sys
