/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_command_queue.h"
#include "nucleus/logger/logger.h"

#define checkBackendError(name) { \
    GLenum error = glGetError(); \
    if (error != GL_NO_ERROR) { \
        logger.error(LOG_GPU, "Something went wrong in %s. Error code: %x", name, error); \
    } \
}

namespace gfx {

void OpenGLCommandQueue::execute(const OpenGLCommand& cmd) {
    switch (cmd.type) {
    case OPENGL_CMD_CLEAR_COLOR:
        executeClearColor(cmd.data);
        break;

    case OPENGL_CMD_CLEAR_DEPTH_STENCIL:
        executeClearDepthStencil(cmd.data);
        break;

    default:
        logger.error(LOG_GRAPHICS, "OpenGLCommandQueue::execute: Unrecognized command (%d)", cmd.type);
    }
}

void OpenGLCommandQueue::executeClearColor(const OpenGLCommandData& data) {
    const GLuint framebuffer = data.clearColor.framebuffer;
    const GLint drawbuffer = data.clearColor.drawbuffer;
    const GLfloat value[4] = {
        data.clearColor.r,
        data.clearColor.g,
        data.clearColor.b,
        data.clearColor.a
    };

#if defined(GRAPHICS_OPENGL_GL45)
    glClearNamedFramebufferfv(framebuffer, GL_COLOR, drawbuffer, value);
#elif defined(GRAPHICS_OPENGL_GLES31)
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearBufferfv(GL_COLOR, drawbuffer, value);
#endif

    checkBackendError("OpenGLCommandQueue::executeClearColor");
}

void OpenGLCommandQueue::executeClearDepthStencil(const OpenGLCommandData& data) {
    const GLuint framebuffer = data.clearDepthStencil.framebuffer;
    const GLfloat depth = data.clearDepthStencil.depth;
    const GLint stencil = data.clearDepthStencil.stencil;

#if defined(GRAPHICS_OPENGL_GL45)
    glClearNamedFramebufferfi(framebuffer, GL_DEPTH_STENCIL, depth, stencil);
#elif defined(GRAPHICS_OPENGL_GLES31)
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
#endif

    checkBackendError("OpenGLCommandQueue::executeClearDepthStencil");
}

void OpenGLCommandQueue::submit(ICommandBuffer* cmdBuffer) {
    commandBuffers.push(dynamic_cast<OpenGLCommandBuffer*>(cmdBuffer));
}

void OpenGLCommandQueue::waitIdle() {
}

}  // namespace gfx
