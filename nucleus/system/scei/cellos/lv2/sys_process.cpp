/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_process.h"
#include "nucleus/logger/logger.h"
#include "nucleus/emulator.h"
#include "../lv2.h"

namespace sys {

HLE_FUNCTION(sys_process_getpid) {
    return 0x01000500; // TODO
}

HLE_FUNCTION(sys_process_getppid) {
    return 0x01000300; // TODO
}

HLE_FUNCTION(sys_process_exit, S32 errorcode) {
    kernel.getEmulator()->task(NUCLEUS_EVENT_STOP);
    return CELL_OK;
}

HLE_FUNCTION(sys_process_get_paramsfo, U08* buffer) {
    return CELL_OK;
}

HLE_FUNCTION(sys_process_get_sdk_version, U32 pid, BE<U32>* version) {
    if (!version) {
        return CELL_EFAULT;
    }
    *version = kernel.proc.param.sdk_version;
    return CELL_OK;
}

HLE_FUNCTION(sys_process_is_spu_lock_line_reservation_address, U32 addr, U64 flags) {
    logger.warning(LOG_HLE, "LV2 Syscall (0x00E) called: sys_process_is_spu_lock_line_reservation_address");
    return CELL_OK;
}

}  // namespace sys
