/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_memory.h"
#include "nucleus/syscalls/lv2.h"
#include "nucleus/emulator.h"

s32 sys_memory_allocate(u32 size, u64 flags, be_t<u32>* alloc_addr)
{
    // Check requisites
    if (alloc_addr == nucleus.memory.ptr(0)) {
        return CELL_EFAULT;
    }

    // Allocate memory
    u32 addr;
    switch (flags) {
    case SYS_MEMORY_PAGE_SIZE_1M:
        if (size & 0xFFFFF) {
            return CELL_EALIGN;
        }
        addr = nucleus.memory.alloc(size, 0x100000);
        break;

    case SYS_MEMORY_PAGE_SIZE_64K:
        if (size & 0xFFFF) {
            return CELL_EALIGN;
        }
        addr = nucleus.memory.alloc(size, 0x10000);
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

s32 sys_memory_allocate_from_container(u32 size, u32 cid, u64 flags, be_t<u32>* alloc_addr)
{
    return CELL_OK;
}

s32 sys_memory_container_create2()
{
    return CELL_OK;
}

s32 sys_memory_free(u32 start_addr)
{
    return CELL_OK;
}

s32 sys_memory_get_page_attribute(u32 addr, sys_page_attr_t* attr)
{
    return CELL_OK;
}

s32 sys_memory_get_user_memory_size(sys_memory_info_t* mem_info)
{
    const MemorySegment& userMemory = nucleus.memory(SEG_USER_MEMORY);
    mem_info->total_user_memory = userMemory.getTotalMemory();
    mem_info->available_user_memory = userMemory.getTotalMemory() - userMemory.getUsedMemory();
    return CELL_OK;
}

s32 sys_memory_container_create(be_t<u32>* cid, u32 yield_size)
{
    return CELL_OK;
}

s32 sys_memory_container_destroy(u32 cid)
{
    return CELL_OK;
}

s32 sys_memory_container_get_size(sys_memory_info_t* mem_info, u32 cid)
{
    return CELL_OK;
}

s32 sys_memory_get_user_memory_stat()
{
    return CELL_OK;
}
