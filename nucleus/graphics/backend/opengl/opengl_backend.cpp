/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_backend.h"
#include "nucleus/graphics/backend/opengl/opengl_command_buffer.h"

namespace gfx {

OpenGLBackend::OpenGLBackend() {
}

ICommandBuffer* OpenGLBackend::createCommandBuffer() {
    ICommandBuffer* commandBuffer = new OpenGLCommandBuffer();
    return commandBuffer;
}

}  // namespace gfx
