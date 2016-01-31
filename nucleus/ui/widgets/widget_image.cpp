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

void WidgetImage::init(core::ResourceName resName) {
    // Open resource
    core::Resource res(resName);
    const Byte* data = reinterpret_cast<Byte*>(res.data);
    init(data, res.size);
}

void WidgetImage::init(const std::string& filepath) {
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

    init(buffer.data(), size);
}

void WidgetImage::init(const Byte* buffer, Size size) {
    imBuffer = stbi_load_from_memory(buffer, size, &imWidth, &imHeight, &imComponents, 4);
}

void WidgetImage::dimensionalize() {
    vertWidth = 0.0;
    vertHeight = 0.0;

    if (style.width.type == Length::TYPE_UNDEFINED) {
        vertWidth = getCoordX(Length{ double(imWidth), Length::TYPE_PX });
    } else {
        vertWidth = getCoordX(style.width);
    }
    if (style.height.type == Length::TYPE_UNDEFINED) {
        vertHeight = getCoordY(Length{ double(imHeight), Length::TYPE_PX });
    } else {
        vertHeight = getCoordY(style.height);
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

    manager->renderWidget(input);
}

}  // namespace ui
