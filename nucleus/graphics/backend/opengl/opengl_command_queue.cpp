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

OpenGLCommandQueue::OpenGLCommandQueue() {
}

OpenGLCommandQueue::~OpenGLCommandQueue() {
}

bool OpenGLCommandQueue::initialize(const BackendParameters& params, OpenGLContext context) {
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    if (!wglMakeCurrent(params.hdc, context)) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::initialize: wglMakeCurrent failed");
        return false;
    }
#elif defined(NUCLEUS_PLATFORM_LINUX) || defined(NUCLEUS_PLATFORM_OSX)
    if (!glXMakeCurrent(display, 0/*TODO*/, context)) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::initialize: glXMakeCurrent failed");
        return false;
    }
#elif defined(NUCLEUS_PLATFORM_ANDROID) || defined(NUCLEUS_PLATFORM_IOS)
    if (!eglMakeCurrent(/*TODO*/)) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::initialize: eglMakeCurrent failed");
        return false;
    }
#endif

    // Start command processing thread
    thread = std::thread([&] {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock);

            while (!commandBuffers.empty()) {
                const auto& buffer = commandBuffers.front();
                for (const auto* cmd : buffer->commands) {
                    execute(*cmd);
                }
                commandBuffers.pop();
            }
        }
    });

    return true;
}

void OpenGLCommandQueue::execute(const OpenGLCommand& cmd) {
    switch (cmd.type) {
    case OpenGLCommand::TYPE_CLEAR_COLOR:
        execute(static_cast<const OpenGLCommandClearColor&>(cmd));
        break;
    case OpenGLCommand::TYPE_CLEAR_DEPTH_STENCIL:
        execute(static_cast<const OpenGLCommandClearDepthStencil&>(cmd));
        break;

    default:
        logger.error(LOG_GRAPHICS, "OpenGLCommandQueue::execute: Unrecognized command (%d)", cmd.type);
    }
}

void OpenGLCommandQueue::execute(const OpenGLCommandClearColor& cmd) {
    const GLuint framebuffer = cmd.framebuffer;
    const GLint drawbuffer = cmd.drawbuffer;
    const GLfloat value[4] = { cmd.r, cmd.g, cmd.b, cmd.a };

#if defined(GRAPHICS_OPENGL_GL45)
    glClearNamedFramebufferfv(framebuffer, GL_COLOR, drawbuffer, value);
#elif defined(GRAPHICS_OPENGL_GLES31)
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearBufferfv(GL_COLOR, drawbuffer, value);
#endif

    checkBackendError("OpenGLCommandQueue::execute: cmdClearColor");
}

void OpenGLCommandQueue::execute(const OpenGLCommandClearDepthStencil& cmd) {
    const GLuint framebuffer = cmd.framebuffer;
    const GLint drawbuffer = cmd.drawbuffer;
    const GLfloat depth = cmd.depth;
    const GLint stencil = cmd.stencil;

#if defined(GRAPHICS_OPENGL_GL45)
    glClearNamedFramebufferfi(framebuffer, GL_DEPTH_STENCIL, drawbuffer, depth, stencil);
#elif defined(GRAPHICS_OPENGL_GLES31)
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
#endif

    checkBackendError("OpenGLCommandQueue::execute: cmdClearDepthStencil");
}

void OpenGLCommandQueue::execute(const OpenGLCommandSetTargets& cmd) {
    checkBackendError("OpenGLCommandQueue::execute: cmdClearDepthStencil");
}

void OpenGLCommandQueue::submit(ICommandBuffer* cmdBuffer) {
    commandBuffers.push(dynamic_cast<OpenGLCommandBuffer*>(cmdBuffer));
    cv.notify_one();
}

void OpenGLCommandQueue::waitIdle() {
}

}  // namespace gfx
