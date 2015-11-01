/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/pipeline.h"
#include "nucleus/graphics/backend/opengl/opengl.h"

namespace gfx {

class OpenGLPipelineState : public IPipelineState {
public:
    // Blending
    GLenum sfactor;
    GLenum dfactor;
};

}  // namespace gfx
