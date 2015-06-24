/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/widget.h"

// OpenGL dependencies
#include "nucleus/opengl.h"

#include <string>

class WidgetText : public Widget
{
    // OpenGL information
    GLuint textureId;

public:
    // Update the texture given a text string
    void update(const std::string& text);

    // Render the image
    virtual void render() override;
};
