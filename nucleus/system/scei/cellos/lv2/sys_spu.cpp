/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_spu.h"
#include "nucleus/system/scei/cellos/lv2.h"
#include "nucleus/emulator.h"

namespace sys {

S32 sys_spu_initialize(U32 max_usable_spu, U32 max_raw_spu) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    if (max_usable_spu < max_raw_spu) {
        return CELL_EINVAL;
    }
    // TODO: Further checks
    return CELL_OK;
}

S32 sys_spu_thread_group_create(U32* id, U32 num, S32 prio, sys_spu_thread_group_attribute_t* attr) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());
    return CELL_OK;
}

}  // namespace sys
