/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "rsx.h"
#include "nucleus/assert.h"
#include "nucleus/emulator.h"
#include "nucleus/logger/logger.h"
#include "nucleus/memory/guest_virtual/guest_virtual_memory.h"
#include "nucleus/core/config.h"
#include "nucleus/system/scei/cellos/lv1/lv1_gpu.h"

#include "nucleus/gpu/rsx/rsx_dma.h"
#include "nucleus/gpu/rsx/rsx_enum.h"
#include "nucleus/gpu/rsx/rsx_methods.h"
#include "nucleus/gpu/rsx/rsx_vp.h"

#ifdef NUCLEUS_TARGET_WINDOWS
#include <Windows.h>
#endif

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
#define case_32(offset, step) \
    case_16(offset, step) \
    case_16(offset + 16*step, step)
#define case_range(n, baseOffset, step) \
    case_##n(baseOffset, step) \
    index = (offset - baseOffset) / step;

namespace gpu {
namespace rsx {

RSX::RSX(Emulator* emulator, mem::Memory* mem, std::shared_ptr<gfx::GraphicsBackend> graphics) : GPU(emulator),
    memory(dynamic_cast<mem::GuestVirtualMemory*>(mem)),
    pgraph(std::move(graphics), this, dynamic_cast<mem::GuestVirtualMemory*>(mem))
{
    // HACK: We store the data in memory (the PS3 stores the data in the GPU and maps it later through a LV2 syscall)
    memory->getSegment(mem::SEG_RSX_MAP_MEMORY).allocFixed(0x40000000, 0x1000);
    memory->getSegment(mem::SEG_RSX_MAP_MEMORY).allocFixed(0x40100000, 0x1000);
    memory->getSegment(mem::SEG_RSX_MAP_MEMORY).allocFixed(0x40200000, 0x4000);
    memory->getSegment(mem::SEG_RSX_MAP_MEMORY).allocFixed(0x40300000, 0x10000);

    // Device
    device = memory->ptr<rsx_device_t>(0x40000000);

    // Context
    dma_control = memory->ptr<rsx_dma_control_t>(0x40100000);
    driver_info = memory->ptr<rsx_driver_info_t>(0x40200000);
    reports = memory->ptr<rsx_reports_t>(0x40300000);

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
    while (true) {
        // Wait until GET and PUT are different
        while (dma_control->get == dma_control->put) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        const U32 get = dma_control->get;
        const U32 put = dma_control->put;

        const Method cmd = { io_read32(get) };

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

        for (U32 i = 0; i < cmd.method_count; i++) {
            const U32 offset = (cmd.method_register << 2) + (cmd.flag_ni ? 0 : 4*i);
            const U32 parameter = io_read32(get + 4*(i+1));

#ifdef NUCLEUS_BUILD_DEBUG
            // Ensure method register correctness by verifying objects to be bound to subchannels
            if (cmd.method_offset == 0) {
                assert_true(cmd.method_count == 1);
                switch (cmd.method_subchannel) {
                case 0: assert_true(parameter == 0x31337000); break;
                case 1: assert_true(parameter == 0x31337303); break;
                case 3: assert_true(parameter == 0x313371C3); break;
                case 4: assert_true(parameter == 0x31337A73); break;
                case 5: assert_true(parameter == 0x31337808); break;
                case 6: assert_true(parameter == 0x3137AF00); break;
                case 7: assert_true(parameter == 0xCAFEBABE); break;
                default:
                    assert_always("Unexpected subchannel");
                }
                continue;
            }
#endif
            //logger.notice(LOG_GPU, "METHOD: 0x%X;  IO: 0x%X;  PARAM: 0x%X", offset, get, parameter);
            method(offset, parameter);
        }

        dma_control->get += 4 * (cmd.method_count + 1);
    }
}

void RSX::method(U32 offset, U32 parameter) {
    // Slot used on multiple-register methods
    U32 index;

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
        while (dma_read32(memory, dma_semaphore, dma_semaphore_offset) != parameter) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            break; // HACK: All processes get stuck here, probably due to the lack of context switching.
        }
        break;

