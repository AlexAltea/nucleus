/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_process.h"
#include "nucleus/system/scei/cellos/lv2.h"
#include "nucleus/emulator.h"

namespace sys {

S32 sys_process_getpid() {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    return 0x01000500; // TODO
}

S32 sys_process_getppid() {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    return 0x01000300; // TODO
}

S32 sys_process_exit(S32 errorcode) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    nucleus.task(NUCLEUS_EVENT_STOP);
    return CELL_OK;
}

S32 sys_process_get_paramsfo(U08* buffer) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    return CELL_OK;
}

S32 sys_process_get_sdk_version(U32 pid, BE<U32>* version) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    if (!version) {
        return CELL_EFAULT;
    }
    *version = lv2.proc.param.sdk_version;
    return CELL_OK;
}

}  // namespace sys
