/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "rsx.h"
#include "nucleus/emulator.h"
#include "nucleus/config.h"

#include "nucleus/gpu/rsx_enum.h"
#include "nucleus/gpu/rsx_methods.h"
#include "nucleus/gpu/rsx_vp.h"

#include "nucleus/gpu/opengl/opengl_renderer.h"

// Method matching
#define case_2(offset, step) \
    case offset: \
    case offset + step:
#define case_4(offset, step) \
    case_2(offset, step) \
    case_2(offset + 2*step, step)
#define case_8(offset, step) \
    case_4(offset, step) \
    case_4(offset + 4*step, step)
#define case_16(offset, step) \
    case_8(offset, step) \
    case_8(offset + 8*step, step)
#define case_range(n, baseOffset, step) \
    case_##n(baseOffset, step) \
    index = (offset - baseOffset) / step;

void RSX::init() {
    // HACK: We store the data in memory (the PS3 stores the data in the GPU and maps it later through a LV2 syscall)
    nucleus.memory(SEG_RSX_MAP_MEMORY).allocFixed(0x40000000, 0x1000);
    nucleus.memory(SEG_RSX_MAP_MEMORY).allocFixed(0x40100000, 0x1000);
    nucleus.memory(SEG_RSX_MAP_MEMORY).allocFixed(0x40200000, 0x4000);
    nucleus.memory(SEG_RSX_MAP_MEMORY).allocFixed(0x40300000, 0x10000);

    // Device
    device = nucleus.memory.ptr<rsx_device_t>(0x40000000);

    // Context
    dma_control = nucleus.memory.ptr<rsx_dma_control_t>(0x40100000);
    driver_info = nucleus.memory.ptr<rsx_driver_info_t>(0x40200000);
    reports = nucleus.memory.ptr<rsx_reports_t>(0x40300000);

    // Write driver information
    driver_info->version_driver = 0x211;
    driver_info->version_gpu = 0x5C;
    driver_info->memory_size = 0xFE00000;       // 254 MB
    driver_info->nvcore_frequency = 500000000;  // 500 MHz
    driver_info->memory_frequency = 650000000;  // 650 MHz

    // Prevent the FIFO from fetching commands
    dma_control->get = 0;
    dma_control->put = 0;

    m_pfifo_thread = new std::thread([&](){
        task();
    });
}

void RSX::task() {
    // Initialize renderer
    switch (config.gpuBackend) {
    case GPU_BACKEND_OPENGL:
        pgraph = new PGRAPH_OpenGL();
    }

    while (true) {
        // Wait until GET and PUT are different
        while (dma_control->get == dma_control->put) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        const u32 get = dma_control->get;
        const u32 put = dma_control->put;

        const rsx_method_t cmd = { nucleus.memory.read32(io_address + get) };

        // Branching commands
        if (cmd.flag_jump) {
            dma_control->get = cmd.jump_offset;
            continue;
        }
        if (cmd.flag_call) {
            m_pfifo_stack.push(get + 4);
            dma_control->get = cmd.call_offset << 2;
            continue;
        }
        if (cmd.flag_ret) {
            dma_control->get = m_pfifo_stack.top();
            m_pfifo_stack.pop();
            continue;
        }

        for (int i = 0; i < cmd.method_count; i++) {
            const u32 offset = (cmd.method_offset << 2) + (cmd.flag_ni ? 0 : 4*i);
            const u32 parameter = nucleus.memory.read32(io_address + get + 4*(i+1));
            //nucleus.log.notice(LOG_GPU, "METHOD: 0x%X;  IO: 0x%X;  PARAM: 0x%X", offset, get, parameter);
            method(offset, parameter);
        }

        dma_control->get += 4 * (cmd.method_count + 1);
    }

    // Finish renderer
    delete pgraph;
}

