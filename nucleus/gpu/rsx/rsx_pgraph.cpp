/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "rsx_pgraph.h"
#include "nucleus/assert.h"
#include "nucleus/emulator.h"
#include "nucleus/logger/logger.h"
#include "nucleus/gpu/rsx/rsx.h"
#include "nucleus/gpu/rsx/rsx_convert.h"
#include "nucleus/gpu/rsx/rsx_enum.h"
#include "nucleus/gpu/rsx/rsx_methods.h"

#include <cstring>

namespace gpu {
namespace rsx {

PGRAPH::PGRAPH(std::shared_ptr<gfx::IBackend> backend, RSX* rsx, mem::Memory* memory) :
    graphics(std::move(backend)), rsx(rsx), memory(memory), surface(), cacheTexture(256_MB) {
    cmdQueue = graphics->getGraphicsCommandQueue();
    cmdBuffer = graphics->createCommandBuffer();

    // Fence
    gfx::FenceDesc fenceDesc = {};
    fence = graphics->createFence(fenceDesc);

    // Heaps
    gfx::HeapDesc heapResourcesDesc = {};
    heapResourcesDesc.type = gfx::HEAP_TYPE_RESOURCE;
    heapResourcesDesc.size = RSX_MAX_TEXTURES + 1;
    heapResources = graphics->createHeap(heapResourcesDesc);
    gfx::HeapDesc heapSamplersDesc = {};
    heapSamplersDesc.type = gfx::HEAP_TYPE_RESOURCE;
    heapSamplersDesc.size = RSX_MAX_TEXTURES;
    heapSamplers = graphics->createHeap(heapSamplersDesc);

    // Vertex Buffer for VPE constant registers
    gfx::VertexBufferDesc vtxConstantBufferDesc;
    vtxConstantBufferDesc.size = 468 * sizeof(V128);
    vpeConstantMemory = graphics->createVertexBuffer(vtxConstantBufferDesc);

    // Vertex buffer for VPE input attributes
    gfx::VertexBufferDesc vtxInputBufferDesc = {};
    vtxInputBufferDesc.size = 0x100000; // TODO: Could it be bigger? Assuming 0x10000 vertices * 4 coords/vertex * 4 bytes/coord
    for (Size i = 0; i < 16; i++) {
        vpeInputs[i] = graphics->createVertexBuffer(vtxInputBufferDesc);
    }
}

PGRAPH::~PGRAPH() {
}

U64 PGRAPH::HashVertexProgram(rsx_vp_instruction_t* program) {
    // 64-bit Fowler/Noll/Vo FNV-1a hash code
    U64 hash = 0xCBF29CE484222325ULL;
    do {
        hash ^= program->dword[0];
        hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) + (hash << 8) + (hash << 40);
        hash ^= program->dword[1];
        hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) + (hash << 8) + (hash << 40);
    } while (!(program++)->end);
    return hash;
}

U64 PGRAPH::HashFragmentProgram(rsx_fp_instruction_t* program) {
    // 64-bit Fowler/Noll/Vo FNV-1a hash code
    bool end = false;
    U64 hash = 0xCBF29CE484222325ULL;
    do {
        hash ^= program->dword[0];
        hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) + (hash << 8) + (hash << 40);
        hash ^= program->dword[1];
        hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) + (hash << 8) + (hash << 40);
        end = ((program++)->word[0] >> 8) & 0x1; // NOTE: We can't acces program->end directly, since words require byte swapping
    } while (!end);
    return hash;
}

