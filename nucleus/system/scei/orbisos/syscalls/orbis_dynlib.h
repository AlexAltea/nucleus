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
S64	sys_dynlib_dlopen();

/**
 * OrbisOS Syscall 590 (0x24E)
 */
S64	sys_dynlib_dlclose();

/**
 * OrbisOS Syscall 591 (0x24F)
 */
S64	sys_dynlib_dlsym();

/**
 * OrbisOS Syscall 592 (0x250)
 */
S64	sys_dynlib_get_list();

/**
 * OrbisOS Syscall 593 (0x251)
 */
S64	sys_dynlib_get_info();

/**
 * OrbisOS Syscall 594 (0x252)
 */
S64	sys_dynlib_load_prx();

/**
 * OrbisOS Syscall 595 (0x253)
 */
S64	sys_dynlib_unload_prx();

/**
 * OrbisOS Syscall 596 (0x254)
 */
S64	sys_dynlib_do_copy_relocations();

/**
 * OrbisOS Syscall 597 (0x255)
 */
S64	sys_dynlib_prepare_dlclose();

/**
 * OrbisOS Syscall 598 (0x256)
 */
S64	sys_dynlib_get_proc_param();

/**
 * OrbisOS Syscall 599 (0x257)
 */
S64	sys_dynlib_process_needed_and_relocate();

/**
 * OrbisOS Syscall 608 (0x260)
 */
S64 sys_dynlib_get_info_ex();

}  // namespace orbis
}  // namespace scei
}  // namespace sys
