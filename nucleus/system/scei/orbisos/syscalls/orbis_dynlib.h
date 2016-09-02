/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace sys {
namespace scei {
namespace orbis {

/**
 * OrbisOS Syscall 589 (0x24D)
 */
S64 sys_dynlib_dlopen(void* thread, void* args);

/**
 * OrbisOS Syscall 590 (0x24E)
 */
S64 sys_dynlib_dlclose(void* thread, void* args);

/**
 * OrbisOS Syscall 591 (0x24F)
 */
S64 sys_dynlib_dlsym(void* thread, void* args);

/**
 * OrbisOS Syscall 592 (0x250)
 */
S64 sys_dynlib_get_list(void* thread, void* args);

/**
 * OrbisOS Syscall 593 (0x251)
 */
S64 sys_dynlib_get_info(void* thread, void* args);

/**
 * OrbisOS Syscall 594 (0x252)
 */
// TODO: Members below could be wrong
struct dynlib_load_prx_args {
    S08* path;  // const char *
    U64 args;   // size_t
    U64 argp;   // const void *
    U32 flags;  // uint32_t
    U64 pOpt;   // const SceKernelLoadModuleOpt *
    U64 pRes    // int *
};
S64 sys_dynlib_load_prx(void* thread, dynlib_load_prx_args* args);

/**
 * OrbisOS Syscall 595 (0x253)
 */
// TODO: Members below could be wrong
struct dynlib_unload_prx_args {
    S32 handle; // SceKernelModule
    U64 args;   // size_t
    U64 argp;   // const void *
    U32 flags;  // uint32_t
    U64 pOpt;   // const SceKernelUnloadModuleOpt *
    U64 pRes    // int *
};
S64 sys_dynlib_unload_prx(void* thread, dynlib_unload_prx_args* args);

/**
 * OrbisOS Syscall 596 (0x254)
 */
S64 sys_dynlib_do_copy_relocations(void* thread, void* args);

/**
 * OrbisOS Syscall 597 (0x255)
 */
struct dynlib_prepare_dlclose_args {
	S32  handle;
	S32* buffer;
	U64* countAddress;
};
S64 sys_dynlib_prepare_dlclose(void* thread, dynlib_prepare_dlclose_args* args);

/**
 * OrbisOS Syscall 598 (0x256)
 */
S64 sys_dynlib_get_proc_param(void* thread, void* args);

/**
 * OrbisOS Syscall 599 (0x257)
 */
S64 sys_dynlib_process_needed_and_relocate(void* thread, void* args);

/**
 * OrbisOS Syscall 608 (0x260)
 */
S64 sys_dynlib_get_info_ex(void* thread, void* args);

}  // namespace orbis
}  // namespace scei
}  // namespace sys
