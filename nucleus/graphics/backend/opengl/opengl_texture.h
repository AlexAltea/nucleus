/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/texture.h"
#include "nucleus/graphics/backend/opengl/opengl.h"

namespace gfx {

class OpenGLTexture : public Texture {
public:
    GLuint id;
};

}  // namespace gfx