void PGRAPH::LoadVertexAttributes(U32 first, U32 count) {
    // Bytes per vertex coordinate. Index is given by attribute::type.
    static const U32 vertexTypeSize[] = {
        0, 2, 4, 2, 1, 2, 4, 1
    };

    for (Size attrIndex = 0; attrIndex < 16; attrIndex++) {
        auto& attr = vpe.attr[attrIndex];
        if (!attr.size) {
            continue;
        }

        // Get vertex buffer address
        U32 addr;
        if (attr.location == RSX_LOCATION_LOCAL) {
            addr = nucleus.memory->getSegment(mem::SEG_RSX_LOCAL_MEMORY).getBaseAddr() + attr.offset;
        } else {
            addr = rsx->get_ea(attr.offset);
        }

        const U32 typeSize = vertexTypeSize[attr.type];
        //attr.data.resize(count * attr.size * typeSize);

        // Copy data per vertex
        U08* data = (U08*)vpeInputs[attrIndex]->map();
        for (U32 i = 0; i < count; i++) {
            U32 src = addr + vertex_data_base_offset + attr.stride * (first + i + vertex_data_base_index);
            void* dst = &/*attr.*/data[i * attr.size * typeSize];

            switch (typeSize) {
            case 1:
                for (U08 j = 0; j < attr.size; j++) {
                    ((U08*)dst)[j] = nucleus.memory->read8(src + 1*j);
                }
                break;
            case 2:
                for (U08 j = 0; j < attr.size; j++) {
                    ((U16*)dst)[j] = nucleus.memory->read16(src + 2*j);
                }
                break;
            case 4:
                for (U08 j = 0; j < attr.size; j++) {
                    ((U32*)dst)[j] = nucleus.memory->read32(src + 4*j);
                }
                break;
            }
        }
        vpeInputs[attrIndex]->unmap();

        U32 offset = 0;
        U32 stride = attr.size * typeSize;
        cmdBuffer->cmdSetVertexBuffers(attrIndex, 1, &vpeInputs[attrIndex], &offset, &stride);
    }
}

gfx::Texture* PGRAPH::getTexture(U32 address) {
    if (textures.find(address) != textures.end()) {
        return textures[address];
    } else {
        return nullptr;
    }
}

gfx::ColorTarget* PGRAPH::getColorTarget(U32 address) {
    if (colorTargets.find(address) != colorTargets.end()) {
        return colorTargets[address];
    }
    // Generate a texture to hold the color buffer
    gfx::TextureDesc desc = {};
    desc.mipmapLevels = 1;
    desc.width = surface.width;
    desc.height = surface.height;
    desc.format = convertFormat(surface.colorFormat);
    desc.flags = gfx::TEXTURE_FLAG_COLOR_TARGET;
    auto* texture = graphics->createTexture(desc);
    textures[address] = texture;

    auto* target = graphics->createColorTarget(texture);
    colorTargets[address] = target;
    return target;
}

gfx::DepthStencilTarget* PGRAPH::getDepthStencilTarget(U32 address) {
    if (depthStencilTargets.find(address) != depthStencilTargets.end()) {
        return depthStencilTargets[address];
    }
    // Generate a texture to hold the depth buffer
    gfx::TextureDesc desc = {};
    desc.mipmapLevels = 1;
    desc.width = surface.width;
    desc.height = surface.height;
    desc.format = convertFormat(surface.depthFormat);
    desc.flags = gfx::TEXTURE_FLAG_DEPTHSTENCIL_TARGET;
    auto* texture = graphics->createTexture(desc);
    textures[address] = texture;

    auto* target = graphics->createDepthStencilTarget(texture);
    depthStencilTargets[address] = target;
    return target;
}

void PGRAPH::setSurface() {
    Size colorCount = 0;
    gfx::ColorTarget* colors[4];
    gfx::DepthStencilTarget* depth = getDepthStencilTarget(surface.depthOffset);

    switch (surface.colorTarget) {
    case RSX_SURFACE_TARGET_NONE:
        colorCount = 0;
        break;
    case RSX_SURFACE_TARGET_0:
        colors[0] = getColorTarget(surface.colorOffset[0]);
        colorCount = 1;
        break;
    case RSX_SURFACE_TARGET_1:
        colors[1] = getColorTarget(surface.colorOffset[1]);
        colorCount = 1;
        break;
    case RSX_SURFACE_TARGET_MRT1:
        colors[0] = getColorTarget(surface.colorOffset[0]);
        colors[1] = getColorTarget(surface.colorOffset[1]);
        colorCount = 2;
        break;
    case RSX_SURFACE_TARGET_MRT2:
        colors[0] = getColorTarget(surface.colorOffset[0]);
        colors[1] = getColorTarget(surface.colorOffset[1]);
        colors[2] = getColorTarget(surface.colorOffset[2]);
        colorCount = 3;
        break;
    case RSX_SURFACE_TARGET_MRT3:
        colors[0] = getColorTarget(surface.colorOffset[0]);
        colors[1] = getColorTarget(surface.colorOffset[1]);
        colors[2] = getColorTarget(surface.colorOffset[2]);
        colors[3] = getColorTarget(surface.colorOffset[3]);
        colorCount = 4;
        break;
    default:
        assert_always("Unexpected");
    }
    cmdBuffer->cmdSetTargets(colorCount, colors, depth);
    surface.dirty = false;
}

