/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

enum
{
    SYS_MEMORY_PAGE_SIZE_1M  = 0x400,
    SYS_MEMORY_PAGE_SIZE_64K = 0x200,
};

struct sys_memory_info_t
{
    be_t<u32> total_user_memory;
    be_t<u32> available_user_memory;
};

struct sys_page_attr_t
{
    be_t<u64> attribute;
    be_t<u64> access_right;
    be_t<u32> page_size;
    be_t<u32> pad;
};

// SysCalls
s32 sys_memory_allocate(u32 size, u64 flags, be_t<u32>* alloc_addr);
s32 sys_memory_allocate_from_container(u32 size, u32 cid, u64 flags, be_t<u32>* alloc_addr);
s32 sys_memory_container_create2();
s32 sys_memory_free(u32 start_addr);
s32 sys_memory_get_page_attribute(u32 addr, sys_page_attr_t* attr);
s32 sys_memory_get_user_memory_size(sys_memory_info_t* mem_info);
s32 sys_memory_container_create(be_t<u32>* cid, u32 yield_size);
s32 sys_memory_container_destroy(u32 cid);
s32 sys_memory_container_get_size(sys_memory_info_t* mem_info, u32 cid);
s32 sys_memory_get_user_memory_stat();