    case NV406E_SEMAPHORE_RELEASE:
        dma_write32(memory, dma_semaphore, dma_semaphore_offset, parameter);
        break;

    // NV40_CURIE_PRIMITIVE
    case NV4097_SET_DITHER_ENABLE:
    case NV4097_SET_ALPHA_TEST_ENABLE:
    case NV4097_SET_POLY_OFFSET_FILL_ENABLE:
    case NV4097_SET_POLY_OFFSET_LINE_ENABLE:
    case NV4097_SET_POLY_OFFSET_POINT_ENABLE:
    case NV4097_SET_SPECULAR_ENABLE:
    case NV4097_SET_LINE_SMOOTH_ENABLE:
    case NV4097_SET_POLY_SMOOTH_ENABLE:
        pgraph.Enable(offset, parameter);
        break;

    case NV4097_SET_CULL_FACE_ENABLE:
        pgraph.pipeline.cull_face_enable = parameter;
        break;

    case NV4097_SET_CULL_FACE:
        pgraph.pipeline.cull_mode = static_cast<CullMode>(parameter);
        break;

    case NV4097_SET_FRONT_FACE:
        pgraph.pipeline.front_face = static_cast<FrontFace>(parameter);
        break;

    case NV4097_SET_SEMAPHORE_OFFSET:
        pgraph.semaphore_index = parameter >> 4;
        break;

    case NV4097_TEXTURE_READ_SEMAPHORE_RELEASE:
        reports->semaphore[pgraph.semaphore_index].value = parameter;
        reports->semaphore[pgraph.semaphore_index].padding = 0;
        reports->semaphore[pgraph.semaphore_index].timestamp = ptimer_gettime();
        break;

    case NV4097_BACK_END_WRITE_SEMAPHORE_RELEASE: {
        const U32 value = (parameter & 0xFF00FF00) | ((parameter & 0xFF) << 16) | ((parameter >> 16) & 0xFF);
        reports->semaphore[pgraph.semaphore_index].value = value;
        reports->semaphore[pgraph.semaphore_index].padding = 0;
        reports->semaphore[pgraph.semaphore_index].timestamp = ptimer_gettime();
        break;
    }

    // Clearing
    case NV4097_SET_COLOR_CLEAR_VALUE:
        pgraph.clear_color = parameter;
        break;
    case NV4097_SET_ZSTENCIL_CLEAR_VALUE:
        pgraph.clear_depth = parameter >> 8;
        pgraph.clear_stencil = parameter & 0xFF;
        break;
    case NV4097_CLEAR_SURFACE:
        pgraph.ClearSurface(parameter);
        break;

    // Pipeline
    case NV4097_SET_ALPHA_FUNC:
        pgraph.pipeline.alpha_func = parameter;
        break;

    case NV4097_SET_ALPHA_REF:
        pgraph.pipeline.alpha_ref = reinterpret_cast<F32&>(parameter);
        break;

    case NV4097_SET_BLEND_ENABLE:
        pgraph.pipeline.blend_enable = parameter;
        break;

    case NV4097_SET_BLEND_EQUATION:
        pgraph.pipeline.blend_equation_rgb = static_cast<BlendEquation>(parameter & 0xFFFF);
        pgraph.pipeline.blend_equation_alpha = static_cast<BlendEquation>(parameter >> 16);
        break;

    case NV4097_SET_BLEND_FUNC_SFACTOR:
        pgraph.pipeline.blend_sfactor_rgb = static_cast<Blend>(parameter & 0xFFFF);
        pgraph.pipeline.blend_sfactor_alpha = static_cast<Blend>(parameter >> 16);
        break;

