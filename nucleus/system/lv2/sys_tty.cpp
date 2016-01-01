/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_tty.h"
#include "nucleus/system/lv2.h"

#include <iostream>

namespace sys {

S32 sys_tty_read(S32 ch, S8* buf, S32 len, U32* preadlen) {
    return CELL_OK;
}

S32 sys_tty_write(S32 ch, S8* buf, S32 len, U32* pwritelen) {
    if (ch > 15 || len <= 0) {
        return CELL_EINVAL;
    }

    std::cout << std::string(buf, len).c_str();
    return CELL_OK;
}

}  // namespace sys