void RSX::method(u32 offset, u32 parameter)
{
    // Slot used on multiple-register methods
    u32 index;

    switch (offset) {
    // NV40_CHANNEL_DMA
    case NV406E_SET_REFERENCE:
        dma_control->ref = parameter;
        break;

    case NV406E_SET_CONTEXT_DMA_SEMAPHORE:
        dma_semaphore = parameter;
        break;

    case NV406E_SEMAPHORE_OFFSET:
        dma_semaphore_offset = parameter;
        break;

    case NV406E_SEMAPHORE_ACQUIRE:
        while (dma_read32(dma_semaphore, dma_semaphore_offset) != parameter) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            break; // HACK: All processes get stuck here, probably due to the lack of context switching.
        }
        break;

    case NV406E_SEMAPHORE_RELEASE:
        dma_write32(dma_semaphore, dma_semaphore_offset, parameter);
        break;

    // NV40_CURIE_PRIMITIVE
    case NV4097_SET_DITHER_ENABLE:
    case NV4097_SET_ALPHA_TEST_ENABLE:
    case NV4097_SET_STENCIL_TEST_ENABLE:
    case NV4097_SET_DEPTH_TEST_ENABLE:
    case NV4097_SET_CULL_FACE_ENABLE:
    case NV4097_SET_BLEND_ENABLE:
    case NV4097_SET_POLY_OFFSET_FILL_ENABLE:
    case NV4097_SET_POLY_OFFSET_LINE_ENABLE:
    case NV4097_SET_POLY_OFFSET_POINT_ENABLE:
    case NV4097_SET_LOGIC_OP_ENABLE:
    case NV4097_SET_SPECULAR_ENABLE:
    case NV4097_SET_LINE_SMOOTH_ENABLE:
    case NV4097_SET_POLY_SMOOTH_ENABLE:
        pgraph->Enable(offset, parameter);
        break;

    case NV4097_SET_ALPHA_FUNC:
        pgraph->alpha_func = parameter;
        pgraph->AlphaFunc(pgraph->alpha_func, pgraph->alpha_ref);
        break;
        
    case NV4097_SET_ALPHA_REF:
        pgraph->alpha_ref = parameter;
        pgraph->AlphaFunc(pgraph->alpha_func, pgraph->alpha_ref);
        break;

    case_range(8, NV4097_SET_TRANSFORM_PROGRAM_UNK0, 16)
        pgraph->vp_data[pgraph->vp_load].unk0 = parameter;
        break;

    case_range(8, NV4097_SET_TRANSFORM_PROGRAM_UNK1, 16)
        pgraph->vp_data[pgraph->vp_load].unk1 = parameter;
        break;

    case_range(8, NV4097_SET_TRANSFORM_PROGRAM_UNK2, 16)
        pgraph->vp_data[pgraph->vp_load].unk2 = parameter;
        break;

    case_range(8, NV4097_SET_TRANSFORM_PROGRAM_UNK3, 16)
        pgraph->vp_data[pgraph->vp_load].unk3 = parameter;
        pgraph->vp_load += 1;
        break;

    case NV4097_SET_TRANSFORM_PROGRAM_LOAD:
        pgraph->vp_load = parameter;
        break;

    case NV4097_SET_TRANSFORM_PROGRAM_START:
        pgraph->vp_start = parameter;
        break;

    case_range(16, NV4097_SET_VERTEX_DATA_ARRAY_FORMAT, 4)
        pgraph->vp_attr[index].type = parameter & 0xF;
        pgraph->vp_attr[index].size = (parameter >> 4) & 0xF;
        pgraph->vp_attr[index].stride = (parameter >> 8) & 0xFF;
        pgraph->vp_attr[index].frequency = (parameter >> 16);
        pgraph->vp_attr[index].dirty = true;
        break;

    case_range(16, NV4097_SET_VERTEX_DATA_ARRAY_OFFSET, 4)
        pgraph->vp_attr[index].offset = parameter & 0x7FFFFFFF;
        pgraph->vp_attr[index].location = (parameter >> 31);
        pgraph->vp_attr[index].dirty = true;
        break;

    case NV4097_SET_VERTEX_DATA_BASE_OFFSET:
        pgraph->vertex_data_base_offset = parameter;
        break;

    case NV4097_SET_VERTEX_DATA_BASE_INDEX:
        pgraph->vertex_data_base_index = parameter;
        break;

    case NV4097_SET_BEGIN_END:
        if (parameter) {
            pgraph->Begin(parameter);
        } else {
            pgraph->End();
        }
        break;

    case NV4097_DRAW_ARRAYS: {
        const u32 first = parameter & 0xFFFFFF;
        const u32 count = (parameter >> 24) + 1;
        pgraph->LoadVertexAttributes(first, count);
        pgraph->BindVertexAttributes();
        pgraph->DrawArrays(0, first, count);
        pgraph->UnbindVertexAttributes();
        break;
    }

    // SCE DRIVER
    case SCE_DRIVER_FLIP:
        pgraph->Flip();
        driver_info->flip |= 0x80000000;

    //default:
        //nucleus.log.notice(LOG_GPU, "METHOD: 0x%X; PARAM: 0x%X", offset, parameter);
    }
}

DMAObject RSX::dma_address(u32 dma_object)
{
    // NOTE: RAMIN is not emulated, therefore DMA Objects are hardcoded in this function
    switch (dma_object) {
    case RSX_CONTEXT_DMA_REPORT_LOCATION_LOCAL:
        return DMAObject{0x40101400, 0x8000, DMAObject::READ};
    case RSX_CONTEXT_DMA_DEVICE_RW:
        return DMAObject{0x40000000, 0x1000, DMAObject::READWRITE};
    case RSX_CONTEXT_DMA_DEVICE_R:
        return DMAObject{0x40000000, 0x1000, DMAObject::READ};
    case RSX_CONTEXT_DMA_SEMAPHORE_RW:
        return DMAObject{0x40100000, 0x1000, DMAObject::READWRITE};
    case RSX_CONTEXT_DMA_SEMAPHORE_R:
        return DMAObject{0x40100000, 0x1000, DMAObject::READ};
    default:
        nucleus.log.warning(LOG_GPU, "Unknown DMA object (0x%08X)", dma_object);
        return DMAObject{};
    }
}

u32 RSX::dma_read32(u32 dma_object, u32 offset)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::READ) {
        return nucleus.memory.read32(dma.addr + offset);
    }

    nucleus.log.warning(LOG_GPU, "Illegal DMA read");
    return 0;
}

void RSX::dma_write32(u32 dma_object, u32 offset, u32 value)
{
    const DMAObject& dma = dma_address(dma_object);

    if (dma.addr && dma.flags & DMAObject::WRITE) {
        nucleus.memory.write32(dma.addr + offset, value);
    }

    nucleus.log.warning(LOG_GPU, "Illegal DMA write");
}
