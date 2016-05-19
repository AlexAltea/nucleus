/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_spu.h"
#include "nucleus/system/scei/cellos/lv2.h"
#include "nucleus/cpu/cpu.h"
#include "nucleus/cpu/frontend/spu/spu_thread.h"
#include "nucleus/cpu/frontend/spu/spu_state.h"
#include "nucleus/emulator.h"
#include "nucleus/assert.h"

namespace sys {

S32 sys_spu_initialize(U32 max_usable_spu, U32 max_raw_spu) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    if (max_usable_spu < max_raw_spu) {
        return CELL_EINVAL;
    }
    // TODO: Further checks
    return CELL_OK;
}

S32 sys_spu_thread_group_create(BE<U32>* id, U32 num, S32 prio, sys_spu_thread_group_attribute_t* attr) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    // TODO: Check if num is in range specified [1, N] with N := value specified on sys_spu_initialize
    if (num < 1) {
        return CELL_EINVAL;
    }
    if (prio < 16 || 255 < prio) {
        return CELL_EINVAL;
    }
    if ((attr->type != SYS_SPU_THREAD_GROUP_TYPE_NORMAL) &&
        (attr->type != SYS_SPU_THREAD_GROUP_TYPE_MEMORY_FROM_CONTAINER) &&
        (attr->type != SYS_SPU_THREAD_GROUP_TYPE_EXCLUSIVE_NON_CONTEXT)) {
        return CELL_EINVAL;
    }

    assert_true(num <= 255);
    auto* spuThreadGroup = new SPUThreadGroup();
    spuThreadGroup->threads.resize(num);
    spuThreadGroup->prio = prio;
    spuThreadGroup->type = attr->type;
    if (attr->nsize && attr->name) {
        spuThreadGroup->name = attr->name;
    }

    *id = lv2.objects.add(spuThreadGroup, SYS_SPU_THREAD_GROUP_OBJECT);
    return CELL_OK;
}

S32 sys_spu_thread_group_destroy(U32 id) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    // TODO: Check if SPU thread group is busy, if so return CELL_EBUSY.
    if (!lv2.objects.remove(id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

S32 sys_spu_thread_initialize(BE<U32>* thread, U32 group, U32 spu_num, sys_spu_image_t* img, sys_spu_thread_attribute_t* attr, sys_spu_thread_argument_t* arg) {
    LV2& lv2 = static_cast<LV2&>(*nucleus.sys.get());

    // Load SPU thread group and perform checks
    auto* spuThreadGroup = lv2.objects.get<SPUThreadGroup>(group);
    if (!spuThreadGroup) {
        return CELL_ESRCH;
    }
    if (spu_num >= 256) {
        return CELL_EINVAL;
    }
    if (spuThreadGroup->threads[spu_num] != nullptr) {
        return CELL_EBUSY;
    }

    // Create thread
    auto* spuThread = new SPUThread();
    spuThread->parent = spuThreadGroup;
    spuThread->thread = static_cast<cpu::frontend::spu::SPUThread*>(nucleus.cpu->addThread(cpu::THREAD_TYPE_SPU));
    if (0 /*TODO*/) {
        spuThread->name = attr->name;
    }
    spuThreadGroup->threads[spu_num] = spuThread;

    // Set SPU thread initial state
    auto* state = spuThread->thread->state.get();
    state->gpr[3].u64[1] = arg->arg1;
    state->gpr[4].u64[1] = arg->arg2;
    state->gpr[5].u64[1] = arg->arg3;
    state->gpr[6].u64[1] = arg->arg4;

    *thread = lv2.objects.add(spuThread, SYS_SPU_THREAD_OBJECT);
    return CELL_OK;
}

}  // namespace sys
