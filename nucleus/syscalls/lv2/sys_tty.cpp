/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_tty.h"
#include "nucleus/syscalls/lv2.h"

#include <iostream>

s32 sys_tty_read(s32 ch, s8* buf, s32 len, u32* preadlen)
{
    return CELL_OK;
}

s32 sys_tty_write(s32 ch, s8* buf, s32 len, u32* pwritelen)
{
    if (ch > 15 || len <= 0) {
        return CELL_EINVAL;
    }

    std::cout << std::string(buf, len).c_str();
    return CELL_OK;
}
