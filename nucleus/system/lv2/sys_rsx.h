/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace sys {

// SysCalls
S32 sys_rsx_device_open();
S32 sys_rsx_device_close();
S32 sys_rsx_memory_allocate(BE<U32>* mem_handle, BE<U64>* mem_addr, U32 size, U64 flags, U64 a5, U64 a6, U64 a7);
S32 sys_rsx_memory_free(U32 mem_handle);
S32 sys_rsx_context_allocate(BE<U32>* context_id, BE<U64>* lpar_dma_control, BE<U64>* lpar_driver_info, BE<U64>* lpar_reports, U64 mem_ctx, U64 system_mode);
S32 sys_rsx_context_free(U32 context_id);
S32 sys_rsx_context_iomap(U32 context_id, U32 io, U32 ea, U32 size, U64 flags);
S32 sys_rsx_context_iounmap(U32 context_id, U32 a2, U32 io_addr, U32 size);
S32 sys_rsx_context_attribute(S32 context_id, U32 operation_code, U64 p1, U64 p2, U64 p3, U64 p4);
S32 sys_rsx_device_map(BE<U32>* a1, BE<U32>* a2, U32 dev_id);
S32 sys_rsx_device_unmap(U32 dev_id);
S32 sys_rsx_attribute(U32 a1, U32 a2, U32 a3, U32 a4, U32 a5);

}  // namespace sys
