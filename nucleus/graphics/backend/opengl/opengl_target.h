/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/target.h"
#include "nucleus/graphics/backend/opengl/opengl.h"

namespace graphics {

class OpenGLColorTarget : public IColorTarget {
public:
    GLuint framebuffer;
    GLint drawbuffer;
};

class OpenGLDepthStencilTarget : public IDepthStencilTarget {
public:
    GLuint framebuffer;
};

}  // namespace graphics
