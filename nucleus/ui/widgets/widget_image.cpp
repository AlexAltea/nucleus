/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "widget_image.h"
#include "nucleus/common.h"
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
    float width = 0.0;
    float height = 0.0;

    if (style.width.type == Length::TYPE_UNDEFINED) {
        width = getCoordX(Length{ double(imWidth), Length::TYPE_PX });
    } else {
        width = getCoordX(style.width);
    }
    if (style.height.type == Length::TYPE_UNDEFINED) {
        height = getCoordY(Length{ double(imHeight), Length::TYPE_PX });
    } else {
        height = getCoordY(style.height);
    }
}

void WidgetImage::render() {
    manager->renderWidget(input);
}

}  // namespace ui
