/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <vector>
#include <string>

// Forward-declarations
namespace cpu { namespace frontend { namespace ppu { class SPUThread; }}}

namespace sys {

// Classes
struct sys_spu_thread_argument_t {
	U64 arg1;  // SPU GPR3
	U64 arg2;  // SPU GPR4
	U64 arg3;  // SPU GPR5
	U64 arg4;  // SPU GPR6
};

struct sys_spu_thread_attribute_t {
	const S08* name;  // SPU thread name
	U32 nsize;        // SPU thread name size including NULL terminator
	U32 option;       // SPU thread options
};

struct sys_spu_thread_group_attribute_t {
	U32 nsize;            // Name length including NULL terminator
	const S08* name;      // SPU thread group name
	S32 type;             // SPU thread group type
	/*union {
		sys_memory_container_t ct;
	} option;*/
};

struct sys_raw_spu_attribute_t {
    // TODO
};

struct sys_spu_segment_t {
	S32 type;        // Segment type
	U32 ls_start;    // LS start address
	S32 size;        // Size in bytes
	union {
		U32 pa_start;   // COPY segment address
		U32 value;      // FILL segment value
		U64 pad;
	} src;
};

struct sys_spu_image_t {
	U32 type;
	U32 entry_point;
	sys_spu_segment_t*segs;
	S32 nsegs;
};

// Auxiliary classes
struct SPUThread {
    // TODO
};

struct SPUThreadGroup {
    std::string name;
    std::vector<SPUThread> threads;
};

// SysCalls
S32 sys_raw_spu_create_interrupt_tag(S32 id, U32 class_id, U32 hwthread, U32* intrtag);
S32 sys_raw_spu_set_int_mask(S32 id, U32 class_id, U64 mask);
S32 sys_raw_spu_get_int_mask(S32 id, U32 class_id, U64* mask);
S32 sys_raw_spu_set_int_stat(S32 id, U32 class_id, U64 stat);
S32 sys_raw_spu_get_int_stat(S32 id, U32 class_id, U64* stat);
S32 sys_spu_image_get_information(sys_spu_image_t* img/*, TODO */);
S32 sys_spu_image_open(sys_spu_image_t* img, const S08* path);
S32 sys_spu_image_import(sys_spu_image_t* img, const void* src, U32 type);
S32 sys_spu_image_close(sys_spu_image_t* img);
S32 sys_raw_spu_create(S32* id, sys_raw_spu_attribute_t* attr);
S32 sys_raw_spu_destroy(S32 id);
S32 sys_raw_spu_read_puint_mb(S32 id, U32* value);
S32 sys_spu_thread_get_exit_status(U32 id, S32* status);
S32 sys_spu_thread_set_argument(U32 id, sys_spu_thread_argument_t* arg);
S32 sys_spu_initialize(U32 max_usable_spu, U32 max_raw_spu);
S32 sys_spu_thread_group_create(U32* id, U32 num, S32 prio, sys_spu_thread_group_attribute_t* attr);
S32 sys_spu_thread_group_destroy(U32 id);
S32 sys_spu_thread_initialize(U32* thread, U32 group, U32 spu_num, S32* img, sys_spu_thread_attribute_t* attr, sys_spu_thread_argument_t* arg);
S32 sys_spu_thread_group_start(U32 id);
S32 sys_spu_thread_group_suspend(U32 id);
S32 sys_spu_thread_group_resume(U32 id);
S32 sys_spu_thread_group_yield(U32 id);
S32 sys_spu_thread_group_terminate(U32 id, S32 value);
S32 sys_spu_thread_group_join(U32 gid, S32* cause, S32* status);
S32 sys_spu_thread_group_set_priority(U32 id, S32 priority);
S32 sys_spu_thread_group_get_priority(U32 id, S32* priority);
S32 sys_spu_thread_write_ls(U32 id, U32 address, U64 value, U32 type);
S32 sys_spu_thread_read_ls(U32 id, U32 address, U64* value, U32 type);
S32 sys_spu_thread_write_snr(U32 id, S32 number, U32 value);
S32 sys_spu_thread_group_connect_event(U32 id, U32 equeue, U32 et);
S32 sys_spu_thread_group_disconnect_event(U32 id, U32 et);
S32 sys_spu_thread_set_spu_cfg(U32 id, U64 value);
S32 sys_spu_thread_get_spu_cfg(U32 id, U64* value);
S32 sys_spu_thread_write_spu_mb(U32 id, U32 value);
S32 sys_spu_thread_connect_event(U32 id, U32 eq, U32 et, U08 spup);
S32 sys_spu_thread_disconnect_event(U32 id, U32 et, U08 spup);
S32 sys_spu_thread_bind_queue(U32 id, U32 spuq, U32 spuq_num);
S32 sys_spu_thread_unbind_queue(U32 id, U32 spuq_num);
S32 sys_raw_spu_set_spu_cfg(S32 id, U32 value);
S32 sys_raw_spu_get_spu_cfg(S32 id, U32* value);
S32 sys_spu_thread_recover_page_fault(U32 id);
S32 sys_raw_spu_recover_page_fault(S32 id);

}  // namespace sys
