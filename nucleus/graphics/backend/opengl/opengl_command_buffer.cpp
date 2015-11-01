/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_command_buffer.h"
#include "nucleus/graphics/backend/opengl/opengl_target.h"
#include "nucleus/logger/logger.h"

namespace gfx {

bool OpenGLCommandBuffer::reset() {
    commands.clear();
    return true;
}

void OpenGLCommandBuffer::cmdClearColor(IColorTarget* target, const F32* colorValue) {
    auto glTarget = static_cast<OpenGLColorTarget*>(target);
    if (!glTarget) {
        logger.error(LOG_GRAPHICS, "OpenGLCommandBuffer::cmdClearColor: Invalid target specified");
        return;
    }

    OpenGLCommand command;
    command.type = OPENGL_CMD_CLEAR_COLOR;
    command.data.clearColor.framebuffer = glTarget->framebuffer;
    command.data.clearColor.drawbuffer = glTarget->drawbuffer;
    command.data.clearColor.r = colorValue[0];
    command.data.clearColor.g = colorValue[1];
    command.data.clearColor.b = colorValue[2];
    command.data.clearColor.a = colorValue[3];
    commands.push_back(command);
}

void OpenGLCommandBuffer::cmdClearDepthStencil(IDepthStencilTarget* target, F32 depthValue, U8 stencilValue) {
    auto glTarget = static_cast<OpenGLDepthStencilTarget*>(target);
    if (!glTarget) {
        logger.error(LOG_GRAPHICS, "OpenGLCommandBuffer::cmdClearDepthStencil: Invalid target specified");
        return;
    }

    OpenGLCommand command;
    command.type = OPENGL_CMD_CLEAR_DEPTH_STENCIL;
    command.data.clearDepthStencil.framebuffer = glTarget->framebuffer;
    command.data.clearDepthStencil.depth = depthValue;
    command.data.clearDepthStencil.stencil = stencilValue;
    commands.push_back(command);
}

}  // namespace gfx
