/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_command_buffer.h"
#include "nucleus/graphics/backend/opengl/opengl_target.h"
#include "nucleus/logger/logger.h"

namespace gfx {

OpenGLCommandBuffer::OpenGLCommandBuffer() {
}

OpenGLCommandBuffer::~OpenGLCommandBuffer() {
}

bool OpenGLCommandBuffer::reset() {
    commands.clear();
    return true;
}

void OpenGLCommandBuffer::cmdBindPipeline(IPipelineState* pipeline) {
}

void OpenGLCommandBuffer::cmdClearColor(IColorTarget* target, const F32* colorValue) {
    auto glTarget = static_cast<OpenGLColorTarget*>(target);
    if (!glTarget) {
        logger.error(LOG_GRAPHICS, "OpenGLCommandBuffer::cmdClearColor: Invalid target specified");
        return;
    }

    auto* command = new OpenGLCommandClearColor();
    command->framebuffer = glTarget->framebuffer;
    command->drawbuffer = glTarget->drawbuffer;
    command->r = colorValue[0];
    command->g = colorValue[1];
    command->b = colorValue[2];
    command->a = colorValue[3];

    commands.push_back(command);
}

void OpenGLCommandBuffer::cmdClearDepthStencil(IDepthStencilTarget* target, F32 depthValue, U8 stencilValue) {
    auto glTarget = static_cast<OpenGLDepthStencilTarget*>(target);
    if (!glTarget) {
        logger.error(LOG_GRAPHICS, "OpenGLCommandBuffer::cmdClearDepthStencil: Invalid target specified");
        return;
    }

    auto* command = new OpenGLCommandClearDepthStencil();
    command->framebuffer = glTarget->framebuffer;
    command->drawbuffer = glTarget->drawbuffer;
    command->depth = depthValue;
    command->stencil = stencilValue;

    commands.push_back(command);
}

void OpenGLCommandBuffer::cmdDraw() {
}

void OpenGLCommandBuffer::cmdDrawIndexed() {
}

void OpenGLCommandBuffer::cmdDrawIndirect() {
}

void OpenGLCommandBuffer::cmdDrawIndexedIndirect() {
}

}  // namespace gfx
