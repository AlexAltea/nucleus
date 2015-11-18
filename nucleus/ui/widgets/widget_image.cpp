/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "widget_image.h"
#include "nucleus/common.h"
#include "nucleus/logger/logger.h"

#include "externals/stb/stb_image.h"

namespace ui {

WidgetImage::~WidgetImage() {
    delete image.buffer;
}

void WidgetImage::init(const std::string& pngfile) {
    // Get file contents
    FILE* file = fopen(pngfile.c_str(), "rb");
    if (!file) {
        logger.error(LOG_UI, "Image not found: %s", pngfile.c_str());
        return;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    auto* pngbuffer = new unsigned char[size + 1];
    fseek(file, 0, SEEK_SET);
    fread(pngbuffer, size, 1, file);
    fclose(file);

    init(pngbuffer, size);
}

void WidgetImage::init(const unsigned char* pngbuffer, size_t size) {
    /**
     * NOTE: STB generates stores the image rows in reverse order with respect to the format OpenGL expects.
     * Vertical quad coordinates are swapped on rendering to make sure the image shows up properly.
     */
    image.buffer = stbi_load_from_memory(pngbuffer, size, &image.width, &image.height, &image.components, 4);
}

void WidgetImage::render(gfx::CommandBuffer* cmdBuffer) {
    Length width = style.width;
    Length height = style.height;

    if (style.sizeMode == PROPORTION_AUTOWIDTH) {
        width = height * float(image.width) / float(image.height);
    }
    if (style.sizeMode == PROPORTION_AUTOHEIGHT) {
        height = width * float(image.height) / float(image.width);
    }
    height = Widget::correctHeight(height);

    cmdBuffer->cmdSetPrimitiveTopology(gfx::TOPOLOGY_QUAD_LIST);
    cmdBuffer->cmdSetVertexBuffers(0, nullptr); // TODO
    cmdBuffer->cmdDraw(0, 4, 0, 1);
}

}  // namespace ui
