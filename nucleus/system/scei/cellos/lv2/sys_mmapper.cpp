/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_mmapper.h"
#include "sys_memory.h"
#include "nucleus/emulator.h"
#include "../lv2.h"

namespace sys {

HLE_FUNCTION(sys_mmapper_allocate_address, U32 size, U64 flags, U32 alignment, U32* alloc_addr) {
    switch (flags & (SYS_MEMORY_PAGE_SIZE_1M | SYS_MEMORY_PAGE_SIZE_64K)) {
    /*case SYS_MEMORY_PAGE_SIZE_1M:
        *alloc_addr = memory->alloc(size, 0x100000);
        break;

    case SYS_MEMORY_PAGE_SIZE_64K:
        *alloc_addr = memory->alloc(size, 0x10000);
        break;*/

    default:
        //logger.error(LOG_HLE, "sys_mmapper_allocate_address: Invalid flags");
        return CELL_EINVAL;
    }

    return CELL_OK;
}

HLE_FUNCTION(sys_mmapper_allocate_shared_memory, U32 size, U64 flags, U32 alignment, U32* alloc_addr) {
    *alloc_addr = kernel.memory->alloc(size, alignment);
    return CELL_OK;
}

}  // namespace sys
