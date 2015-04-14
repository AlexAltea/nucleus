/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "lv1_gpu.h"
#include "nucleus/emulator.h"
#include "nucleus/syscalls/lv2/sys_event.h"

// TEST/HACK: I'm not sure where the event port ID is stored
be_t<u32> eport_handlers;

// LV1 Syscall 217 (0xD9)
s32 lv1_gpu_context_allocate(be_t<u32>* context_id, be_t<u64>* lpar_dma_control, be_t<u64>* lpar_driver_info, be_t<u64>* lpar_reports, u64 mem_ctx, u64 system_mode)
{
    // HACK: We already store data in the memory on RSX initialization, mapping is not necessary
    *lpar_dma_control = 0x40100000;
    *lpar_driver_info = 0x40200000;
    *lpar_reports = 0x40300000;

    // HACK: On the PS3, this is: *context_id = id ^ 0x55555555
    *context_id = 0 ^ 0x55555555;

    // TEST/HACK: I'm not sure if the event queue is created here
    sys_event_queue_attr_t equeue_attr{};
    equeue_attr.protocol = SYS_SYNC_PRIORITY;
    equeue_attr.type = SYS_PPU_QUEUE;
    sys_event_queue_create(&nucleus.rsx.driver_info->handler_queue, &equeue_attr, 0, 0x20);

    // TEST/HACK: I'm not sure if the event port is created/connected here
    sys_event_port_create(&eport_handlers, SYS_EVENT_PORT_LOCAL, 0); // TODO: This might not be SYS_EVENT_PORT_LOCAL.
    sys_event_port_connect_local(eport_handlers, nucleus.rsx.driver_info->handler_queue);

    return LV1_SUCCESS;
}

// LV1 Syscall 225 (0xE1)
s32 lv1_gpu_context_attribute(s32 context_id, u32 operation_code, u64 p1, u64 p2, u64 p3, u64 p4)
{
    switch (operation_code) {
    case L1GPU_CONTEXT_ATTRIBUTE_FIFO_SETUP:
        nucleus.rsx.dma_control->get = p1;
        nucleus.rsx.dma_control->put = p2;
        break;

    case L1GPU_CONTEXT_ATTRIBUTE_DISPLAY_MODE_SET:
    case L1GPU_CONTEXT_ATTRIBUTE_DISPLAY_SYNC:
        break;

    case L1GPU_CONTEXT_ATTRIBUTE_DISPLAY_FLIP:
        nucleus.rsx.driver_info->head[p1].flip |= 0x80000000;
        if (p1 == 0)
            sys_event_port_send(eport_handlers, 0, (1 << 3), 0);
        if (p1 == 1)
            sys_event_port_send(eport_handlers, 0, (1 << 4), 0);
        break;

    case L1GPU_CONTEXT_ATTRIBUTE_DISPLAY_QUEUE:
        nucleus.rsx.driver_info->head[p1].flip |= 0x40000000 | (1 << p2);
        if (p1 == 0)
            sys_event_port_send(eport_handlers, 0, (1 << 5), 0);
        if (p1 == 1)
            sys_event_port_send(eport_handlers, 0, (1 << 6), 0);
        break;

    case L1GPU_CONTEXT_ATTRIBUTE_DISPLAY_BUFFER: {
        u8 id = p1 & 0xFF;
        u32 width = (p2 >> 32) & 0xFFFFFFFF;
        u32 height = p2 & 0xFFFFFFFF;
        u32 pitch = (p3 >> 32) & 0xFFFFFFFF;
        u32 offset = p3 & 0xFFFFFFFF;

        if (id > 7 || width > 0x1000 || height > 0x1000 /* TODO: || offset >= something*/) {
            return LV1_ILLEGAL_PARAMETER_VALUE;
        }

        nucleus.rsx.display[id].width = width;
        nucleus.rsx.display[id].height = height;
        nucleus.rsx.display[id].pitch = pitch;
        nucleus.rsx.display[id].offset = offset;
        break;
    }

    case 0x106: // ? (Used by cellGcmInitPerfMon)
        break;

    case 0x10A: { // ? (Involved in managing flip status through cellGcmResetFlipStatus)
        if (p1 > 7) {
            return LV1_ILLEGAL_PARAMETER_VALUE;
        }
        auto& flipStatus = nucleus.rsx.driver_info->head[p1].flip;
        flipStatus = (flipStatus & p2) | p3;
        break;
    }

    case 0x10D: // Called by cellGcmInitCursor
        break;

    case 0x300: // Tiles
        break;

    case 0x301: // Depth-buffer (Z-cull)
        break;

    case L1GPU_CONTEXT_ATTRIBUTE_FB_SETUP:
    case L1GPU_CONTEXT_ATTRIBUTE_FB_BLIT:
    case L1GPU_CONTEXT_ATTRIBUTE_FB_BLIT_SYNC:
    case L1GPU_CONTEXT_ATTRIBUTE_FB_CLOSE:
        break;

    default:
        return LV1_ILLEGAL_PARAMETER_VALUE;
    }

    return LV1_SUCCESS;
}
