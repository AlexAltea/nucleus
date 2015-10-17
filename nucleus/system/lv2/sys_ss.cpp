/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_ss.h"
#include "nucleus/system/lv2.h"

namespace sys {

S32 sys_ss_access_control_engine(U32 pid, U64 arg2, U64 arg3) {
    return CELL_OK;
}

}  // namespace sys
