/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "rsx.h"
#include "nucleus/emulator.h"
#include "nucleus/config.h"

#include "nucleus/gpu/rsx_methods.h"
#include "nucleus/gpu/rsx_vp.h"

#include "nucleus/gpu/opengl/opengl_renderer.h"

void RSX::init() {
    // HACK: We store the data in memory (the PS3 stores the data in the GPU and maps it later through a LV2 syscall)
    nucleus.memory(SEG_RSX_MAP_MEMORY).allocFixed(0x60100000, 0x1000);
    nucleus.memory(SEG_RSX_MAP_MEMORY).allocFixed(0x60200000, 0x4000);
    nucleus.memory(SEG_RSX_MAP_MEMORY).allocFixed(0x60300000, 0x10000);

    dma_control = nucleus.memory.ptr<rsx_dma_control_t>(0x60100000);
    driver_info = nucleus.memory.ptr<rsx_driver_info_t>(0x60200000);
    reports = nucleus.memory.ptr<rsx_reports_t>(0x60300000);

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
        m_renderer = new RSXRendererOpenGL();
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

        //nucleus.log.notice(LOG_GPU, "METHOD: 0x%X @ IO:0x%X", cmd.method_offset << 2, cmd.method_count, get);
        for (int i = 0; i < cmd.method_count; i++) {
            const u32 argument = nucleus.memory.read32(io_address + get + 4*(i+1));
            //nucleus.log.notice(LOG_GPU, "    > 0x%X", argument);
            regs[cmd.method_offset + i * cmd.flag_ni] = argument;
        }

        const u32 offset = cmd.method_offset << 2;
        const u32 count = cmd.method_count;
        const be_t<u32>* arguments = nucleus.memory.ptr<be_t<u32>>(io_address + get + 4);
        method(offset, count, arguments);

        dma_control->get += 4 * (cmd.method_count + 1);
    }
}

void RSX::method(u32 offset, u32 count, const be_t<u32>* args)
{
    switch (offset) {
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
        m_renderer->Enable(offset, args[0]);
        break;

    case NV4097_SET_ALPHA_FUNC:
    case NV4097_SET_ALPHA_REF: {
        const u32 func = regs[NV4097_SET_ALPHA_FUNC];
        const f32 ref = (f32&)regs[NV4097_SET_ALPHA_REF];
        m_renderer->AlphaFunc(func, ref);
        break;
    }

    case NV4097_SET_TRANSFORM_PROGRAM:
        for (u32 i = 0; i < count; i++) {
            pgraph.vp_data[pgraph.vp_load++] = args[i];
        }
        pgraph.vp_dirty = true;
        break;

    case NV4097_SET_TRANSFORM_PROGRAM_LOAD:
        pgraph.vp_load = args[0];
        break;

    case NV4097_SET_TRANSFORM_PROGRAM_START:
        pgraph.vp_start = args[0];
        break;

    case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT:
        break;

    case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET:
        break;

    case NV4097_SET_BEGIN_END:
        if (args[0]) {
            m_renderer->Begin(args[0]);
        } else {
            m_renderer->End();
        }
        break;

    case NV4097_DRAW_ARRAYS: {
        const u32 first = args[0] & 0xFFFFFF;
        const u32 count = (args[0] >> 24) + 1;
        m_renderer->DrawArrays(0, first, count);
        break;
    }

    case SCE_DRIVER_FLIP:
        m_renderer->Flip();
        driver_info->flip |= 0x80000000;
    }
}
