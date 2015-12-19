/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_command_queue.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/backend/opengl/opengl_debug.h"

#define checkBackendError(name) { \
    GLenum error = glGetError(); \
    if (error != GL_NO_ERROR) { \
        logger.error(LOG_GPU, "Something went wrong in %s. Error code: %x", name, error); \
    } \
}

namespace gfx {
namespace opengl {

OpenGLCommandQueue::OpenGLCommandQueue() {
}

OpenGLCommandQueue::~OpenGLCommandQueue() {
}

bool OpenGLCommandQueue::initialize(const BackendParameters& params, OpenGLContext context) {
    // Start command processing thread
    thread = std::thread([=] {
        task(params, context);
    });

    return true;
}

void OpenGLCommandQueue::task(const BackendParameters& params, OpenGLContext context) {
    // Set context
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    if (!wglMakeCurrent(params.hdc, context)) {
        logger.warning(LOG_GRAPHICS, "OpenGLCommandQueue::task: wglMakeCurrent failed");
        return;
    }
#elif defined(NUCLEUS_PLATFORM_LINUX) || defined(NUCLEUS_PLATFORM_OSX)
    if (!glXMakeCurrent(display, 0/*TODO*/, context)) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::initialize: glXMakeCurrent failed");
        return;
    }
#elif defined(NUCLEUS_PLATFORM_ANDROID) || defined(NUCLEUS_PLATFORM_IOS)
    if (!eglMakeCurrent(/*TODO*/)) {
        logger.warning(LOG_GRAPHICS, "OpenGLBackend::initialize: eglMakeCurrent failed");
        return;
    }
#endif

    // Logging
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    glDebugMessageCallback(reinterpret_cast<GLDEBUGPROC>(OpenGLDebugCallback), nullptr);

    // Initial scratch state
    glGenFramebuffers(1, &tmpFramebuffer);

    while (true) {
        std::unique_lock<std::mutex> lock(mutex);

        if (work.empty()) {
            cv.wait(lock);
        }

        while (!work.empty()) {
            const auto& workUnit = work.front();
            const auto& commands = workUnit.cmdBuffer->commands;
            for (size_t i = 0; i < commands.size(); i++) {
                execute(*commands[i]);
            }
            if (workUnit.destroyOnCompletion) {
                delete workUnit.cmdBuffer;
            }
            work.pop();
        }
    }
}

void OpenGLCommandQueue::execute(const OpenGLCommand& cmd) {
    switch (cmd.type) {
    case OpenGLCommand::TYPE_BIND_PIPELINE:
        execute(static_cast<const OpenGLCommandBindPipeline&>(cmd));
        break;
    case OpenGLCommand::TYPE_CLEAR_COLOR:
        execute(static_cast<const OpenGLCommandClearColor&>(cmd));
        break;
    case OpenGLCommand::TYPE_CLEAR_DEPTH_STENCIL:
        execute(static_cast<const OpenGLCommandClearDepthStencil&>(cmd));
        break;
    case OpenGLCommand::TYPE_DRAW:
        execute(static_cast<const OpenGLCommandDraw&>(cmd));
        break;
    case OpenGLCommand::TYPE_DRAW_INDEXED:
        execute(static_cast<const OpenGLCommandDrawIndexed&>(cmd));
        break;
    case OpenGLCommand::TYPE_SET_VERTEX_BUFFERS:
        execute(static_cast<const OpenGLCommandSetVertexBuffers&>(cmd));
        break;
    case OpenGLCommand::TYPE_SET_PRIMITIVE_TOPOLOGY:
        execute(static_cast<const OpenGLCommandSetPrimitiveTopology&>(cmd));
        break;
    case OpenGLCommand::TYPE_SET_TARGETS:
        execute(static_cast<const OpenGLCommandSetTargets&>(cmd));
        break;
    case OpenGLCommand::TYPE_SET_VIEWPORTS:
        execute(static_cast<const OpenGLCommandSetViewports&>(cmd));
        break;
    case OpenGLCommand::TYPE_SET_SCISSORS:
        execute(static_cast<const OpenGLCommandSetScissors&>(cmd));
        break;
    case OpenGLCommand::TYPE_INTERNAL_SIGNAL_FENCE:
        execute(static_cast<const OpenGLCommandInternalSignalFence&>(cmd));
        break;
    default:
        logger.error(LOG_GRAPHICS, "OpenGLCommandQueue::execute: Unrecognized command (%d)", cmd.type);
    }
}

void OpenGLCommandQueue::execute(const OpenGLCommandBindPipeline& cmd) {
    auto* glPipeline = static_cast<OpenGLPipeline*>(cmd.pipeline);

    if (!glPipeline->vao) {
        glGenVertexArrays(1, &glPipeline->vao);
        glBindVertexArray(glPipeline->vao);

        for (const auto& inputElement : glPipeline->vaoDesc) {
            GLuint index = inputElement.semanticIndex;
            GLuint slot = inputElement.inputSlot;
            GLsizei stride = inputElement.stride;

            GLint size;
            GLenum type;
            GLboolean normalized;
            switch (inputElement.format) {
            case DATA_FORMAT_R32G32:
                size = 2; type = GL_FLOAT; normalized = GL_FALSE; break;
            case DATA_FORMAT_R32G32B32:
                size = 3; type = GL_FLOAT; normalized = GL_FALSE; break;
            case DATA_FORMAT_R32G32B32A32:
                size = 4; type = GL_FLOAT; normalized = GL_FALSE; break;
            }

            glEnableVertexAttribArray(index);
            glVertexAttribFormat(index, size, type, normalized, 0);
            glVertexAttribBinding(index, slot);
        }
        glPipeline->vaoDesc.clear();
    }
    else {
        glBindVertexArray(glPipeline->vao);
    }

    glUseProgram(glPipeline->program);
    checkBackendError("OpenGLCommandQueue::execute: cmdBindPipeline");
}

void OpenGLCommandQueue::execute(const OpenGLCommandClearColor& cmd) {
    GLuint framebuffer;
    GLint drawbuffer;
    GLfloat value[4] = { cmd.r, cmd.g, cmd.b, cmd.a };
    
    const auto& target = cmd.target;
    if (target->attached) {
        framebuffer = target->framebuffer;
        drawbuffer = target->drawbuffer;
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, tmpFramebuffer);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target->texture, 0);
        framebuffer = tmpFramebuffer;
        drawbuffer = 0;
    }

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

