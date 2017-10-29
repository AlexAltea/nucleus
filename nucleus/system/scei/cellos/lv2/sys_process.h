/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "../hle_macro.h"

namespace sys {

// Process objects
enum
{
    // Official
    SYS_MEM_OBJECT                  = 0x08,
    SYS_INTR_TAG_OBJECT             = 0x0A,
    SYS_INTR_SERVICE_HANDLE_OBJECT  = 0x0B,
    SYS_EVENT_PORT_OBJECT           = 0x0E,
    SYS_TIMER_OBJECT                = 0x11,
    SYS_TRACE_OBJECT                = 0x21,
    SYS_SPUIMAGE_OBJECT             = 0x22,
    SYS_PRX_OBJECT                  = 0x23,
    SYS_SPUPORT_OBJECT              = 0x24,
    SYS_SERVICE_LISTENER_OBJECT_41  = 0x41,  // base system config handle
    SYS_SERVICE_LISTENER_OBJECT_42  = 0x42,  // a system config service listener
    SYS_SERVICE_OBJECT              = 0x43,  // a system config service
    SYS_FS_FD_OBJECT                = 0x73,
    SYS_MUTEX_OBJECT                = 0x85,
    SYS_COND_OBJECT                 = 0x86,
    SYS_RWLOCK_OBJECT               = 0x88,
    SYS_EVENT_QUEUE_OBJECT          = 0x8D,
    SYS_LWMUTEX_OBJECT              = 0x95,
    SYS_SEMAPHORE_OBJECT            = 0x96,
    SYS_LWCOND_OBJECT               = 0x97,
    SYS_EVENT_FLAG_OBJECT           = 0x98,

    // Custom
    SYS_PROCESS_OBJECT              = 0x01,
    SYS_PROGRAM_SEGMENT_OBJECT      = 0x0F,  // ppc program segments
    SYS_UNK_18_OBJECT               = 0x12,  // ? debug related
    SYS_SHARED_MEM_OBJECT           = 0x20,
    SYS_OVERLAY_PRX_OBJECT          = 0x25,
    SYS_UNK_48_OBJECT               = 0x30,  // ? sys usbd
    SYS_RSX_AUDIO_OBJECT            = 0x60,  
    SYS_GSSM_OBJECT                 = 0x65,  // Game Save Storage Manager interface
    SYS_CRYPTO_OBJECT               = 0x78,  // crypto_engine

