/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_tty.h"

#include <iostream>

s32 sys_tty_read(s32 ch, vm_ptr<s8> buf, s32 len, vm_ptr<u32> preadlen)
{
    return CELL_OK;
}

s32 sys_tty_write(s32 ch, vm_ptr<s8> buf, s32 len, vm_ptr<u32> pwritelen)
{
    if (ch > 15 || len <= 0) {
        return CELL_EINVAL;
    }

    const std::string data(buf.ptr(), len);
    std::cout << data.data();
    *pwritelen.ptr() = data.size();

    return CELL_OK;
}
