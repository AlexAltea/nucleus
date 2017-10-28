/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "../lv2_macro.h"

namespace sys {

// SysCalls
LV2_SYSCALL(sys_mmapper_allocate_address, U32 size, U64 flags, U32 alignment, U32* alloc_addr);
LV2_SYSCALL(sys_mmapper_allocate_shared_memory, U32 size, U64 flags, U32 alignment, U32* alloc_addr);

}  // namespace sys
