/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/pipeline.h"
#include "nucleus/graphics/backend/opengl/opengl.h"

namespace gfx {
namespace opengl {

/**
 * OpenGL Pipeline
 * ===============
 * Holds the state of an OpenGL graphics pipeline.
 *
 * Notes:
 *  - Generation of the VAO happens on the main command queue since
 *    it's not possible to share VAO objects between contexts.
 */
class OpenGLPipeline : public Pipeline {
public:
    GLuint program;

    // Input Layout
    std::vector<InputElement> vaoDesc;
    GLuint vao;

    // Blending
    GLenum sfactor;
    GLenum dfactor;
};

}  // namespace opengl
}  // namespace gfx