    // TODO: Are these even objects?
    SYS_PPU_THREAD_OBJECT           = 0x01,
    SYS_SPU_THREAD_OBJECT           = 0x02, // no object with this ID into LV2 kernel
    SYS_SPU_THREAD_GROUP_OBJECT     = 0x04,
};

// ELF file headers
struct sys_process_param_t
{
    BE<U32> size;
    BE<U32> magic;
    BE<U32> version;
    BE<U32> sdk_version;
    BE<S32> primary_prio;
    BE<U32> primary_stacksize;
    BE<U32> malloc_pagesize;
    BE<U32> ppc_seg;
    BE<U32> crash_dump_param_addr;
};

struct sys_process_prx_param_t
{
    BE<U32> size;
    BE<U32> magic;
    BE<U32> version;
    BE<U32> unk0;
    BE<U32> libentstart;
    BE<U32> libentend;
    BE<U32> libstubstart;
    BE<U32> libstubend;
    BE<U16> ver;
    BE<U16> unk1;
    BE<U32> unk2;
};

// process info, size 0x34
struct sys_process_info_t
{
	BE<U32> id;             // 0x00: process ID
	BE<U32> status;         // 0x04: process status
	BE<U32> ppu_thr_count;  // 0x08: PPU thread count
	BE<U32> spu_thr_count;  // 0x0C: SPU thread count
	BE<U32> raw_spu_count;  // 0x10: raw SPU count
	BE<U32> pp_id;          // 0x14: parent process ID
	char *binary_path;      // 0x18: pointer to a char buffer of 0x200 in size, holding process binary path
	BE<U32> total_mem;      // 0x1C: total memory(user space) size in byte
	U08 osabi_type;         // 0x20: OS ABI type
	U08 pad[7];             // 0x21: padding
	BE<U64> intr_mask;      // 0x28: process interrupt bitmap mask
	BE<U32> trace_id;       // 0x30: trace ID, for debugging
};
    
// process param sfo
struct sys_param_sfo_t
{
    U08 flag;           // 0x00: 1(exist), 0xFF(process have no param_sfo, e.g. VSH process)
    char title_id[9];   // 0x01: (8+'/0'), format: "ABCD1234"
    U08 pad[6];         // 0x0A: padding
    BE<U64> param_0;    // 0x10: ? sys_process_param_t.crash_dump_param_addr related
    BE<U64> param_1;    // 0x18: ?
    BE<U64> param_2;    // 0x20: ?
    BE<U64> param_3;    // 0x28: ?
    BE<U64> param_4;    // 0x30: ?
    BE<U64> param_5;    // 0x38: ? sys_process_param_t.crash_dump_param_addr related
};

// process debugging data, size 0x10; arg for sys_process_spawns_a_self2()
struct sys_process_dbg_t
{
	BE<U64> intr_mask;  // 0x00: process interrupt bitmap mask
	BE<U32> trace_id;   // 0x08: trace ID, for debugging
	BE<U32> pad;        // 0x0C: padding
};
    
// Auxiliary classes
struct sys_process_t {
    sys_process_param_t param;
    sys_process_prx_param_t prx_param;
};

// SysCalls
HLE_FUNCTION(sys_process_getpid);
HLE_FUNCTION(sys_process_getppid);
HLE_FUNCTION(sys_process_get_number_of_object, U32 object, BE<U32>* nump);
HLE_FUNCTION(sys_process_get_id, U32 object, BE<U32>* buffer, U32 size, BE<U32>* set_size);
HLE_FUNCTION(sys_process_get_paramsfo, U08* buffer);
HLE_FUNCTION(sys_process_get_sdk_version, U32 pid, BE<U32>* version);
HLE_FUNCTION(sys_process_get_status, U64 unk);
HLE_FUNCTION(sys_process_exit, S32 errorcode);
HLE_FUNCTION(sys_process_kill, U32 pid);
HLE_FUNCTION(sys_process_wait_for_child, U32 pid, BE<U32>* status, U64 unk);
HLE_FUNCTION(sys_process_wait_for_child2, U64 unk1, U64 unk2, U64 unk3, U64 unk4, U64 unk5, U64 unk6);
HLE_FUNCTION(sys_process_detach_child, U64 unk);
HLE_FUNCTION(sys_process_is_spu_lock_line_reservation_address, U32 addr, U64 flags);
HLE_FUNCTION(sys_game_process_exitspawn, U32 path_addr, U32 argv_addr, U32 envp_addr, U32 data_addr, U32 data_size, U32 prio, U64 flags);
HLE_FUNCTION(sys_game_process_exitspawn2, U32 path_addr, U32 argv_addr, U32 envp_addr, U32 data_addr, U32 data_size, U32 prio, U64 flags);
HLE_FUNCTION(sys_process_wait_for_child, BE<U32>* child_proc_id, BE<U32>* status, U32 flag);
HLE_FUNCTION(sys_process_get_status, U32 proc_id);
HLE_FUNCTION(sys_process_detach_child, U32 child_proc_id);
HLE_FUNCTION(sys_process_get_number_of_object, U32 obj_type, BE<U32>* count);
HLE_FUNCTION(sys_process_get_id, U32 obj_type, BE<U32>* id_list, U32 id_list_size, BE<U32>* count);
HLE_FUNCTION(sys_process_kill, U32 proc_id);
HLE_FUNCTION(sys_process_spawn, BE<U32>* proc_id, S32 primary_prio, U32 flags, BE<U32>* stack, U32 stack_size, U64 intr_mask, U32 trace_id);
HLE_FUNCTION(sys_process_exit2, S32 exit_status, BE<U32>* user_data, U32 user_data_size);
HLE_FUNCTION(sys_process_wait_for_child2, BE<U32>* child_proc_id, BE<U32>* status, BE<U32>* data_out, U32 data_out_size, U32 arg_5, U32 flags);
HLE_FUNCTION(sys_process_spawns_a_self, BE<U32>* proc_id, S32 primary_prio, U32 flags, BE<U32>* stack, U32 stack_size, U64 proc_intr_mask, U32 trace_id, U32 mc_id);
HLE_FUNCTION(sys_process_exit3, S32 exit_status, BE<U32>* user_data, U32 user_data_size, U32 flags);
HLE_FUNCTION(sys_process_spawns_a_self2, BE<U32>* proc_id, S32 primary_prio, U32 flags, BE<U32>* stack, U32 stack_size, U32 mc_id, sys_param_sfo_t* param_sfo, sys_process_dbg_t* dbg_data);
HLE_FUNCTION(sys_process_get_number_of_object2, U32 obj_type);
HLE_FUNCTION(sys_process_get_id2, U32 obj_type, BE<U32>* id_list, U32 id_list_size, BE<U32>* count);
HLE_FUNCTION(sys_process_get_ppu_guid);

    
}  // namespace sys
