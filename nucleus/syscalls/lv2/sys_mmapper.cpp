/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_mmapper.h"
#include "sys_memory.h"
#include "nucleus/syscalls/lv2.h"
#include "nucleus/emulator.h"

s32 sys_mmapper_allocate_address(u32 size, u64 flags, u32 alignment, u32* alloc_addr)
{
    switch (flags & (SYS_MEMORY_PAGE_SIZE_1M | SYS_MEMORY_PAGE_SIZE_64K)) {
    /*case SYS_MEMORY_PAGE_SIZE_1M:
        *alloc_addr = nucleus.memory.alloc(size, 0x100000);
        break;
    
    case SYS_MEMORY_PAGE_SIZE_64K:
        *alloc_addr = nucleus.memory.alloc(size, 0x10000);
        break;*/

    default:
        //nucleus.log.error(LOG_HLE, "sys_mmapper_allocate_address: Invalid flags");
        return CELL_EINVAL;
    }

    return CELL_OK;
}

s32 sys_mmapper_allocate_shared_memory(u32 size, u64 flags, u32 alignment, u32* alloc_addr)
{
    *alloc_addr = nucleus.memory.alloc(size, alignment);
    return CELL_OK;
}