/**
 * PGRAPH methods
 */
// NOTE: RSX doesn't know how big the vertex buffer is, but OpenGL requires this information
// to copy the data to the host GPU. Therefore, LoadVertexAttributes needs to be called.
/*void PGRAPH::BindVertexAttributes() {
    // Indices are attr.type
    static const GLenum vertexType[] = {
        0, GL_SHORT, GL_FLOAT, GL_HALF_FLOAT, GL_UNSIGNED_BYTE, GL_SHORT, GL_FLOAT, GL_UNSIGNED_BYTE
    };
    static const GLboolean vertexNormalized[] = {
        0, GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE, GL_FALSE
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    for (int index = 0; index < 16; index++) {
        const auto& attr = vpe.attr[index];
        if (attr.size) {
            GLuint vbo;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, attr.data.size(), attr.data.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, attr.size, vertexType[attr.type], vertexNormalized[attr.type], 0, 0);
        }
    }
}*/

void PGRAPH::Begin(Primitive primitive) {
    // Set surface
    setSurface();

    // Set viewport
    viewport.width = 1280; // TODO: Why is this failing?
    scissor.width = 1280;  // TODO: Why is this failing?
    gfx::Viewport viewportRect = { viewport.x, viewport.y, viewport.width, viewport.height, 0.0f, 1.0f };
    gfx::Rectangle scissorRect = { scissor.x, scissor.y, scissor.width, scissor.height };
    cmdBuffer->cmdSetViewports(1, &viewportRect);
    cmdBuffer->cmdSetScissors(1, &scissorRect);

    // Hashing
    auto vpData = &vpe.data[vpe.start];
    auto vpHash = HashVertexProgram(vpData);
    auto fpData = memory->ptr<rsx_fp_instruction_t>((fp_location ? rsx->get_ea(0x0) : 0xC0000000) + fp_offset);
    auto fpHash = HashFragmentProgram(fpData);
    auto pipelineHash = hashStruct(pipeline) ^ vpHash ^ fpHash;

    if (cachePipeline.find(pipelineHash) == cachePipeline.end()) {
        const auto& p = pipeline;
        if (cacheVP.find(vpHash) == cacheVP.end()) {
            auto vp = std::make_unique<RSXVertexProgram>();
            vp->decompile(vpData);
            vp->compile(graphics.get());
            cacheVP[vpHash] = std::move(vp);
        }
        if (cacheFP.find(fpHash) == cacheFP.end()) {
            auto fp = std::make_unique<RSXFragmentProgram>();
            fp->decompile(fpData);
            fp->compile(graphics.get());
            cacheFP[fpHash] = std::move(fp);
        }
        gfx::PipelineDesc pipelineDesc = {};
        pipelineDesc.numCBVs = 1;
        pipelineDesc.numSRVs = 1;
        pipelineDesc.vs = cacheVP[vpHash]->shader;
        pipelineDesc.ps = cacheFP[fpHash]->shader;
        pipelineDesc.rsState.fillMode = gfx::FILL_MODE_SOLID;
        pipelineDesc.rsState.cullMode = p.cull_face_enable ? convertCullMode(p.cull_mode) : gfx::CULL_MODE_NONE;
        pipelineDesc.cbState.colorTarget[0].enableBlend = p.blend_enable;
        pipelineDesc.cbState.colorTarget[0].enableLogicOp = p.logic_op_enable;
        pipelineDesc.cbState.colorTarget[0].blendOp = convertBlendOp(p.blend_equation_rgb);
        pipelineDesc.cbState.colorTarget[0].blendOpAlpha = convertBlendOp(p.blend_equation_alpha);
        pipelineDesc.cbState.colorTarget[0].srcBlend = convertBlend(p.blend_sfactor_rgb);
        pipelineDesc.cbState.colorTarget[0].destBlend = convertBlend(p.blend_dfactor_rgb);
        pipelineDesc.cbState.colorTarget[0].srcBlendAlpha = convertBlend(p.blend_sfactor_alpha);
        pipelineDesc.cbState.colorTarget[0].destBlendAlpha = convertBlend(p.blend_dfactor_alpha);
        pipelineDesc.cbState.colorTarget[0].colorWriteMask = convertColorMask(p.color_mask);
        pipelineDesc.cbState.colorTarget[0].logicOp = convertLogicOp(p.logic_op);
        pipelineDesc.iaState.topology = convertPrimitiveTopology(primitive);
        for (U32 index = 0; index < RSX_MAX_VERTEX_INPUTS; index++) {
            const auto& attr = vpe.attr[index];
            if (!attr.size) {
                continue;
            }
            gfx::Format format = convertVertexFormat(attr.type, attr.size);
            U32 stride = attr.stride;
            pipelineDesc.iaState.inputLayout.push_back({
                index, format, index, 0, stride, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 }
            );
        }
        for (U32 i = 0; i < 1/*RSX_MAX_TEXTURES*/; i++) {
            gfx::Sampler sampler = {};
            sampler.filter = gfx::FILTER_MIN_MAG_MIP_LINEAR;
            sampler.addressU = gfx::TEXTURE_ADDRESS_MIRROR;
            sampler.addressV = gfx::TEXTURE_ADDRESS_MIRROR;
            sampler.addressW = gfx::TEXTURE_ADDRESS_MIRROR;
            pipelineDesc.samplers.push_back(sampler);
        }
        cachePipeline[pipelineHash] = std::unique_ptr<gfx::Pipeline>(graphics->createPipeline(pipelineDesc));
    }

    heapResources->reset();

    // Upload VPE constants if necessary
    void* constantsPtr = vpeConstantMemory->map();
    memcpy(constantsPtr, &vpe.constant, sizeof(vpe.constant));
    vpeConstantMemory->unmap();
    heapResources->pushVertexBuffer(vpeConstantMemory);

    // Set textures
    for (U32 i = 0; i < 1/*RSX_MAX_TEXTURES*/; i++) {
        const auto& tex = texture[i];
        if (!tex.enable) {
            heapResources->pushTexture(nullptr);
        } else {
            auto texFormat = static_cast<TextureFormat>(tex.format & ~RSX_TEXTURE_LN & ~RSX_TEXTURE_UN);

            gfx::TextureDesc texDesc = {};
            texDesc.data = memory->ptr<Byte>((tex.location ? rsx->get_ea(0x0) : 0xC0000000) + tex.offset);
            texDesc.size = tex.width * tex.height;
            texDesc.width = tex.width;
            texDesc.height = tex.height;
            texDesc.format = convertTextureFormat(texFormat);
            texDesc.mipmapLevels = tex.mipmap;
            texDesc.swizzle = convertTextureSwizzle(texFormat);

            gfx::Texture* texDescriptor = graphics->createTexture(texDesc);
            heapResources->pushTexture(texDescriptor);
        }
    }

    cmdBuffer->cmdBindPipeline(cachePipeline[pipelineHash].get());
    cmdBuffer->cmdSetHeaps({ heapResources });
    cmdBuffer->cmdSetDescriptor(0, heapResources, 0);
    cmdBuffer->cmdSetDescriptor(1, heapResources, 1);
    cmdBuffer->cmdSetPrimitiveTopology(convertPrimitiveTopology(primitive));
}

