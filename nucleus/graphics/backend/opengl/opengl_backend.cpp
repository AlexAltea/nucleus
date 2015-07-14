/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_backend.h"
#include "nucleus/graphics/backend/opengl/opengl_command_buffer.h"

namespace graphics {

OpenGLBackend::OpenGLBackend()
{
}

ICommandBuffer* OpenGLBackend::CreateCommandBuffer()
{
    ICommandBuffer* commandBuffer = new OpenGLCommandBuffer();
    return commandBuffer;
}

}  // namespace graphics
