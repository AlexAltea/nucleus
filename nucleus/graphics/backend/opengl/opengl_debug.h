/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/backend/opengl/opengl.h"

namespace gfx {
namespace opengl {

/**
 * Callback for OpenGL debug messages. Has to match the signature of GLDEBUGPROC.
 * @param[in]  source     Message source
 * @param[in]  type       Message type
 * @param[in]  id         Message ID
 * @param[in]  severity   Message severity
 * @param[in]  length     Length of the message string
 * @param[in]  message    Message string
 * @param[in]  userParam  User supplied pointer specified on glDebugMessageCallback
 */
void OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam);

}  // namespace opengl
}  // namespace gfx
