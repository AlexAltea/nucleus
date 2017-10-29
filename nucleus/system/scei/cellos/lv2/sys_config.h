/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "../hle_macro.h"

namespace sys {

// SysCalls
HLE_FUNCTION(sys_config_add_service_listener);
HLE_FUNCTION(sys_config_close);
HLE_FUNCTION(sys_config_open);

}  // namespace sys