void PGRAPH::End() {
    cmdBuffer->finalize();
    cmdQueue->submit(cmdBuffer, fence);
    fence->wait();
    cmdBuffer->reset();
}

void PGRAPH::ClearSurface(U32 mask) {
    // Avoid clearing empty surfaces
    if (surface.width == 0 || surface.height == 0) {
        return;
    }
    const F32 color[4] = {
        ((clear_color >> 24) & 0xFF) / 255.0f, // Red
        ((clear_color >> 16) & 0xFF) / 255.0f, // Green
        ((clear_color >>  8) & 0xFF) / 255.0f, // Blue
        ((clear_color >>  0) & 0xFF) / 255.0f, // Alpha
    };
    const F32 depth = clear_depth / F32(0xFFFFFF);
    const U08 stencil = clear_stencil;

    if (mask & RSX_CLEAR_BIT_COLOR) {
        auto* colorTarget = getColorTarget(surface.colorOffset[0]);
        cmdBuffer->cmdClearColor(colorTarget, color);
    }
    if (mask & (RSX_CLEAR_BIT_DEPTH | RSX_CLEAR_BIT_STENCIL)) {
        // TODO: Depth-exclusive or stencil-exclusive clears are unimplemented
        auto* depthTarget = getDepthStencilTarget(surface.depthOffset);
        cmdBuffer->cmdClearDepthStencil(depthTarget, depth, stencil);
    }

    // TODO: Check if cmdBuffer is empty
    cmdBuffer->finalize();
    cmdQueue->submit(cmdBuffer, fence);
    fence->wait();
    cmdBuffer->reset();
}

