/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

// Classes
struct sys_ppu_thread_attr_t
{
    be_t<u32> entry;
    be_t<u32> tls_addr;
};

struct sys_ppu_thread_stack_t
{
    be_t<u32> addr;
    be_t<u32> size;
};

// SysCalls
s32 sys_ppu_thread_create(be_t<u64>* thread_id, sys_ppu_thread_attr_t* entry, u64 arg, u64 unk0, s32 prio, u32 stacksize, u64 flags, s8* threadname);
s32 sys_ppu_thread_detach(u64 thread_id);
s32 sys_ppu_thread_exit(s32 errorcode);
void sys_ppu_thread_get_join_state(be_t<s32>* isjoinable);
s32 sys_ppu_thread_get_priority(u64 thread_id, be_t<s32>* prio);
s32 sys_ppu_thread_get_stack_information(sys_ppu_thread_stack_t* info);
s32 sys_ppu_thread_join(u64 thread_id, be_t<u64>* vptr);
s32 sys_ppu_thread_restart();
s32 sys_ppu_thread_rename(u64 thread_id, s8* name);
s32 sys_ppu_thread_set_priority(u64 thread_id, s32 prio);
s32 sys_ppu_thread_start(u64 thread_id);
s32 sys_ppu_thread_stop(u64 thread_id);
void sys_ppu_thread_yield();