    case NV4097_SET_BLEND_FUNC_DFACTOR:
        pgraph.pipeline.blend_dfactor_rgb = static_cast<Blend>(parameter & 0xFFFF);
        pgraph.pipeline.blend_dfactor_alpha = static_cast<Blend>(parameter >> 16);
        break;

    case NV4097_SET_LOGIC_OP_ENABLE:
        pgraph.pipeline.logic_op_enable = parameter;
        break;

    case NV4097_SET_LOGIC_OP:
        pgraph.pipeline.logic_op = static_cast<LogicOp>(parameter);
        break;

    case NV4097_SET_COLOR_MASK:
        pgraph.pipeline.color_mask = static_cast<ColorMask>(parameter);
        break;

    // Depth
    case NV4097_SET_DEPTH_TEST_ENABLE:
        pgraph.pipeline.depth_test_enable = parameter;
        break;
    case NV4097_SET_DEPTH_MASK:
        pgraph.pipeline.depth_mask = parameter;
        break;
    case NV4097_SET_DEPTH_FUNC:
        pgraph.pipeline.depth_func = static_cast<CompareFunc>(parameter);
        break;

    // Stencil
    case NV4097_SET_STENCIL_TEST_ENABLE:
        pgraph.pipeline.stencil_test_enable = parameter;
        break;
    case NV4097_SET_TWO_SIDED_STENCIL_TEST_ENABLE:
        pgraph.pipeline.two_sided_stencil_test_enable = parameter;
        break;
    case NV4097_SET_STENCIL_FUNC_MASK:
        pgraph.pipeline.stencil_func_mask = parameter;
        break;
    case NV4097_SET_STENCIL_MASK:
        pgraph.pipeline.stencil_mask = parameter;
        break;
    case NV4097_SET_STENCIL_OP_ZPASS:
        pgraph.pipeline.stencil_op_zpass = static_cast<StencilOp>(parameter);
        break;
    case NV4097_SET_STENCIL_OP_ZFAIL:
        pgraph.pipeline.stencil_op_zfail = static_cast<StencilOp>(parameter);
        break;
    case NV4097_SET_STENCIL_OP_FAIL:
        pgraph.pipeline.stencil_op_fail = static_cast<StencilOp>(parameter);
        break;
    case NV4097_SET_STENCIL_FUNC:
        pgraph.pipeline.stencil_func = static_cast<CompareFunc>(parameter);
        break;
    case NV4097_SET_BACK_STENCIL_OP_ZPASS:
        pgraph.pipeline.back_stencil_op_zpass = static_cast<StencilOp>(parameter);
        break;
    case NV4097_SET_BACK_STENCIL_OP_ZFAIL:
        pgraph.pipeline.back_stencil_op_zfail = static_cast<StencilOp>(parameter);
        break;
    case NV4097_SET_BACK_STENCIL_OP_FAIL:
        pgraph.pipeline.back_stencil_op_fail = static_cast<StencilOp>(parameter);
        break;
    case NV4097_SET_BACK_STENCIL_FUNC:
        pgraph.pipeline.back_stencil_func = static_cast<CompareFunc>(parameter);
        break;

    case NV4097_SET_CONTEXT_DMA_REPORT:
        pgraph.dma_report = parameter;
        break;

    case NV4097_SET_SURFACE_FORMAT:
        pgraph.surface.dirty = true;
        pgraph.surface.depthFormat = static_cast<Surface::DepthStencilFormat>((parameter >> 5) & 0b111);
        pgraph.surface.colorFormat = static_cast<Surface::ColorFormat>((parameter >> 0) & 0b11111);
        break;

    case NV4097_SET_SURFACE_COLOR_AOFFSET:
        pgraph.surface.dirty = true;
        pgraph.surface.colorOffset[0] = parameter;
        break;

    case NV4097_SET_SURFACE_COLOR_BOFFSET:
        pgraph.surface.dirty = true;
        pgraph.surface.colorOffset[1] = parameter;
        break;

