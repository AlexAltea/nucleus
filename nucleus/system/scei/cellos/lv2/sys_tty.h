/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "../hle_macro.h"

namespace sys {

// TTY channels
enum {
    SYS_TTYP_PPU_STDIN   = 0,
    SYS_TTYP_PPU_STDOUT  = 0,
    SYS_TTYP_PPU_STDERR  = 1,
    SYS_TTYP_SPU_STDOUT  = 2,
    SYS_TTYP_USER1       = 3,
    SYS_TTYP_USER2       = 4,
    SYS_TTYP_USER3       = 5,
    SYS_TTYP_USER4       = 6,
    SYS_TTYP_USER5       = 7,
    SYS_TTYP_USER6       = 8,
    SYS_TTYP_USER7       = 9,
    SYS_TTYP_USER8       = 10,
    SYS_TTYP_USER9       = 11,
    SYS_TTYP_USER10      = 12,
    SYS_TTYP_USER11      = 13,
    SYS_TTYP_USER12      = 14,
    SYS_TTYP_USER13      = 15,
};

// SysCalls
HLE_FUNCTION(sys_tty_read, S32 ch, S08* buf, S32 len, U32* preadlen);
HLE_FUNCTION(sys_tty_write, S32 ch, S08* buf, S32 len, U32* pwritelen);

}  // namespace sys
