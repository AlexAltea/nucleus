/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_memory.h"
#include "../lv2.h"

namespace sys {

HLE_FUNCTION(sys_memory_allocate, U32 size, U64 flags, BE<U32>* alloc_addr) {
    // Check requisites
    if (alloc_addr == kernel.memory->ptr(0)) {
        return CELL_EFAULT;
    }

    // Allocate memory
    U32 addr;
    switch (flags) {
    case SYS_MEMORY_PAGE_SIZE_1M:
        if (size & 0xFFFFF) {
            return CELL_EALIGN;
        }
        addr = kernel.memory->alloc(size, 0x100000);
        break;

    case SYS_MEMORY_PAGE_SIZE_64K:
        if (size & 0xFFFF) {
            return CELL_EALIGN;
        }
        addr = kernel.memory->alloc(size, 0x10000);
        break;

    default:
        return CELL_EINVAL;
    }

    if (!addr) {
        return CELL_ENOMEM;
    }
    *alloc_addr = addr;
    return CELL_OK;
}

HLE_FUNCTION(sys_memory_allocate_from_container, U32 size, U32 cid, U64 flags, BE<U32>* alloc_addr) {
    return CELL_OK;
}

HLE_FUNCTION(sys_memory_container_create2) {
    return CELL_OK;
}

HLE_FUNCTION(sys_memory_free, U32 start_addr) {
    return CELL_OK;
}

HLE_FUNCTION(sys_memory_get_page_attribute, U32 addr, sys_page_attr_t* attr) {
    return CELL_OK;
}

HLE_FUNCTION(sys_memory_get_user_memory_size, sys_memory_info_t* mem_info) {
    const auto& userMemory = kernel.memory->getSegment(mem::SEG_USER_MEMORY);
    mem_info->total_user_memory = userMemory.getTotalMemory();
    mem_info->available_user_memory = userMemory.getTotalMemory() - userMemory.getUsedMemory();
    return CELL_OK;
}

HLE_FUNCTION(sys_memory_container_create, BE<U32>* cid, U32 yield_size) {
    return CELL_OK;
}

HLE_FUNCTION(sys_memory_container_destroy, U32 cid) {
    return CELL_OK;
}

HLE_FUNCTION(sys_memory_container_get_size, sys_memory_info_t* mem_info, U32 cid) {
    return CELL_OK;
}

HLE_FUNCTION(sys_memory_get_user_memory_stat) {
    return CELL_OK;
}

}  // namespace sys