    case NV4097_SET_SURFACE_COLOR_COFFSET:
        pgraph.surface.dirty = true;
        pgraph.surface.colorOffset[2] = parameter;
        break;

    case NV4097_SET_SURFACE_COLOR_DOFFSET:
        pgraph.surface.dirty = true;
        pgraph.surface.colorOffset[3] = parameter;
        break;

    case NV4097_SET_SURFACE_ZETA_OFFSET:
        pgraph.surface.dirty = true;
        pgraph.surface.depthOffset = parameter;
        break;

    case NV4097_SET_SURFACE_CLIP_HORIZONTAL:
        pgraph.surface.dirty = true;
        pgraph.surface.x = parameter & 0xFFFF;
        pgraph.surface.width = (parameter >> 16) & 0xFFFF;
        pgraph.vertex_transform_dirty = true;
        break;

    case NV4097_SET_SURFACE_CLIP_VERTICAL:
        pgraph.surface.dirty = true;
        pgraph.surface.y = parameter & 0xFFFF;
        pgraph.surface.height = (parameter >> 16) & 0xFFFF;
        pgraph.vertex_transform_dirty = true;
        break;

    case NV4097_SET_SURFACE_COLOR_TARGET:
        pgraph.surface.dirty = true;
        pgraph.surface.colorTarget = parameter;
        break;

    // Viewport
    case NV4097_SET_VIEWPORT_HORIZONTAL:
        pgraph.viewport.x = parameter & 0xFFFF;
        pgraph.viewport.width = (parameter >> 16) & 0xFFFF;
        pgraph.viewport.dirty = true;
        break;
    case NV4097_SET_VIEWPORT_VERTICAL:
        pgraph.viewport.y = parameter & 0xFFFF;
        pgraph.viewport.height = (parameter >> 16) & 0xFFFF;
        pgraph.viewport.dirty = true;
        break;
    case_range(4, NV4097_SET_VIEWPORT_SCALE, 4)
        pgraph.viewport_scale.u32[index] = parameter;
        pgraph.vertex_transform_dirty = true;
        break;
    case_range(4, NV4097_SET_VIEWPORT_OFFSET, 4)
        pgraph.viewport_offset.u32[index] = parameter;
        pgraph.vertex_transform_dirty = true;
        break;

    // Scissor
    case NV4097_SET_SCISSOR_HORIZONTAL:
        pgraph.scissor.x = parameter & 0xFFFF;
        pgraph.scissor.width = (parameter >> 16) & 0xFFFF;
        pgraph.scissor.dirty = true;
        break;
    case NV4097_SET_SCISSOR_VERTICAL:
        pgraph.scissor.y = parameter & 0xFFFF;
        pgraph.scissor.height = (parameter >> 16) & 0xFFFF;
        pgraph.scissor.dirty = true;
        break;

    case_range(32, NV4097_SET_TRANSFORM_PROGRAM, 4)
        pgraph.vpe.data[pgraph.vpe.load].word[index % 4] = parameter;
        if (index % 4 == 3) {
            pgraph.vpe.load += 1;
            pgraph.vpe.dirty_instructions = true;
        }
        break;

    case NV4097_SET_TRANSFORM_PROGRAM_LOAD:
        pgraph.vpe.load = parameter;
        break;

    case NV4097_SET_TRANSFORM_PROGRAM_START:
        pgraph.vpe.start = parameter;
        break;

    case_range(32, NV4097_SET_TRANSFORM_CONSTANT, 4)
        pgraph.vpe.constant[pgraph.vpe.constant_load].u32[index % 4] = parameter;
        if (index % 4 == 3) {
            pgraph.vpe.constant_load += 1;
            pgraph.vpe.dirty_constant = true;
        }
        break;

    case NV4097_SET_TRANSFORM_CONSTANT_LOAD:
        pgraph.vpe.constant_load = parameter;
        break;

