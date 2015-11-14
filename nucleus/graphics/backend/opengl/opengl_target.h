/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/target.h"
#include "nucleus/graphics/backend/opengl/opengl.h"

namespace gfx {

class OpenGLColorTarget : public IColorTarget {
public:
    // Texture ID holding this color buffer
    GLuint texture;

    // True if this target is attached to the framebuffer below
    bool attached;

    GLuint framebuffer;
    GLint drawbuffer;
};

class OpenGLDepthStencilTarget : public IDepthStencilTarget {
public:
    // Texture ID holding this depth-stencil buffer
    GLuint texture;

    // True if this target is attached to the framebuffer below
    bool attached;

    GLuint framebuffer;
    GLint drawbuffer;
};

}  // namespace gfx
