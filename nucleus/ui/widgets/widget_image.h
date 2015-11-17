/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/widget.h"

#include <string>

namespace ui {

class WidgetImage : public Widget {
    struct Image {
        unsigned char* buffer = nullptr;
        int width;
        int height;
        int components;
    } image;

public:
    ~WidgetImage();

    // Read PNG from file
    void init(const std::string& pngfile);

    // Read PNG from buffer
    void init(const unsigned char* pngbuffer, size_t size);

    // Render the image
    virtual void render(gfx::CommandBuffer* cmdBuffer) override;
};

}  // namespace ui
