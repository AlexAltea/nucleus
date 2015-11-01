/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_backend.h"
#include "nucleus/graphics/backend/opengl/opengl_command_buffer.h"

namespace gfx {

OpenGLBackend::OpenGLBackend() {
}

OpenGLBackend::~OpenGLBackend() {
}

ICommandQueue* OpenGLBackend::createCommandQueue() {
    return nullptr;
}

ICommandBuffer* OpenGLBackend::createCommandBuffer() {
    ICommandBuffer* commandBuffer = new OpenGLCommandBuffer();
    return commandBuffer;
}

void OpenGLBackend::createPipeline() {
}

void OpenGLBackend::createShader() {
}

}  // namespace gfx
