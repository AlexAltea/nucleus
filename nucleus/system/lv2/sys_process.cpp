/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_process.h"
#include "nucleus/system/lv2.h"
#include "nucleus/emulator.h"

namespace sys {

S32 sys_process_getpid()
{
    return 0x01000500; // TODO
}

S32 sys_process_getppid()
{
    return 0x01000300; // TODO
}

S32 sys_process_exit(S32 errorcode)
{
    nucleus.task(NUCLEUS_EVENT_STOP);
    return CELL_OK;
}

S32 sys_process_get_paramsfo(U8* buffer)
{
    return CELL_OK;
}

S32 sys_process_get_sdk_version(U32 pid, BE<U32>* version)
{
    if (!version) {
        return CELL_EFAULT;
    }
    *version = nucleus.lv2.proc.param.sdk_version;
    return CELL_OK;
}

}  // namespace sys
