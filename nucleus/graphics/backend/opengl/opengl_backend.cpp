/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_backend.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/backend/opengl/opengl_command_buffer.h"

namespace gfx {

OpenGLBackend::OpenGLBackend(DisplayHandler display) : IBackend(display) {
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    static const int contextAttribs[] = {
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB, 0
    };
    context = wglCreateContextAttribsARB(display, 0, contextAttribs);
    if (!context) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::OpenGLBackend: wglCreateContextAttribsARB failed");
    }
    if (!wglMakeCurrent(display, context)) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::OpenGLBackend: wglMakeCurrent failed");
        return;
    }
#elif defined(NUCLEUS_PLATFORM_LINUX) || defined(NUCLEUS_PLATFORM_OSX)
    XVisualInfo* info = glXChooseVisual(display, 0, nullptr/*TODO*/);
    context = glXCreateContext(display, info, NULL, GL_TRUE);
    if (!context) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::OpenGLBackend: glXCreateContext failed");
    }
    if (!glXMakeCurrent(display, 0/*TODO*/, context)) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::OpenGLBackend: glXMakeCurrent failed");
        return;
    }
#elif defined(NUCLEUS_PLATFORM_ANDROID) || defined(NUCLEUS_PLATFORM_IOS)
#endif
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
