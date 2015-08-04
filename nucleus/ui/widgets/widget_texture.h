/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/widget.h"

// OpenGL dependencies
#include "nucleus/opengl.h"

class WidgetTexture : public Widget {
public:
    // OpenGL information
    GLuint textureId;

    // Initialize from the texture ID pointer
    void init(GLuint* id);

    // Render the image
    virtual void render() override;
};
