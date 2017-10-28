/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "../lv2_macro.h"

namespace sys {

// Kernel structures
struct sys_rsx_device_t {
    S08 init_flag;
    S08 pad[7];
    BE<S64> lpar_size;
    BE<S64> lpar_addr;
    BE<S32> ctx_count;
    BE<S32> unk2;
};

struct sys_rsx_context_t {
    BE<S32> initialized;
    BE<U32> id;
    BE<U32> mem_ctx_id;
    BE<U32> unk;
    BE<U64> system_mode;
    BE<S64> dma_control_unk;
    BE<S64> dma_control_ea;
    BE<S64> driver_info_unk;
    BE<S64> driver_info_ea;
    BE<S64> reports_unk;
    BE<S64> reports_ea;
    BE<U64> unk2;
    BE<U64> addr_unk;
    BE<S64> equeue_addr;
    BE<S64> irq_tag;
    BE<S64> irq_handler;
    BE<U32> equeue_id;
    BE<U32> unk3;
    BE<U64> unk4;
    BE<S64> unk5;
    BE<U16> pages[0x200];
};

// SysCalls
LV2_SYSCALL(sys_rsx_device_open);
LV2_SYSCALL(sys_rsx_device_close);
LV2_SYSCALL(sys_rsx_memory_allocate, BE<U32>* mem_handle, BE<U64>* mem_addr, U32 size, U64 flags, U64 a5, U64 a6, U64 a7);
LV2_SYSCALL(sys_rsx_memory_free, U32 mem_handle);
LV2_SYSCALL(sys_rsx_context_allocate, BE<U32>* context_id, BE<U64>* lpar_dma_control, BE<U64>* lpar_driver_info, BE<U64>* lpar_reports, U64 mem_ctx, U64 system_mode);
LV2_SYSCALL(sys_rsx_context_free, U32 context_id);
LV2_SYSCALL(sys_rsx_context_iomap, U32 context_id, U32 io, U32 ea, U32 size, U64 flags);
LV2_SYSCALL(sys_rsx_context_iounmap, U32 context_id, U32 a2, U32 io_addr, U32 size);
LV2_SYSCALL(sys_rsx_context_attribute, S32 context_id, U32 operation_code, U64 p1, U64 p2, U64 p3, U64 p4);
LV2_SYSCALL(sys_rsx_device_map, BE<U32>* a1, BE<U32>* a2, U32 dev_id);
LV2_SYSCALL(sys_rsx_device_unmap, U32 dev_id);
LV2_SYSCALL(sys_rsx_attribute, U32 a1, U32 a2, U32 a3, U32 a4, U32 a5);

}  // namespace sys