    case NV4097_SET_SHADER_PROGRAM:
        pgraph.fp_location = (parameter & 0x3) - 1;
        pgraph.fp_offset = (parameter & ~0x3);
        break;

    case NV4097_SET_SHADER_CONTROL:
        pgraph.fp_control = parameter;
        break;

    case_range(16, NV4097_SET_VERTEX_DATA_ARRAY_FORMAT, 4)
        pgraph.vpe.attr[index].type = static_cast<VertexType>(parameter & 0xF);
        pgraph.vpe.attr[index].size = (parameter >> 4) & 0xF;
        pgraph.vpe.attr[index].stride = (parameter >> 8) & 0xFF;
        pgraph.vpe.attr[index].frequency = (parameter >> 16);
        pgraph.vpe.attr[index].dirty = true;
        break;

    case_range(16, NV4097_SET_VERTEX_DATA_ARRAY_OFFSET, 4)
        pgraph.vpe.attr[index].offset = parameter & 0x7FFFFFFF;
        pgraph.vpe.attr[index].location = (parameter >> 31);
        pgraph.vpe.attr[index].dirty = true;
        break;

    case NV4097_SET_VERTEX_DATA_BASE_OFFSET:
        pgraph.vertex_data_base_offset = parameter;
        break;

    case NV4097_SET_VERTEX_DATA_BASE_INDEX:
        pgraph.vertex_data_base_index = parameter;
        break;

    case_range(16, NV4097_SET_TEXTURE_OFFSET, 0x20)
        pgraph.texture[index].offset = parameter;
        break;

    case_range(16, NV4097_SET_TEXTURE_FORMAT, 0x20)
        pgraph.texture[index].location = (parameter & 0x3) - 1;
        pgraph.texture[index].cubemap = (parameter >> 2) & 0x1;
        pgraph.texture[index].border = (parameter >> 3) & 0x1;
        pgraph.texture[index].dimension = (parameter >> 4) & 0xF;
        pgraph.texture[index].format = (parameter >> 8) & 0xFF;
        pgraph.texture[index].mipmap = (parameter >> 16);
        break;

    case_range(16, NV4097_SET_TEXTURE_ADDRESS, 0x20)
        pgraph.texture[index].wrap_s = parameter & 0xF;
        pgraph.texture[index].anisoBias = (parameter >> 4) & 0xF;
        pgraph.texture[index].wrap_t = (parameter >> 8) & 0xF;
        pgraph.texture[index].unsignedRemap = (parameter >> 12) & 0xF;
        pgraph.texture[index].wrap_r = (parameter >> 16) & 0xF;
        pgraph.texture[index].gamma = (parameter >> 20) & 0xF;
        pgraph.texture[index].signedRemap = (parameter >> 24) & 0xF;
        pgraph.texture[index].zfunc = (parameter >> 28) & 0xF;
        break;

    case_range(16, NV4097_SET_TEXTURE_CONTROL0, 0x20)
        pgraph.texture[index].alphakill = (parameter >> 2) & 0x3;
        pgraph.texture[index].max_aniso = (parameter >> 4) & 0x7;
        pgraph.texture[index].max_lod = (parameter >> 7) & 0xFFF;
        pgraph.texture[index].min_lod = (parameter >> 19) & 0xFFF;
        pgraph.texture[index].enable = (parameter >> 31);
        break;

    case_range(16, NV4097_SET_TEXTURE_CONTROL1, 0x20)
        pgraph.texture[index].remap = parameter;
        break;

    case_range(16, NV4097_SET_TEXTURE_FILTER, 0x20)
        pgraph.texture[index].filter.value = parameter;
        break;

    case_range(16, NV4097_SET_TEXTURE_IMAGE_RECT, 0x20)
        pgraph.texture[index].width = parameter >> 16;
        pgraph.texture[index].height = parameter & 0xFFFF;
        break;

    case_range(16, NV4097_SET_TEXTURE_BORDER_COLOR, 0x20)
        pgraph.texture[index].border_color = parameter;
        break;

