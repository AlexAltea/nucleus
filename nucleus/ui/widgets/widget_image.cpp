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

void WidgetImage::init(const std::string& pngfile) {
    // Get file contents
    std::FILE* file;
    fopen_s(&file, pngfile.c_str(), "rb");
    if (!file) {
        logger.error(LOG_UI, "Image not found: %s", pngfile.c_str());
        return;
    }

    fseek(file, 0, SEEK_END);
    Size size = ftell(file);
    std::vector<Byte> pngBuffer(size + 1);
    fseek(file, 0, SEEK_SET);
    fread(pngBuffer.data(), size, 1, file);
    fclose(file);

    init(pngBuffer.data(), size);
}

void WidgetImage::init(const Byte* pngbuffer, Size size) {
    /**
     * NOTE: STB generates stores the image rows in reverse order with respect to the format nucleus::gfx expects.
     * Vertical quad coordinates are swapped on rendering to make sure the image shows up properly.
     */
    imBuffer = stbi_load_from_memory(pngbuffer, size, &imWidth, &imHeight, &imComponents, 4);
}

void WidgetImage::render() {
    Length width = style.width;
    Length height = style.height;

    if (style.sizeMode == PROPORTION_AUTOWIDTH) {
        width = height * float(imWidth) / float(imHeight);
    }
    if (style.sizeMode == PROPORTION_AUTOHEIGHT) {
        height = width * float(imHeight) / float(imWidth);
    }
    height = Widget::correctHeight(height);

    WidgetInput input;
    manager->renderWidget(input);
}

}  // namespace ui
