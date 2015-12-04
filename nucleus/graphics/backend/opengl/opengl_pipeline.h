/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/pipeline.h"
#include "nucleus/graphics/backend/opengl/opengl.h"

namespace gfx {

class OpenGLPipeline : public Pipeline {
public:
    GLuint program;
    GLuint vao;

    // Blending
    GLenum sfactor;
    GLenum dfactor;
};

}  // namespace gfx