void OpenGLCommandQueue::execute(const OpenGLCommandDraw& cmd) {
    const GLint first = cmd.firstVertex;
    const GLsizei instancecount = cmd.instanceCount;
    const GLsizei count = cmd.vertexCount;
    const GLuint baseinstance = cmd.firstInstance;

    glDrawArraysInstancedBaseInstance(tmpTopology, first, count, instancecount, baseinstance);
    checkBackendError("OpenGLCommandQueue::execute: cmdDraw");
}

void OpenGLCommandQueue::execute(const OpenGLCommandDrawIndexed& cmd) {
    const GLsizei count = cmd.indexCount;
    const GLsizei instancecount = cmd.instanceCount;
    const GLint basevertex = cmd.vertexOffset;
    const GLuint baseinstance = cmd.firstInstance;

    // TODO: Set index buffer (modify it via cmd.firstIndex)
    glDrawElementsInstancedBaseVertexBaseInstance(tmpTopology, count, 0, nullptr, instancecount, basevertex, baseinstance);
    checkBackendError("OpenGLCommandQueue::execute: cmdDrawIndexed");
}

void OpenGLCommandQueue::execute(const OpenGLCommandSetVertexBuffers& cmd) {
    GLint index = cmd.index;
    GLsizei count = cmd.count;
    const auto& buffers = cmd.buffers;
    const auto& offsets = cmd.offsets;
    const auto& strides = cmd.strides;

    if (buffers.size() == 0) {
        glBindVertexBuffers(index, count, nullptr, nullptr, nullptr);
    } else {
        glBindVertexBuffers(index, count, buffers.data(), offsets.data(), strides.data());
    }
    checkBackendError("OpenGLCommandQueue::execute: cmdSetVertexBuffers");
}

void OpenGLCommandQueue::execute(const OpenGLCommandSetPrimitiveTopology& cmd) {
    tmpTopology = cmd.topology;
    checkBackendError("OpenGLCommandQueue::execute: cmdSetPrimitiveTopology");
}

void OpenGLCommandQueue::execute(const OpenGLCommandSetTargets& cmd) {
    for (U32 index = 0; index < cmd.colorCount; index++) {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, cmd.colorTargets[index], 0);
    }
    if (cmd.depthStencilTarget) {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cmd.depthStencilTarget, 0);
    }

    checkBackendError("OpenGLCommandQueue::execute: cmdSetTargets");
}

void OpenGLCommandQueue::execute(const OpenGLCommandSetViewports& cmd) {
    glViewport(cmd.x, cmd.y, cmd.width, cmd.height);
    checkBackendError("OpenGLCommandQueue::execute: cmdSetViewports");
}

void OpenGLCommandQueue::execute(const OpenGLCommandSetScissors& cmd) {
    glScissor(cmd.x, cmd.y, cmd.width, cmd.height);
    checkBackendError("OpenGLCommandQueue::execute: cmdSetScissors");
}

void OpenGLCommandQueue::execute(const OpenGLCommandInternalSignalFence& cmd) {
    glFinish();
    cmd.fence->signal();
    checkBackendError("OpenGLCommandQueue::execute: cmdInternalSignalFence");
}

void OpenGLCommandQueue::submit(CommandBuffer* cmdBuffer, Fence* fence) {
    auto glCmdBuffer = static_cast<OpenGLCommandBuffer*>(cmdBuffer);
    auto* glFence = static_cast<OpenGLFence*>(fence);

    // Append fence signaling command if requested
    bool destroyOnCompletion = true;
    if (fence) {
        destroyOnCompletion = false;
        auto* cmd = new OpenGLCommandInternalSignalFence();
        cmd->fence = glFence;
        glFence->clear();
        glCmdBuffer->commands.push_back(cmd);
    }

    OpenGLCommandQueueUnit unit;
    unit.cmdBuffer = glCmdBuffer;
    unit.destroyOnCompletion = destroyOnCompletion;

    std::lock_guard<std::mutex> lock(mutex);
    work.push(unit);

    cv.notify_all();
}

void OpenGLCommandQueue::waitIdle() {
}

}  // namespace opengl
}  // namespace gfx
