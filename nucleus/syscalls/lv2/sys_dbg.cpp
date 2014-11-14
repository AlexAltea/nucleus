/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_dbg.h"
#include "nucleus/syscalls/lv2.h"
#include "nucleus/emulator.h"

#include "nucleus/cpu/cell.h"

s32 sys_dbg_ppu_exception_handler()
{
    return CELL_OK;
}

s32 sys_dbg_get_thread_list(u32 pid, u64* ids, u64* ids_num, u64* all_ids_num)
{
    if (ids_num == nucleus.memory.ptr(0) || all_ids_num == nucleus.memory.ptr(0)) {
        return CELL_LV2DBG_ERROR_DEINVALIDARGUMENTS;
    }

    size_t i = 0;
    for (auto* thread : nucleus.cell) {
        ids[i] = thread->id;
        if (i >= *ids_num) {
            break;
        }
    }

    *all_ids_num = nucleus.cell.size();
    *ids_num = i;
    return CELL_OK;
}
