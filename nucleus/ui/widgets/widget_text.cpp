/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "widget_text.h"
#include "nucleus/core/resource.h"
#include "nucleus/graphics/frontend/shader_parser.h"
#include "nucleus/ui/ui.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "externals/stb/stb_truetype.h"

namespace ui {

gfx::Pipeline* WidgetText::createPipeline(gfx::GraphicsBackend& backend) {
    gfx::ShaderDesc vertDesc = {};
    gfx::ShaderDesc fragDesc = {};
    core::Resource resVS(core::RES_SHADER_UI_WIDGET_TEXT_VS);
    core::Resource resPS(core::RES_SHADER_UI_WIDGET_TEXT_PS);
    vertDesc.type = gfx::SHADER_TYPE_VERTEX;
    vertDesc.module = gfx::frontend::ShaderParser::parse(reinterpret_cast<const char*>(resVS.data), resVS.size);
    fragDesc.type = gfx::SHADER_TYPE_PIXEL;
    fragDesc.module = gfx::frontend::ShaderParser::parse(reinterpret_cast<const char*>(resPS.data), resPS.size);

    gfx::PipelineDesc pipelineDesc = {};
    pipelineDesc.numCBVs = 0;
    pipelineDesc.numSRVs = 1;
    pipelineDesc.vs = backend.createShader(vertDesc);
    pipelineDesc.ps = backend.createShader(fragDesc);
    pipelineDesc.iaState.topology = gfx::TOPOLOGY_TRIANGLE_STRIP;
    pipelineDesc.iaState.inputLayout = {
        { 0, gfx::FORMAT_R32G32_FLOAT,       0,  0, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 }, // Position
        { 1, gfx::FORMAT_R32G32_FLOAT,       0,  8, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 }, // TexCoord
        { 2, gfx::FORMAT_R32G32B32A32_FLOAT, 0, 16, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 }, // Color
        { 3, gfx::FORMAT_R32_FLOAT,          0, 32, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 }, // Z-Index
        { 4, gfx::FORMAT_R32_FLOAT,          0, 36, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 }, // Opacity
    };
    pipelineDesc.cbState.colorTarget[0] = {
        true, false,
        gfx::BLEND_SRC_ALPHA, gfx::BLEND_INV_SRC_ALPHA, gfx::BLEND_OP_ADD,
        gfx::BLEND_SRC_ALPHA, gfx::BLEND_INV_SRC_ALPHA, gfx::BLEND_OP_ADD,
        gfx::LOGIC_OP_NOOP,
        gfx::COLOR_WRITE_ENABLE_ALL
    };
    pipelineDesc.samplers.resize(1);
    pipelineDesc.samplers[0] = {
        gfx::FILTER_MIN_MAG_MIP_LINEAR,
        gfx::TEXTURE_ADDRESS_MIRROR,
        gfx::TEXTURE_ADDRESS_MIRROR,
        gfx::TEXTURE_ADDRESS_MIRROR,
    };
    return backend.createPipeline(pipelineDesc);
}

void WidgetText::update(const Font* fontFamily, const Length& fontSize, const std::string& text) {
    const auto* fontInfo = &fontFamily->fontinfo;
    auto lineHeight = getCoordY(fontSize) * manager->surface.getHeight();

    // Font properties
    int ascent, descent, lineGap;
    float scale = stbtt_ScaleForPixelHeight(fontInfo, lineHeight);
    stbtt_GetFontVMetrics(fontInfo, &ascent, &descent, &lineGap);
    ascent *= scale;
    descent *= scale;

    // Compute texture size
    txWidth = 0;
    txHeight = ascent - descent;
    for (Size i = 0; i < text.length(); i++) {
        int dx;
        int dy = (ascent - descent) + lineGap;
        int kern = stbtt_GetCodepointKernAdvance(fontInfo, text[i], text[i+1]);
        stbtt_GetCodepointHMetrics(fontInfo, text[i], &dx, 0);
        txWidth += (dx + kern) * scale;
    }

    // Render characters leaving a 1-pixel margin
    Size x = 1;
    Size y = 1;
    txWidth += 2;
    txHeight += 2;
    txBuffer.clear();
    txBuffer.resize(txWidth * txHeight);
    for (Size i = 0; i < text.length(); i++) {
        int lsb;
        int dx, dy = (ascent - descent) + lineGap;
        int kern = stbtt_GetCodepointKernAdvance(fontInfo, text[i], text[i+1]);
        stbtt_GetCodepointHMetrics(fontInfo, text[i], &dx, &lsb);

        int x1, y1, x2, y2;
        stbtt_GetCodepointBitmapBox(fontInfo, text[i], scale, scale, &x1, &y1, &x2, &y2);

        Size offset = x + (lsb * scale) + ((y + ascent + y1) * txWidth);
        stbtt_MakeCodepointBitmap(fontInfo, &txBuffer[offset], x2 - x1, y2 - y1, txWidth, scale, scale, text[i]);
        x += (dx + kern) * scale;
    }

    // Create texture
    gfx::TextureDesc textureDesc = {};
    textureDesc.width = txWidth;
    textureDesc.height = txHeight;
    textureDesc.mipmapLevels = 1;
    textureDesc.format = gfx::FORMAT_R8_UNORM;
    textureDesc.data = txBuffer.data();
    textureDesc.size = txBuffer.size();
    texture = manager->graphics->createTexture(textureDesc);

    // Create heap
    gfx::HeapDesc heapDesc = {};
    heapDesc.type = gfx::HEAP_TYPE_RESOURCE;
    heapDesc.size = 1;
    heap = manager->graphics->createHeap(heapDesc);
    heap->reset();
    heap->pushTexture(texture);
}

void WidgetText::dimensionalize() {
    auto compWidth = getCoordX(Length{ double(txWidth), Length::TYPE_PX });
    auto compHeight = getCoordY(Length{ double(txHeight), Length::TYPE_PX });

    if (style.width.type != Length::TYPE_UNDEFINED) {
        vertWidth = getCoordX(style.width);
    } else if (style.height.type != Length::TYPE_UNDEFINED) {
        vertWidth = compWidth * getCoordY(style.height) / compHeight;
    } else {
        vertWidth = compWidth;
    }

    if (style.height.type != Length::TYPE_UNDEFINED) {
        vertHeight = getCoordY(style.height);
    } else if (style.width.type != Length::TYPE_UNDEFINED) {
        vertHeight = compHeight * getCoordX(style.width) / compWidth;
    } else {
        vertHeight = compHeight;
    }
}

void WidgetText::render() {
    auto offsetTop = getOffsetTop() + getCoordY(style.margin.top);
    auto offsetLeft = getOffsetLeft() + getCoordX(style.margin.left);

    // Preparing vertex input
    auto x1 = -1.0 + 2 * (offsetLeft);
    auto x2 = -1.0 + 2 * (offsetLeft + getPaddingWidth());
    auto y1 = +1.0 - 2 * (offsetTop + getPaddingHeight());
    auto y2 = +1.0 - 2 * (offsetTop);

    auto& V0 = input.vertex[0];
    auto& V1 = input.vertex[1];
    auto& V2 = input.vertex[2];
    auto& V3 = input.vertex[3];

    V0.position[0] = V1.position[0] = x1;
    V2.position[0] = V3.position[0] = x2;
    V0.position[1] = V2.position[1] = y1;
    V1.position[1] = V3.position[1] = y2;
    V0.position[2] = V1.position[2] = V2.position[2] = V3.position[2] = 0.0;
    V0.position[3] = V1.position[3] = V2.position[3] = V3.position[3] = 1.0;
    V0.color = style.color;
    V1.color = style.color;
    V2.color = style.color;
    V3.color = style.color;
    V0.opacity = style.opacity;
    V1.opacity = style.opacity;
    V2.opacity = style.opacity;
    V3.opacity = style.opacity;

    // Texture coordinates assume top-left is (0,0) and bottom-right is (1,1)
    V0.texcoord[0] = 0.0; V0.texcoord[1] = 1.0;
    V1.texcoord[0] = 0.0; V1.texcoord[1] = 0.0;
    V2.texcoord[0] = 1.0; V2.texcoord[1] = 1.0;
    V3.texcoord[0] = 1.0; V3.texcoord[1] = 0.0;

    manager->pushWidgetText(input, texture, heap);
}

}  // namespace ui
