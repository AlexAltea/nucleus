/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace sys {

enum {
    SYS_MEMORY_PAGE_SIZE_1M  = 0x400,
    SYS_MEMORY_PAGE_SIZE_64K = 0x200,
};

struct sys_memory_info_t
{
    BE<U32> total_user_memory;
    BE<U32> available_user_memory;
};

struct sys_page_attr_t
{
    BE<U64> attribute;
    BE<U64> access_right;
    BE<U32> page_size;
    BE<U32> pad;
};

// SysCalls
S32 sys_memory_allocate(U32 size, U64 flags, BE<U32>* alloc_addr);
S32 sys_memory_allocate_from_container(U32 size, U32 cid, U64 flags, BE<U32>* alloc_addr);
S32 sys_memory_container_create2();
S32 sys_memory_free(U32 start_addr);
S32 sys_memory_get_page_attribute(U32 addr, sys_page_attr_t* attr);
S32 sys_memory_get_user_memory_size(sys_memory_info_t* mem_info);
S32 sys_memory_container_create(BE<U32>* cid, U32 yield_size);
S32 sys_memory_container_destroy(U32 cid);
S32 sys_memory_container_get_size(sys_memory_info_t* mem_info, U32 cid);
S32 sys_memory_get_user_memory_stat();

}  // namespace sys
