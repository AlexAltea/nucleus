/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

// Forward-declarations
namespace cpu { namespace frontend { namespace ppu { class PPUThread; }}}

namespace sys {

// Classes
struct sys_ppu_thread_attr_t {
    BE<U32> entry;
    BE<U32> tls_addr;
};

struct sys_ppu_thread_stack_t {
    BE<U32> addr;
    BE<U32> size;
};

// Auxiliary classes
struct sys_ppu_thread_t {
    cpu::frontend::ppu::PPUThread* thread;
    sys_ppu_thread_stack_t stack;
};

// SysCalls
S32 sys_ppu_thread_create(BE<U64>* thread_id, sys_ppu_thread_attr_t* attr, U64 arg, U64 unk0, S32 prio, U32 stacksize, U64 flags, S8* threadname);
S32 sys_ppu_thread_detach(U64 thread_id);
S32 sys_ppu_thread_exit(S32 errorcode);
void sys_ppu_thread_get_join_state(BE<S32>* isjoinable);
S32 sys_ppu_thread_get_priority(U64 thread_id, BE<S32>* prio);
S32 sys_ppu_thread_get_stack_information(sys_ppu_thread_stack_t* info);
S32 sys_ppu_thread_join(U64 thread_id, BE<U64>* vptr);
S32 sys_ppu_thread_restart();
S32 sys_ppu_thread_rename(U64 thread_id, S8* name);
S32 sys_ppu_thread_set_priority(U64 thread_id, S32 prio);
S32 sys_ppu_thread_start(U64 thread_id);
S32 sys_ppu_thread_stop(U64 thread_id);
void sys_ppu_thread_yield();

}  // namespace sys
