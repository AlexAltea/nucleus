/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

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
    SYS_UNK65_OBJECT                = 0x41,
    SYS_UNK66_OBJECT                = 0x42,
    SYS_UNK67_OBJECT                = 0x43,
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
    SYS_PPU_THREAD_OBJECT           = 0x01,

    // TODO: Are these even objects?
    SYS_SPU_THREAD_OBJECT           = 0x02,
    SYS_SPU_THREAD_GROUP_OBJECT     = 0x03,
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

// Auxiliary classes
struct sys_process_t {
    sys_process_param_t param;
    sys_process_prx_param_t prx_param;
};

// SysCalls
S32 sys_process_getpid();
S32 sys_process_getppid();
S32 sys_process_get_number_of_object(U32 object, BE<U32>* nump);
S32 sys_process_get_id(U32 object, BE<U32>* buffer, U32 size, BE<U32>* set_size);
S32 sys_process_get_paramsfo(U08* buffer);
S32 sys_process_get_sdk_version(U32 pid, BE<U32>* version);
S32 sys_process_get_status(U64 unk);
S32 sys_process_exit(S32 errorcode);
S32 sys_process_kill(U32 pid);
S32 sys_process_wait_for_child(U32 pid, BE<U32>* status, U64 unk);
S32 sys_process_wait_for_child2(U64 unk1, U64 unk2, U64 unk3, U64 unk4, U64 unk5, U64 unk6);
S32 sys_process_detach_child(U64 unk);
S32 sys_process_is_spu_lock_line_reservation_address(U32 addr, U64 flags);
void sys_game_process_exitspawn(U32 path_addr, U32 argv_addr, U32 envp_addr, U32 data_addr, U32 data_size, U32 prio, U64 flags);
void sys_game_process_exitspawn2(U32 path_addr, U32 argv_addr, U32 envp_addr, U32 data_addr, U32 data_size, U32 prio, U64 flags);

}  // namespace sys
