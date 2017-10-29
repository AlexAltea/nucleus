/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "../hle_macro.h"

#include <vector>
#include <string>

// Forward-declarations
namespace cpu { namespace frontend { namespace spu { class SPUThread; }}}

namespace sys {

// Enum
enum {
    SYS_SPU_THREAD_GROUP_TYPE_NORMAL                 = 0x00,
    SYS_SPU_THREAD_GROUP_TYPE_SEQUENTIAL             = 0x01,
    SYS_SPU_THREAD_GROUP_TYPE_SYSTEM                 = 0x02,
    SYS_SPU_THREAD_GROUP_TYPE_MEMORY_FROM_CONTAINER  = 0x04,
    SYS_SPU_THREAD_GROUP_TYPE_NON_CONTEXT            = 0x08,
    SYS_SPU_THREAD_GROUP_TYPE_EXCLUSIVE_NON_CONTEXT  = 0x18,
    SYS_SPU_THREAD_GROUP_TYPE_COOPERATE_WITH_SYSTEM  = 0x20,
};

enum {
    SYS_SPU_THREAD_GROUP_EVENT_RUN            = 0x1,
    SYS_SPU_THREAD_GROUP_EVENT_EXCEPTION      = 0x2,
    SYS_SPU_THREAD_GROUP_EVENT_SYSTEM_MODULE  = 0x4,
};

enum {
    SYS_SPU_EXCEPTION_NO_VALUE        =    0x0,
    SYS_SPU_EXCEPTION_DMA_ALIGNMENT   = 0x0001,
    SYS_SPU_EXCEPTION_DMA_COMMAND     = 0x0002,
    SYS_SPU_EXCEPTION_SPU_ERROR       = 0x0004,
    SYS_SPU_EXCEPTION_MFC_FIR         = 0x0008,
    SYS_SPU_EXCEPTION_MFC_SEGMENT     = 0x0010,
    SYS_SPU_EXCEPTION_MFC_STORAGE     = 0x0020,
    SYS_SPU_EXCEPTION_STOP_CALL       = 0x0100,
    SYS_SPU_EXCEPTION_STOP_BREAK      = 0x0200,
    SYS_SPU_EXCEPTION_HALT            = 0x0400,
    SYS_SPU_EXCEPTION_UNKNOWN_SIGNAL  = 0x0800,
    SYS_SPU_EXCEPTION_NON_CONTEXT     = 0x1000,
    SYS_SPU_EXCEPTION_MAT             = 0x2000,
};

enum {
    SYS_SPU_SEGMENT_TYPE_COPY   = 0x0001,
    SYS_SPU_SEGMENT_TYPE_FILL   = 0x0002,
    SYS_SPU_SEGMENT_TYPE_INFO   = 0x0004,
};

enum {
    SYS_SPU_THREAD_BASE_HIGH  = 0x00000000,
    SYS_SPU_THREAD_BASE_LOW   = 0xF0000000,
    SYS_SPU_THREAD_OFFSET     = 0x00100000,
    SYS_SPU_THREAD_LS_BASE    = 0x00000000,
    SYS_SPU_THREAD_SNR1       = 0x0005400C,
    SYS_SPU_THREAD_SNR2       = 0x0005C00C,
};

// Classes
struct sys_spu_thread_argument_t {
    BE<U64> arg1;  // SPU GPR3
    BE<U64> arg2;  // SPU GPR4
    BE<U64> arg3;  // SPU GPR5
    BE<U64> arg4;  // SPU GPR6
};

struct sys_spu_thread_attribute_t {
    const S08* name;   // SPU thread name
    BE<U32> nsize;     // SPU thread name size including NULL terminator
    BE<U32> option;    // SPU thread options
};

struct sys_spu_thread_group_attribute_t {
    BE<U32> nsize;     // Name length including NULL terminator
    const S08* name;   // SPU thread group name
    BE<U32> type;      // SPU thread group type
    /*union {
        sys_memory_container_t ct;
    } option;*/
};

struct sys_raw_spu_attribute_t {
    // TODO
};

struct sys_spu_segment_t {
    BE<S32> type;        // Segment type
    BE<U32> ls_start;    // LS start address
    BE<S32> size;        // Size in bytes
    union {
        BE<U32> pa_start;   // COPY segment address
        BE<U32> value;      // FILL segment value
        BE<U64> pad;
    } src;
};

struct sys_spu_image_t {
    BE<U32> type;
    BE<U32> entry_point;
    BE<U32> segs_addr;
    BE<S32> nsegs;
};

// Auxiliary classes
struct SPUThread;
struct SPUThreadGroup;

struct SPUThread {
    SPUThreadGroup* parent;
    std::string name;
    cpu::frontend::spu::SPUThread* thread;
};

struct SPUThreadGroup {
    std::string name;
    std::vector<SPUThread*> threads;
    S32 prio;
    S32 type;
};

// SysCalls
HLE_FUNCTION(sys_raw_spu_create_interrupt_tag, S32 id, U32 class_id, U32 hwthread, BE<U32>* intrtag);
HLE_FUNCTION(sys_raw_spu_set_int_mask, S32 id, U32 class_id, U64 mask);
HLE_FUNCTION(sys_raw_spu_get_int_mask, S32 id, U32 class_id, BE<U64>* mask);
HLE_FUNCTION(sys_raw_spu_set_int_stat, S32 id, U32 class_id, U64 stat);
HLE_FUNCTION(sys_raw_spu_get_int_stat, S32 id, U32 class_id, BE<U64>* stat);
HLE_FUNCTION(sys_spu_image_get_information, sys_spu_image_t* img/*, TODO */);
HLE_FUNCTION(sys_spu_image_open, sys_spu_image_t* img, const S08* path);
HLE_FUNCTION(sys_spu_image_import, sys_spu_image_t* img, const void* src, U32 type);
HLE_FUNCTION(sys_spu_image_close, sys_spu_image_t* img);
HLE_FUNCTION(sys_raw_spu_create, BE<S32>* id, sys_raw_spu_attribute_t* attr);
HLE_FUNCTION(sys_raw_spu_destroy, S32 id);
HLE_FUNCTION(sys_raw_spu_read_puint_mb, S32 id, BE<U32>* value);
HLE_FUNCTION(sys_spu_thread_get_exit_status, U32 id, BE<S32>* status);
HLE_FUNCTION(sys_spu_thread_set_argument, U32 id, sys_spu_thread_argument_t* arg);
HLE_FUNCTION(sys_spu_initialize, U32 max_usable_spu, U32 max_raw_spu);
HLE_FUNCTION(sys_spu_thread_group_create, BE<U32>* id, U32 num, S32 prio, sys_spu_thread_group_attribute_t* attr);
HLE_FUNCTION(sys_spu_thread_group_destroy, U32 id);
HLE_FUNCTION(sys_spu_thread_initialize, BE<U32>* thread, U32 group, U32 spu_num, sys_spu_image_t* img, sys_spu_thread_attribute_t* attr, sys_spu_thread_argument_t* arg);
HLE_FUNCTION(sys_spu_thread_group_start, U32 id);
HLE_FUNCTION(sys_spu_thread_group_suspend, U32 id);
HLE_FUNCTION(sys_spu_thread_group_resume, U32 id);
HLE_FUNCTION(sys_spu_thread_group_yield, U32 id);
HLE_FUNCTION(sys_spu_thread_group_terminate, U32 id, S32 value);
HLE_FUNCTION(sys_spu_thread_group_join, U32 gid, BE<S32>* cause, BE<S32>* status);
HLE_FUNCTION(sys_spu_thread_group_set_priority, U32 id, S32 priority);
HLE_FUNCTION(sys_spu_thread_group_get_priority, U32 id, BE<S32>* priority);
HLE_FUNCTION(sys_spu_thread_write_ls, U32 id, U32 address, U64 value, U32 type);
HLE_FUNCTION(sys_spu_thread_read_ls, U32 id, U32 address, BE<U64>* value, U32 type);
HLE_FUNCTION(sys_spu_thread_write_snr, U32 id, S32 number, U32 value);
HLE_FUNCTION(sys_spu_thread_group_connect_event, U32 id, U32 eq, U32 et);
HLE_FUNCTION(sys_spu_thread_group_disconnect_event, U32 id, U32 et);
HLE_FUNCTION(sys_spu_thread_set_spu_cfg, U32 id, U64 value);
HLE_FUNCTION(sys_spu_thread_get_spu_cfg, U32 id, BE<U64>* value);
HLE_FUNCTION(sys_spu_thread_write_spu_mb, U32 id, U32 value);
HLE_FUNCTION(sys_spu_thread_connect_event, U32 id, U32 eq, U32 et, U08 spup);
HLE_FUNCTION(sys_spu_thread_disconnect_event, U32 id, U32 et, U08 spup);
HLE_FUNCTION(sys_spu_thread_bind_queue, U32 id, U32 spuq, U32 spuq_num);
HLE_FUNCTION(sys_spu_thread_unbind_queue, U32 id, U32 spuq_num);
HLE_FUNCTION(sys_raw_spu_set_spu_cfg, S32 id, U32 value);
HLE_FUNCTION(sys_raw_spu_get_spu_cfg, S32 id, BE<U32>* value);
HLE_FUNCTION(sys_spu_thread_recover_page_fault, U32 id);
HLE_FUNCTION(sys_raw_spu_recover_page_fault, S32 id);
HLE_FUNCTION(sys_spu_thread_group_connect_event_all_threads, S32 group_id, U32 equeue_id, U64 req, U08* spup);
HLE_FUNCTION(sys_spu_thread_group_disconnect_event_all_threads, S32 group_id, U08 spup);

}  // namespace sys
