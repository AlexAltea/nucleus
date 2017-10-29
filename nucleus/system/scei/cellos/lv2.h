/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/memory/guest_virtual/guest_virtual_memory.h"
#include "nucleus/cpu/frontend/ppu/ppu_thread.h"
#include "nucleus/filesystem/filesystem_virtual.h"
#include "nucleus/system/scei/cellos/module.h"
#include "nucleus/system/scei/cellos/syscall.h"
#include "nucleus/system/object.h"
#include "nucleus/system/system.h"

#include "lv2/sys_process.h"
#include "lv2/sys_prx.h"
#include "lv2/sys_rsx.h"

namespace sys {

// LV2 Return Codes
enum {
    CELL_OK            = 0x00000000,

    CELL_EAGAIN        = 0x80010001, // The resource is temporarily unavailable
    CELL_EINVAL        = 0x80010002, // An invalid argument value is specified
    CELL_ENOSYS        = 0x80010003, // The feature is not yet implemented
    CELL_ENOMEM        = 0x80010004, // Memory allocation failure
    CELL_ESRCH         = 0x80010005, // The resource with the specified identifier does not exist
    CELL_ENOENT        = 0x80010006, // The file does not exist
    CELL_ENOEXEC       = 0x80010007, // The file is in unrecognized format
    CELL_EDEADLK       = 0x80010008, // Resource deadlock is avoided
    CELL_EPERM         = 0x80010009, // The operation is not permitted
    CELL_EBUSY         = 0x8001000A, // The device or resource is busy
    CELL_ETIMEDOUT     = 0x8001000B, // The operation is timed out
    CELL_EABORT        = 0x8001000C, // The operation is aborted
    CELL_EFAULT        = 0x8001000D, // Invalid memory access
    CELL_ENOCHILD      = 0x8001000E, // Process has no child(s)
    CELL_ESTAT         = 0x8001000F, // State of the target thread is invalid
    CELL_EALIGN        = 0x80010010, // Alignment is invalid.
    CELL_EKRESOURCE    = 0x80010011, // Shortage of the kernel resources
    CELL_EISDIR        = 0x80010012, // The file is a directory
    CELL_ECANCELED     = 0x80010013, // Operation canceled
    CELL_EEXIST        = 0x80010014, // Entry already exists
    CELL_EISCONN       = 0x80010015, // Port is already connected
    CELL_ENOTCONN      = 0x80010016, // Port is not connected
    CELL_EAUTHFAIL     = 0x80010017, // Program authentication fail
    CELL_ENOTMSELF     = 0x80010018, // The file is not a MSELF
    CELL_ESYSVER       = 0x80010019, // System version error
    CELL_EAUTHFATAL    = 0x8001001A, // Fatal system error
    CELL_EDOM          = 0x8001001B,
    CELL_ERANGE        = 0x8001001C,
    CELL_EILSEQ        = 0x8001001D,
    CELL_EFPOS         = 0x8001001E,
    CELL_EINTR         = 0x8001001F,
    CELL_EFBIG         = 0x80010020,
    CELL_EMLIN         = 0x80010021,
    CELL_ENFILE        = 0x80010022,
    CELL_ENOSPC        = 0x80010023,
    CELL_ENOTTY        = 0x80010024,
    CELL_EPIPE         = 0x80010025,
    CELL_EROFS         = 0x80010026,
    CELL_ESPIPE        = 0x80010027,
    CELL_E2BIG         = 0x80010028,
    CELL_EACCES        = 0x80010029,
    CELL_EBADF         = 0x8001002A,
    CELL_EIO           = 0x8001002B,
    CELL_EMFILE        = 0x8001002C,
    CELL_ENODEV        = 0x8001002D,
    CELL_ENOTDIR       = 0x8001002E,
    CELL_ENXIO         = 0x8001002F,
    CELL_EXDEV         = 0x80010030,
    CELL_EBADMSG       = 0x80010031,
    CELL_EINPROGRESS   = 0x80010032,
    CELL_EMSGSIZE      = 0x80010033,
    CELL_ENAMETOOLONG  = 0x80010034,
    CELL_ENOLCK        = 0x80010035,
    CELL_ENOTEMPTY     = 0x80010036,
    CELL_ENOTSUP       = 0x80010037,
    CELL_EFSSPECIFIC   = 0x80010038,
    CELL_EOVERFLOW     = 0x80010039,
    CELL_ENOTMOUNTED   = 0x8001003A,
    CELL_ENOTSDATA     = 0x8001003B,
};

enum {
    LV2_NONE  = 0,

    // Flags
    LV2_AUTH  = (1 << 0),
    LV2_ROOT  = (1 << 1),
    LV2_DBG   = (1 << 2),

    // FW Type
    LV2_CEX   = (1 << 0),
    LV2_DEX   = (1 << 1),
    LV2_DECR  = (1 << 2),
};

struct LV2Syscall {
    Syscall* func;
    const char* name;
    U32 flags;
};

class LV2 : public System {
    LV2Syscall syscalls[1024];

public:
    ObjectManager objects;
    ModuleManager modules;

    mem::GuestVirtualMemory* memory;

    // Kernel information
    sys_process_t proc;
    sys_rsx_device_t rsx_device[16];
    sys_rsx_context_t rsx_context[4];

    LV2(Emulator* emulator, std::shared_ptr<mem::Memory> memory, U32 fw_type);

    // Start the process at the given entry point
    bool init(U32 entry);

    // Get LV2 SysCall ID from the current thread and call it
    void call(cpu::frontend::ppu::PPUState& state);
};

}  // namespace sys
