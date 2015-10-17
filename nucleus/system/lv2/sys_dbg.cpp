/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_dbg.h"
#include "nucleus/system/lv2.h"
#include "nucleus/emulator.h"

namespace sys {

S32 sys_dbg_ppu_exception_handler() {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    return CELL_OK;
}

S32 sys_dbg_get_thread_list(U32 pid, U64* ids, U64* ids_num, U64* all_ids_num) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    if (ids_num == nucleus.memory.ptr(0) || all_ids_num == nucleus.memory.ptr(0)) {
        return CELL_LV2DBG_ERROR_DEINVALIDARGUMENTS;
    }

    size_t i = 0;
    for (auto* ppu_thread : nucleus.cell.ppu_threads) {
        ids[i] = ppu_thread->id;
        if (i >= *ids_num) {
            break;
        }
    }

    *all_ids_num = nucleus.cell.ppu_threads.size();
    *ids_num = i;
    return CELL_OK;
}

}  // namespace sys
