/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

// Process objects
enum
{
    SYS_MEM_OBJECT                  = 0x08,
    SYS_MUTEX_OBJECT                = 0x85,
    SYS_COND_OBJECT                 = 0x86,
    SYS_RWLOCK_OBJECT               = 0x88,
    SYS_INTR_TAG_OBJECT             = 0x0A,
    SYS_INTR_SERVICE_HANDLE_OBJECT  = 0x0B,
    SYS_EVENT_QUEUE_OBJECT          = 0x8D,
    SYS_EVENT_PORT_OBJECT           = 0x0E,
    SYS_TRACE_OBJECT                = 0x21,
    SYS_SPUIMAGE_OBJECT             = 0x22,
    SYS_PRX_OBJECT                  = 0x23,
    SYS_SPUPORT_OBJECT              = 0x24,
    SYS_LWMUTEX_OBJECT              = 0x95,
    SYS_TIMER_OBJECT                = 0x11,
    SYS_SEMAPHORE_OBJECT            = 0x96,
    SYS_FS_FD_OBJECT                = 0x73,
    SYS_LWCOND_OBJECT               = 0x97,
    SYS_EVENT_FLAG_OBJECT           = 0x98,
};

// ELF file header
struct sys_process_param_t
{
    be_t<u32> size;
    be_t<u32> magic;
    be_t<u32> version;
    be_t<u32> sdk_version;
    be_t<s32> primary_prio;
    be_t<u32> primary_stacksize;
    be_t<u32> malloc_pagesize;
    be_t<u32> ppc_seg;
    be_t<u32> crash_dump_param_addr;
};

// SysCalls
s32 sys_process_getpid();
s32 sys_process_getppid();
s32 sys_process_get_number_of_object(u32 object, be_t<u32>* nump);
s32 sys_process_get_id(u32 object, be_t<u32>* buffer, u32 size, be_t<u32>* set_size);
s32 sys_process_get_paramsfo(be_t<u32>* buffer);
s32 sys_process_get_sdk_version(u32 pid, be_t<u32>* version);
s32 sys_process_get_status(u64 unk);
s32 sys_process_exit(s32 errorcode);
s32 sys_process_kill(u32 pid);
s32 sys_process_wait_for_child(u32 pid, be_t<u32>* status, u64 unk);
s32 sys_process_wait_for_child2(u64 unk1, u64 unk2, u64 unk3, u64 unk4, u64 unk5, u64 unk6);
s32 sys_process_detach_child(u64 unk);
void sys_game_process_exitspawn(u32 path_addr, u32 argv_addr, u32 envp_addr, u32 data_addr, u32 data_size, u32 prio, u64 flags);
void sys_game_process_exitspawn2(u32 path_addr, u32 argv_addr, u32 envp_addr, u32 data_addr, u32 data_size, u32 prio, u64 flags);
