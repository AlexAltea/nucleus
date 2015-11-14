/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_backend.h"
#include "nucleus/logger/logger.h"

#include "nucleus/graphics/backend/opengl/opengl_command_buffer.h"
#include "nucleus/graphics/backend/opengl/opengl_command_queue.h"
#include "nucleus/graphics/backend/opengl/opengl_heap.h"
#include "nucleus/graphics/backend/opengl/opengl_target.h"
#include "nucleus/graphics/backend/opengl/opengl_texture.h"

namespace gfx {

OpenGLBackend::OpenGLBackend() : IBackend() {
}

OpenGLBackend::~OpenGLBackend() {
}

bool OpenGLBackend::initialize(const BackendParameters& params) {
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    // Retrieving OpenGL extension pointers requires in Windows owning a context
    HGLRC dummyRc = wglCreateContext(params.hdc);
    wglMakeCurrent(params.hdc, dummyRc);
    if (!initializeOpenGL()) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::initialize: Could not initialize all OpenGL extensions");
        return false;
    }
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(dummyRc);

    // Create context and make it current
    static const int contextAttribs[] = { WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB, 0 };
    context = wglCreateContextAttribsARB(params.hdc, 0, contextAttribs);
    if (!context) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::initialize: wglCreateContextAttribsARB failed");
        return false;
    }
#elif defined(NUCLEUS_PLATFORM_LINUX) || defined(NUCLEUS_PLATFORM_OSX)
    if (!initializeOpenGL()) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::initialize: Could not initialize all OpenGL extensions");
        return false;
    }
    XVisualInfo* info = glXChooseVisual(display, 0, nullptr/*TODO*/);
    context = glXCreateContext(params.display, info, NULL, GL_TRUE);
    if (!context) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::initialize: glXCreateContext failed");
        return false;
    }
#elif defined(NUCLEUS_PLATFORM_ANDROID) || defined(NUCLEUS_PLATFORM_IOS)
    if (!initializeOpenGL()) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::initialize: Could not initialize all OpenGL extensions");
        return false;
    }
#endif

    auto* backBuffer = new OpenGLColorTarget();
    backBuffer->attached = true;
    backBuffer->framebuffer = 0;
    backBuffer->drawbuffer = 0;
    screenBackBuffer = backBuffer;

    parameters = params;
    return true;
}

ICommandQueue* OpenGLBackend::createCommandQueue() {
    auto* commandQueue = new OpenGLCommandQueue();

    if (!commandQueue->initialize(parameters, context)) {
        logger.error(LOG_GRAPHICS, "OpenGLBackend::createCommandQueue: Could not initialize OpenGLCommandQueue");
        return nullptr;
    }
    return commandQueue;
}

ICommandBuffer* OpenGLBackend::createCommandBuffer() {
    auto* commandBuffer = new OpenGLCommandBuffer();
    return commandBuffer;
}

IHeap* OpenGLBackend::createHeap(const HeapDesc& desc) {
    auto* heap = new OpenGLHeap();
    return heap;
}

IColorTarget* OpenGLBackend::createColorTarget(ITexture* texture) {

    return nullptr;
}

IDepthStencilTarget* OpenGLBackend::createDepthStencilTarget(ITexture* texture) {
    return nullptr;
}

void OpenGLBackend::createPipeline() {
}

void OpenGLBackend::createShader() {
}

ITexture* OpenGLBackend::createTexture(const TextureDesc& desc) {
    auto* texture = new OpenGLTexture();

    glGenTextures(1, &texture->id);
    return nullptr;
}

bool OpenGLBackend::doSwapBuffers() {
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    SwapBuffers(parameters.hdc);
#elif defined(NUCLEUS_PLATFORM_LINUX)
    glXSwapBuffers(parameters.display, window);
#elif defined(NUCLEUS_PLATFORM_OSX)
    glXSwapBuffers(parameters.display, window);
#elif defined(NUCLEUS_PLATFORM_ANDROID)
    eglSwapBuffers(parameters.display, surface);
#elif defined(NUCLEUS_PLATFORM_IOS)
    eglSwapBuffers(parameters.display, surface);
#endif
    return true;
}

}  // namespace gfx
