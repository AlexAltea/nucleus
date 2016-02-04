/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "widget_image.h"
#include "nucleus/logger/logger.h"
#include "nucleus/ui/ui.h"

#define STB_IMAGE_IMPLEMENTATION
#include "externals/stb/stb_image.h"

namespace ui {

WidgetImage::~WidgetImage() {
    stbi_image_free(imBuffer);
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

    texture = manager->graphics->createTexture(textureDesc);
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

    /**
     * Screen
     * ======
     *  (0,1)                              (1,1)
     *  +--------------------------------------+
     *  |   V1 = (x1,y2)       V3 = (x2,y2)    |
     *  |     + - - - - - - - +                |
     *  |     : \             :                |
     *  |     :    \          :                |
     *  |     :       \       : h              |
     *  |     :          \    :                |
     *  |     :       w     \ :                |
     *  |     + - - - - - - - +                |
     *  |   V0 = (x1,y1)       V2 = (x2,y1)    |
     *  +--------------------------------------+
     *  (0,0)                              (1,0)
     */
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

    V0.background = style.background;
    V1.background = style.background;
    V2.background = style.background;
    V3.background = style.background;

    // Texture coordinates assume top-left is (0,0) and bottom-right is (1,1)
    V0.texcoord[0] = 0.0; V0.texcoord[1] = 1.0;
    V1.texcoord[0] = 0.0; V1.texcoord[1] = 0.0;
    V2.texcoord[0] = 1.0; V2.texcoord[1] = 1.0;
    V3.texcoord[0] = 1.0; V3.texcoord[1] = 0.0;

    manager->bindImage(texture);
    manager->renderWidget(input);
}

}  // namespace ui
