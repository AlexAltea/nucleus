/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

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
    sys_spu_segment_t* segs;
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
S32 sys_raw_spu_create_interrupt_tag(S32 id, U32 class_id, U32 hwthread, BE<U32>* intrtag);
S32 sys_raw_spu_set_int_mask(S32 id, U32 class_id, U64 mask);
S32 sys_raw_spu_get_int_mask(S32 id, U32 class_id, BE<U64>* mask);
S32 sys_raw_spu_set_int_stat(S32 id, U32 class_id, U64 stat);
S32 sys_raw_spu_get_int_stat(S32 id, U32 class_id, BE<U64>* stat);
S32 sys_spu_image_get_information(sys_spu_image_t* img/*, TODO */);
S32 sys_spu_image_open(sys_spu_image_t* img, const S08* path);
S32 sys_spu_image_import(sys_spu_image_t* img, const void* src, U32 type);
S32 sys_spu_image_close(sys_spu_image_t* img);
S32 sys_raw_spu_create(BE<S32>* id, sys_raw_spu_attribute_t* attr);
S32 sys_raw_spu_destroy(S32 id);
S32 sys_raw_spu_read_puint_mb(S32 id, BE<U32>* value);
S32 sys_spu_thread_get_exit_status(U32 id, BE<S32>* status);
S32 sys_spu_thread_set_argument(U32 id, sys_spu_thread_argument_t* arg);
S32 sys_spu_initialize(U32 max_usable_spu, U32 max_raw_spu);
S32 sys_spu_thread_group_create(BE<U32>* id, U32 num, S32 prio, sys_spu_thread_group_attribute_t* attr);
S32 sys_spu_thread_group_destroy(U32 id);
S32 sys_spu_thread_initialize(BE<U32>* thread, U32 group, U32 spu_num, sys_spu_image_t* img, sys_spu_thread_attribute_t* attr, sys_spu_thread_argument_t* arg);
S32 sys_spu_thread_group_start(U32 id);
S32 sys_spu_thread_group_suspend(U32 id);
S32 sys_spu_thread_group_resume(U32 id);
S32 sys_spu_thread_group_yield(U32 id);
S32 sys_spu_thread_group_terminate(U32 id, S32 value);
S32 sys_spu_thread_group_join(U32 gid, BE<S32>* cause, BE<S32>* status);
S32 sys_spu_thread_group_set_priority(U32 id, S32 priority);
S32 sys_spu_thread_group_get_priority(U32 id, BE<S32>* priority);
S32 sys_spu_thread_write_ls(U32 id, U32 address, U64 value, U32 type);
S32 sys_spu_thread_read_ls(U32 id, U32 address, BE<U64>* value, U32 type);
S32 sys_spu_thread_write_snr(U32 id, S32 number, U32 value);
S32 sys_spu_thread_group_connect_event(U32 id, U32 equeue, U32 et);
S32 sys_spu_thread_group_disconnect_event(U32 id, U32 et);
S32 sys_spu_thread_set_spu_cfg(U32 id, U64 value);
S32 sys_spu_thread_get_spu_cfg(U32 id, BE<U64>* value);
S32 sys_spu_thread_write_spu_mb(U32 id, U32 value);
S32 sys_spu_thread_connect_event(U32 id, U32 eq, U32 et, U08 spup);
S32 sys_spu_thread_disconnect_event(U32 id, U32 et, U08 spup);
S32 sys_spu_thread_bind_queue(U32 id, U32 spuq, U32 spuq_num);
S32 sys_spu_thread_unbind_queue(U32 id, U32 spuq_num);
S32 sys_raw_spu_set_spu_cfg(S32 id, U32 value);
S32 sys_raw_spu_get_spu_cfg(S32 id, BE<U32>* value);
S32 sys_spu_thread_recover_page_fault(U32 id);
S32 sys_raw_spu_recover_page_fault(S32 id);

}  // namespace sys
