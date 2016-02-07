/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "widget_image.h"
#include "nucleus/logger/logger.h"
#include "nucleus/core/resource.h"
#include "nucleus/graphics/frontend/shader_parser.h"
#include "nucleus/ui/ui.h"

#define STB_IMAGE_IMPLEMENTATION
#include "externals/stb/stb_image.h"

namespace ui {

WidgetImage::~WidgetImage() {
    stbi_image_free(imBuffer);
}

gfx::Pipeline* WidgetImage::createPipeline(gfx::IBackend& backend) {
    gfx::ShaderDesc vertDesc = {};
    gfx::ShaderDesc fragDesc = {};
    core::Resource resVS(core::RES_SHADER_UI_WIDGET_IMAGE_VS);
    core::Resource resPS(core::RES_SHADER_UI_WIDGET_IMAGE_PS);
    vertDesc.type = gfx::SHADER_TYPE_VERTEX;
    vertDesc.module = gfx::frontend::ShaderParser::parse(reinterpret_cast<const char*>(resVS.data), resVS.size);
    fragDesc.type = gfx::SHADER_TYPE_PIXEL;
    fragDesc.module = gfx::frontend::ShaderParser::parse(reinterpret_cast<const char*>(resPS.data), resPS.size);

    gfx::PipelineDesc pipelineDesc = {};
    pipelineDesc.vs = backend.createShader(vertDesc);
    pipelineDesc.ps = backend.createShader(fragDesc);
    pipelineDesc.iaState.topology = gfx::TOPOLOGY_TRIANGLE_STRIP;
    pipelineDesc.iaState.inputLayout = {
        { 0, gfx::FORMAT_R32G32_FLOAT,  0,  0, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 }, // Position
        { 1, gfx::FORMAT_R32G32_FLOAT,  0,  8, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 }, // TexCoord
        { 2, gfx::FORMAT_R32_FLOAT,     0, 16, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 }, // Z-Index
        { 3, gfx::FORMAT_R32_FLOAT,     0, 20, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 }, // Opacity
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

void WidgetImage::update(core::ResourceName resName) {
    // Open resource
    core::Resource res(resName);
    const Byte* data = reinterpret_cast<Byte*>(res.data);
    update(data, res.size);
}

void WidgetImage::update(const std::string& filepath) {
    // Open file
    std::FILE* file;
    fopen_s(&file, filepath.c_str(), "rb");
    if (!file) {
        logger.error(LOG_UI, "Image not found: %s", filepath.c_str());
        return;
    }

    // Read file
    fseek(file, 0, SEEK_END);
    Size size = ftell(file);
    std::vector<Byte> buffer(size + 1);
    fseek(file, 0, SEEK_SET);
    fread(buffer.data(), size, 1, file);
    fclose(file);

    update(buffer.data(), size);
}

void WidgetImage::update(const Byte* buffer, Size size) {
    imBuffer = stbi_load_from_memory(buffer, size, &imWidth, &imHeight, &imComponents, 4);

    // Create texture
    gfx::TextureDesc textureDesc = {};
    textureDesc.width = imWidth;
    textureDesc.height = imHeight;
    textureDesc.mipmapLevels = 1;
    textureDesc.format = gfx::FORMAT_R8G8B8A8_UNORM;
    textureDesc.data = imBuffer;
    textureDesc.size = imWidth * imHeight * imComponents;
    auto* texture = manager->graphics->createTexture(textureDesc);

    update(texture);
}

void WidgetImage::update(gfx::Texture* newTexture) {
    texture = newTexture;
}

void WidgetImage::dimensionalize() {
    auto compWidth = getCoordX(Length{ double(imWidth), Length::TYPE_PX });
    auto compHeight = getCoordY(Length{ double(imHeight), Length::TYPE_PX });

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

void WidgetImage::render() {
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
    V0.opacity = style.opacity;
    V1.opacity = style.opacity;
    V2.opacity = style.opacity;
    V3.opacity = style.opacity;

    // Texture coordinates assume top-left is (0,0) and bottom-right is (1,1)
    V0.texcoord[0] = 0.0; V0.texcoord[1] = 1.0;
    V1.texcoord[0] = 0.0; V1.texcoord[1] = 0.0;
    V2.texcoord[0] = 1.0; V2.texcoord[1] = 1.0;
    V3.texcoord[0] = 1.0; V3.texcoord[1] = 0.0;

    if (texture) {
        // Render texture if possible
        manager->pushWidgetImage(input, texture);
    } else {
        // Otherwise placeholder container
        WidgetContainerInput containerInput;
        auto& cV0 = containerInput.vertex[0];
        auto& cV1 = containerInput.vertex[1];
        auto& cV2 = containerInput.vertex[2];
        auto& cV3 = containerInput.vertex[3];

        cV0.position[0] = cV1.position[0] = x1;
        cV2.position[0] = cV3.position[0] = x2;
        cV0.position[1] = cV2.position[1] = y1;
        cV1.position[1] = cV3.position[1] = y2;
        cV0.position[2] = cV1.position[2] = cV2.position[2] = cV3.position[2] = 0.0;
        cV0.position[3] = cV1.position[3] = cV2.position[3] = cV3.position[3] = 1.0;
        cV0.background = Color{0,0,0,1};
        cV1.background = Color{0,0,0,1};
        cV2.background = Color{0,0,0,1};
        cV3.background = Color{0,0,0,1};

        manager->pushWidgetContainer(containerInput);
    }
}

}  // namespace ui
