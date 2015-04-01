/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/widget.h"

// OpenGL dependencies
#include "nucleus/opengl.h"

#include <string>

class WidgetImage : public Widget
{
    struct Image {
        unsigned char* buffer = nullptr;
        int width;
        int height;
        int components;
    } image;

    // OpenGL information
    GLuint textureId;

public:
    ~WidgetImage();

    // Read PNG from file
    void init(const std::string& pngfile);

    // Read PNG from buffer
    void init(const unsigned char* pngbuffer, size_t size);

    // Render the image
    virtual void render() override;
};
