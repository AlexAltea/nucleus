/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_spu.h"
#include "nucleus/system/scei/cellos/kernel.h"
#include "nucleus/system/scei/cellos/lv2/sys_event.h"
#include "nucleus/cpu/cpu.h"
#include "nucleus/cpu/cell.h"
#include "nucleus/cpu/frontend/spu/spu_decoder.h"
#include "nucleus/cpu/frontend/spu/spu_thread.h"
#include "nucleus/cpu/frontend/spu/spu_state.h"
#include "nucleus/emulator.h"
#include "nucleus/assert.h"

#include <cstring>

namespace sys {

// Addressing
#define SPU_LS_OFFSET(spuNum) \
    (SYS_SPU_THREAD_OFFSET * (spuNum) + SYS_SPU_THREAD_BASE_LOW + SYS_SPU_THREAD_LS_BASE)
#define SPU_SNR1_OFFSET(spuNum) \
    (SYS_SPU_THREAD_OFFSET * (spuNum) + SYS_SPU_THREAD_BASE_LOW + SYS_SPU_THREAD_SNR1)
#define SPU_SNR2_OFFSET(spuNum) \
    (SYS_SPU_THREAD_OFFSET * (spuNum) + SYS_SPU_THREAD_BASE_LOW + SYS_SPU_THREAD_SNR2)

LV2_SYSCALL(sys_spu_initialize, U32 max_usable_spu, U32 max_raw_spu) {
    if (max_usable_spu < max_raw_spu) {
        return CELL_EINVAL;
    }
    // TODO: Further checks
    return CELL_OK;
}

LV2_SYSCALL(sys_spu_thread_group_create, BE<U32>* id, U32 num, S32 prio, sys_spu_thread_group_attribute_t* attr) {
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

    *id = kernel.objects.add(spuThreadGroup, SYS_SPU_THREAD_GROUP_OBJECT);
    return CELL_OK;
}

LV2_SYSCALL(sys_spu_thread_group_destroy, U32 id) {
    // TODO: Check if SPU thread group is busy, if so return CELL_EBUSY.
    if (!kernel.objects.remove(id)) {
        return CELL_ESRCH;
    }
    return CELL_OK;
}

LV2_SYSCALL(sys_spu_thread_initialize, BE<U32>* thread, U32 group, U32 spu_num, sys_spu_image_t* img, sys_spu_thread_attribute_t* attr, sys_spu_thread_argument_t* arg) {
    // Load SPU thread group and perform checks
    auto* spuThreadGroup = kernel.objects.get<SPUThreadGroup>(group);
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
    state->pc = SPU_LS_OFFSET(spu_num) + img->entry_point;
    state->r[3].u64[1] = arg->arg1;
    state->r[4].u64[1] = arg->arg2;
    state->r[5].u64[1] = arg->arg3;
    state->r[6].u64[1] = arg->arg4;

    // Create SPU modules
    for (Size i = 0; i < img->nsegs; i++) {
        const auto& seg = nucleus.memory->ptr<sys_spu_segment_t>(img->segs_addr)[i];
        void* dstAddr = nucleus.memory->ptr(SPU_LS_OFFSET(spu_num) + seg.ls_start);
        void* srcAddr = nucleus.memory->ptr(seg.src.pa_start);
        if (seg.type == SYS_SPU_SEGMENT_TYPE_COPY) {
            memcpy(dstAddr, srcAddr, seg.size);
            // Assuming the it contains executable code
            auto segment = new cpu::frontend::spu::Module(nucleus.cpu.get());
            segment->address = SPU_LS_OFFSET(spu_num) + seg.ls_start;
            segment->size = seg.size;
            static_cast<cpu::Cell*>(nucleus.cpu.get())->spu_modules.push_back(segment);
        }
        if (seg.type == SYS_SPU_SEGMENT_TYPE_FILL) {
            assert_always("Unimplemented");
        }
    }

    *thread = kernel.objects.add(spuThread, SYS_SPU_THREAD_OBJECT);
    return CELL_OK;
}

LV2_SYSCALL(sys_spu_thread_group_connect_event, U32 id, U32 eq, U32 et) {
    auto* spuThreadGroup = kernel.objects.get<SPUThreadGroup>(id);
    auto* eventQueue = kernel.objects.get<sys_event_queue_t>(eq);
    if (!spuThreadGroup || !eventQueue) {
        return CELL_ESRCH;
    }

    switch (et) {
    case SYS_SPU_THREAD_GROUP_EVENT_RUN:
        // TODO: Implement this
        break;
    case SYS_SPU_THREAD_GROUP_EVENT_EXCEPTION:
        // TODO: Implement this
        break;
    default:
        // TODO: This actually returns CELL_OK due to weird implementation reason on a real LV2 kernel.
        return CELL_EINVAL;
    }
    return CELL_OK;
}

LV2_SYSCALL(sys_spu_thread_group_start, U32 id) {
    auto* spuThreadGroup = kernel.objects.get<SPUThreadGroup>(id);
    if (!spuThreadGroup) {
        return CELL_ESRCH;
    }

    for (auto* spuThread : spuThreadGroup->threads) {
        if (spuThread) {
            spuThread->thread->start();
        }
    }
    return CELL_OK;
}

LV2_SYSCALL(sys_spu_thread_group_join, U32 gid, BE<S32>* cause, BE<S32>* status) {
    auto* spuThreadGroup = kernel.objects.get<SPUThreadGroup>(gid);
    if (!spuThreadGroup) {
        return CELL_ESRCH;
    }

    for (auto* spuThread : spuThreadGroup->threads) {
        if (spuThread) {
            spuThread->thread->join();
        }
    }
    // TODO: ?
    return CELL_OK;
}

LV2_SYSCALL(sys_spu_thread_read_ls, U32 id, U32 address, BE<U64>* value, U32 type) {
    auto* spuThread = kernel.objects.get<SPUThread>(id);
    if (!spuThread) {
        return CELL_ESRCH;
    }
    if ((address + type > 0x40000) || (address % type != 0)) {
        return CELL_EINVAL;
    }

    switch (type) {
    case 1: break;  // TODO
    case 2: break;  // TODO
    case 4: break;  // TODO
    case 8: break;  // TODO
    default:
        return CELL_EINVAL;
    }
    return CELL_OK;
}

LV2_SYSCALL(sys_spu_thread_group_connect_event_all_threads, S32 group_id, U32 equeue_id, U64 req, U08* spup) {
    auto* spuThreadGroup = kernel.objects.get<SPUThreadGroup>(group_id);
    auto* eventQueue = kernel.objects.get<sys_event_queue_t>(equeue_id);
    if (!spuThreadGroup || !eventQueue) {
        return CELL_ESRCH;
    }
    if (req == 0) {
        return CELL_EINVAL;
    }

    // TODO: Implement syscall
    return CELL_OK;
}

}  // namespace sys