void PGRAPH::DrawArrays(U32 first, U32 count) {
    cmdBuffer->cmdDraw(first, count, 0, 1);
}

void PGRAPH::Enable(U32 prop, U32 enabled) {
    /*switch (prop) {
    case NV4097_SET_DITHER_ENABLE:
        enabled ? glEnable(GL_DITHER) : glDisable(GL_DITHER);
        break;

    case NV4097_SET_ALPHA_TEST_ENABLE:
        enabled ? glEnable(GL_ALPHA_TEST) : glDisable(GL_ALPHA_TEST);
        break;

    case NV4097_SET_STENCIL_TEST_ENABLE:
        enabled ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
        break;

    case NV4097_SET_DEPTH_TEST_ENABLE:
        enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        break;

    case NV4097_SET_BLEND_ENABLE:
        enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
        break;

    case NV4097_SET_POLY_OFFSET_FILL_ENABLE:
        enabled ? glEnable(GL_POLYGON_OFFSET_FILL) : glDisable(GL_POLYGON_OFFSET_FILL);
        break;

    case NV4097_SET_POLY_OFFSET_LINE_ENABLE:
        enabled ? glEnable(GL_POLYGON_OFFSET_LINE) : glDisable(GL_POLYGON_OFFSET_LINE);
        break;

    case NV4097_SET_POLY_OFFSET_POINT_ENABLE:
        enabled ? glEnable(GL_POLYGON_OFFSET_POINT) : glDisable(GL_POLYGON_OFFSET_POINT);
        break;

    case NV4097_SET_LOGIC_OP_ENABLE:
        // TODO: Nsight dislikes this
        //enabled ? glEnable(GL_LOGIC_OP) : glDisable(GL_LOGIC_OP);
        break;

    case NV4097_SET_SPECULAR_ENABLE:
        // TODO: Nsight dislikes this
        //enabled ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
        break;

    case NV4097_SET_LINE_SMOOTH_ENABLE:
        enabled ? glEnable(GL_LINE_SMOOTH) : glDisable(GL_LINE_SMOOTH);
        break;

    case NV4097_SET_POLY_SMOOTH_ENABLE:
        enabled ? glEnable(GL_POLYGON_SMOOTH) : glDisable(GL_POLYGON_SMOOTH);
        break;
    }
    checkRendererError("Enable");*/
}

void PGRAPH::Flip() {
}

}  // namespace rsx
}  // namespace gpu
