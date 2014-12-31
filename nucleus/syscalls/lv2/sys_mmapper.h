/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

// SysCalls
s32 sys_mmapper_allocate_address(u32 size, u64 flags, u32 alignment, u32* alloc_addr);
s32 sys_mmapper_allocate_shared_memory(u32 size, u64 flags, u32 alignment, u32* alloc_addr);