    case_range(16, NV4097_SET_TEXTURE_CONTROL3, 4)
        pgraph.texture[index].pitch = parameter & 0xFFFFF;
        pgraph.texture[index].depth = parameter >> 20;
        break;

    case NV4097_SET_BEGIN_END:
        if (parameter) {
            pgraph.Begin(static_cast<Primitive>(parameter));
        } else {
            pgraph.End();
        }
        break;

    case NV4097_DRAW_ARRAYS: {
        const U32 first = parameter & 0xFFFFFF;
        const U32 count = (parameter >> 24) + 1;
        pgraph.LoadVertexAttributes(first, count);
        pgraph.DrawArrays(first, count);
        break;
    }

    case NV4097_GET_REPORT: {
        const U32 type = parameter >> 24;
        const U32 offset = parameter & 0xFFFFFF;

        // TODO: Get the value for the requested report type
        U64 timestamp = ptimer_gettime();
        U32 value = 0;

        dma_write64(memory, pgraph.dma_report, offset + 0x0, timestamp);
        dma_write32(memory, pgraph.dma_report, offset + 0x8, value);
        dma_write32(memory, pgraph.dma_report, offset + 0xC, 0);
        break;
    }

    // SCE DRIVER
    case_range(2, SCE_DRIVER_FLIP, 4)
        pgraph.Flip();
        sys::lv1_gpu_context_attribute(*(sys::LV2*)(getEmulator()->sys.get()), 0x55555555, 0x102, index, parameter, 0, 0);
        break;

    case_range(2, SCE_DRIVER_QUEUE, 4)
        //glFinish();
        queued_display = parameter;
        sys::lv1_gpu_context_attribute(*(sys::LV2*)(getEmulator()->sys.get()), 0x55555555, 0x103, index, parameter, 0, 0);
        break;

    default:
        ;//logger.notice(LOG_GPU, "METHOD: 0x%X; PARAM: 0x%X", offset, parameter);
    }
}

U64 RSX::ptimer_gettime() {
#ifdef NUCLEUS_TARGET_WINDOWS
    static struct PerformanceFreqHolder {
        U64 value;
        PerformanceFreqHolder() {
            LARGE_INTEGER freq;
            QueryPerformanceFrequency(&freq);
            value = freq.QuadPart;
        }
    } freq;

    LARGE_INTEGER cycle;
    QueryPerformanceCounter(&cycle);
    const U64 sec = cycle.QuadPart / freq.value;
    return sec * 1000000000 + (cycle.QuadPart % freq.value) * 1000000000 / freq.value;
#else
    logger.error(LOG_CPU, "Could not get the PTIMER value");
    return 0;
#endif
}

U32 RSX::io_read32(U32 offset) {
    for (const auto& map : iomaps) {
        if (map.io <= offset && (offset & ~0x3) < map.io + map.size) {
            return memory->read32(map.ea + (offset - map.io));
        }
    }
    logger.error(LOG_GPU, "Illegal IO 32-bit read");
    return 0;
}

void RSX::io_write32(U32 offset, U32 value) {
    for (const auto& map : iomaps) {
        if (map.io <= offset && (offset & ~0x3) <= map.io + map.size) {
            memory->write32(map.ea + (offset - map.io), value);
        }
    }
    logger.error(LOG_GPU, "Illegal IO 32-bit write");
}

U32 RSX::get_ea(U32 offset) {
    for (const auto& map : iomaps) {
        if (map.io <= offset && offset < map.io + map.size) {
            return map.ea + (offset - map.io);
        }
    }
    logger.warning(LOG_GPU, "Queried invalid IO address");
    return 0;
}

gfx::Texture* RSX::getFrontBuffer() {
    U32 displayAddr = display[queued_display].offset;
    return pgraph.getTexture(displayAddr);
}

}  // namespace rsx
}  // namespace gpu
