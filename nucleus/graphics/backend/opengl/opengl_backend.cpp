/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_backend.h"
#include "nucleus/assert.h"
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
namespace opengl {

thread_local OpenGLContext gCurrentContext = 0;

OpenGLBackend::OpenGLBackend() : IBackend() {
}

OpenGLBackend::~OpenGLBackend() {
}

void OpenGLBackend::useAvailableContext() {
    static size_t index = 0;
    assert_true(index < 8, "OpenGLBackend::getContext: Too many OpenGL sub-contexts requested");
    gCurrentContext = subContext[index++];
#if defined(GRAPHICS_OPENGL_API_WGL)
    wglMakeCurrent(parameters.hdc, gCurrentContext);
#endif
}

OpenGLContext OpenGLBackend::createContext() {
    OpenGLContext newContext;
#if defined(GRAPHICS_OPENGL_API_WGL)
    static const int contextAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
        0};
    newContext = wglCreateContextAttribsARB(parameters.hdc, context, contextAttribs);
#elif defined(GRAPHICS_OPENGL_API_GLX)
    newContext = glXCreateContext(parameters.display, info, NULL, GL_TRUE);
#elif defined(GRAPHICS_OPENGL_API_EGL)
    newContext = eglCreateContext(0, 0, 0, nullptr); // TODO
#endif
    return newContext;
}

bool OpenGLBackend::initialize(const BackendParameters& params) {
    parameters = params;

#if defined(GRAPHICS_OPENGL_API_WGL)
    // Retrieving OpenGL extension pointers requires in Windows owning a context
    HGLRC dummyRc = wglCreateContext(params.hdc);
    wglMakeCurrent(params.hdc, dummyRc);
    if (!initializeOpenGL()) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::initialize: Could not initialize all OpenGL extensions");
        return false;
    }
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(dummyRc);
#elif defined(GRAPHICS_OPENGL_API_GLX)
    if (!initializeOpenGL()) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::initialize: Could not initialize all OpenGL extensions");
        return false;
    }
    XVisualInfo* info = glXChooseVisual(display, 0, nullptr/*TODO*/);
#elif defined(GRAPHICS_OPENGL_API_EGL)
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

    // NOTE: Sharing lists on other threads fails, therefore potential new contexts are created at initialization
    for (unsigned int i = 0; i < 8; i++) {
        subContext[i] = createContext();
    }

    auto* backBuffer = new OpenGLColorTarget();
    backBuffer->attached = true;
    backBuffer->framebuffer = 0;
    backBuffer->drawbuffer = 0;

    screenBackTarget = backBuffer;
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
    if (!gCurrentContext) {
        useAvailableContext();
    }

    // Create program, then attach and link shaders to it
    GLuint program = glCreateProgram();
    for (const auto& shader : { desc.vs, desc.hs, desc.ds, desc.gs, desc.ps }) {
        auto* glShader = static_cast<OpenGLShader*>(shader);
        if (!glShader) {
            continue;
        }
        glAttachShader(program, glShader->id);
    }
    glLinkProgram(program);

    // Check if program linked succesfully
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        GLint length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        
        std::vector<GLchar> infoLog(length);
        glGetProgramInfoLog(program, infoLog.size(), nullptr, infoLog.data());
        logger.error(LOG_GPU, "OpenGLBackend::createPipeline: Cannot link program:\n%s", infoLog.data());
        return nullptr;
    }
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if (status != GL_TRUE) {
        GLint length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        
        std::vector<GLchar> infoLog(length);
        glGetProgramInfoLog(program, infoLog.size(), nullptr, infoLog.data());
        logger.error(LOG_GPU, "OpenGLBackend::createPipeline: Cannot validate program:\n%s", infoLog.data());
        return nullptr;
    }

    // Prepare OpenGL pipeline object
    auto* pipeline = new OpenGLPipeline();
    pipeline->program = program;
    pipeline->vao = 0;
    pipeline->vaoDesc = desc.iaState.inputLayout;
    return pipeline;
}

Shader* OpenGLBackend::createShader(const ShaderDesc& desc) {
    if (!gCurrentContext) {
        useAvailableContext();
    }

    GLenum glType;
    switch (desc.type) {
    case SHADER_TYPE_VERTEX:
        glType = GL_VERTEX_SHADER; break;
    case SHADER_TYPE_PIXEL:
        glType = GL_FRAGMENT_SHADER; break;
#ifdef GRAPHICS_OPENGL_GL
    case SHADER_TYPE_HULL:
        glType = GL_TESS_CONTROL_SHADER; break;
    case SHADER_TYPE_DOMAIN:
        glType = GL_TESS_EVALUATION_SHADER; break;
    case SHADER_TYPE_GEOMETRY:
        glType = GL_GEOMETRY_SHADER; break;
#endif
    default:
        assert_always("Unimplemented case");
    }

    auto* shader = new OpenGLShader();
    shader->id = glCreateShader(glType);
    shader->initialize(desc);
    return shader;
}

Texture* OpenGLBackend::createTexture(const TextureDesc& desc) {
    if (!gCurrentContext) {
        useAvailableContext();
    }
    auto* texture = new OpenGLTexture();
    glGenTextures(1, &texture->id);
    return texture;
}

VertexBuffer* OpenGLBackend::createVertexBuffer(const VertexBufferDesc& desc) {
    if (!gCurrentContext) {
        useAvailableContext();
    }

    GLsizeiptr size = desc.size;
    GLenum usage = GL_DYNAMIC_DRAW;
    auto* vtxBuffer = new OpenGLVertexBuffer(size, usage);
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

}  // namespace opengl
}  // namespace gfx
