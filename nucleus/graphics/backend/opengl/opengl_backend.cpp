/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_backend.h"
#include "nucleus/logger/logger.h"

#include "nucleus/graphics/backend/opengl/opengl_command_buffer.h"
#include "nucleus/graphics/backend/opengl/opengl_command_queue.h"
#include "nucleus/graphics/backend/opengl/opengl_fence.h"
#include "nucleus/graphics/backend/opengl/opengl_heap.h"
#include "nucleus/graphics/backend/opengl/opengl_pipeline.h"
#include "nucleus/graphics/backend/opengl/opengl_shader.h"
#include "nucleus/graphics/backend/opengl/opengl_target.h"
#include "nucleus/graphics/backend/opengl/opengl_texture.h"

namespace gfx {

thread_local OpenGLContext gCurrentContext = 0;

OpenGLBackend::OpenGLBackend() : IBackend() {
}

OpenGLBackend::~OpenGLBackend() {
}

OpenGLContext OpenGLBackend::createContext() {
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    static const int contextAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
        0};
    gCurrentContext = wglCreateContextAttribsARB(parameters.hdc, context, contextAttribs);
#elif defined(NUCLEUS_PLATFORM_LINUX) || defined(NUCLEUS_PLATFORM_OSX)
    gCurrentContext = glXCreateContext(parameters.display, info, NULL, GL_TRUE);
#endif
    return gCurrentContext;
}

bool OpenGLBackend::initialize(const BackendParameters& params) {
    parameters = params;

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
#elif defined(NUCLEUS_PLATFORM_LINUX) || defined(NUCLEUS_PLATFORM_OSX)
    if (!initializeOpenGL()) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::initialize: Could not initialize all OpenGL extensions");
        return false;
    }
    XVisualInfo* info = glXChooseVisual(display, 0, nullptr/*TODO*/);
#elif defined(NUCLEUS_PLATFORM_ANDROID) || defined(NUCLEUS_PLATFORM_IOS)
    if (!initializeOpenGL()) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::initialize: Could not initialize all OpenGL extensions");
        return false;
    }
#endif

    // Initialize context
    context = createContext();
    if (!context) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::initialize: createContext failed");
        return false;
    }

    auto* backBuffer = new OpenGLColorTarget();
    backBuffer->attached = true;
    backBuffer->framebuffer = 0;
    backBuffer->drawbuffer = 0;
    screenBackBuffer = backBuffer;

    return true;
}

CommandQueue* OpenGLBackend::createCommandQueue() {
    queue = new OpenGLCommandQueue();

    if (!queue->initialize(parameters, context)) {
        logger.error(LOG_GRAPHICS, "OpenGLBackend::createCommandQueue: Could not initialize OpenGLCommandQueue");
        return nullptr;
    }
    return queue;
}

CommandBuffer* OpenGLBackend::createCommandBuffer() {
    auto* commandBuffer = new OpenGLCommandBuffer();
    return commandBuffer;
}

Fence* OpenGLBackend::createFence(const FenceDesc& desc) {
    auto* heap = new OpenGLFence();
    return heap;
}

Heap* OpenGLBackend::createHeap(const HeapDesc& desc) {
    auto* heap = new OpenGLHeap();
    return heap;
}

ColorTarget* OpenGLBackend::createColorTarget(Texture* texture) {

    return nullptr;
}

DepthStencilTarget* OpenGLBackend::createDepthStencilTarget(Texture* texture) {
    return nullptr;
}

Pipeline* OpenGLBackend::createPipeline(const PipelineDesc& desc) {
    auto* pipeline = new OpenGLPipeline();
    // TODO

    return pipeline;
}

Shader* OpenGLBackend::createShader(const ShaderDesc& desc) {
    if (!gCurrentContext) {
        createContext();
    }
    auto* shader = new OpenGLShader();
    shader->initialize(desc);
    return shader;
}

Texture* OpenGLBackend::createTexture(const TextureDesc& desc) {
    if (!gCurrentContext) {
        createContext();
    }
    auto* texture = new OpenGLTexture();
    glGenTextures(1, &texture->id);
    return texture;
}

VertexBuffer* OpenGLBackend::createVertexBuffer(const VertexBufferDesc& desc) {
    if (!gCurrentContext) {
        createContext();
    }
    auto* vtxBuffer = new OpenGLVertexBuffer();
    glGenBuffers(1, &vtxBuffer->id);
    return vtxBuffer;
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
