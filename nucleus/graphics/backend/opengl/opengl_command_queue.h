/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/command_queue.h"
#include "nucleus/graphics/backend/opengl/opengl_backend.h"
#include "nucleus/graphics/backend/opengl/opengl_command_buffer.h"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace gfx {

struct OpenGLCommandQueueUnit {
    // Command buffer to be executed
    OpenGLCommandBuffer* cmdBuffer;

    // Delete command buffer after execution
    bool destroyOnCompletion;
};

class OpenGLCommandQueue : public CommandQueue {
private:
    // Internal state
    GLuint scratchFramebuffer;

private:
    // Parent OpenGL backend
    OpenGLBackend* parent;

    // Holds a queue of work units to execute
    std::queue<OpenGLCommandQueueUnit> work;

    // Command execution thread
    std::thread thread;
    std::mutex mutex;
    std::condition_variable cv;

    // Specific commands
    void execute(const OpenGLCommandClearColor& cmd);
    void execute(const OpenGLCommandClearDepthStencil& cmd);
    void execute(const OpenGLCommandSetTargets& cmd);
    void execute(const OpenGLCommandSetViewports& cmd);
    void execute(const OpenGLCommandSetScissors& cmd);
    void execute(const OpenGLCommandInternalCreateTexture& cmd);
    void execute(const OpenGLCommandInternalCreateVertexBuffer& cmd);
    void execute(const OpenGLCommandInternalSignalFence& cmd);

    // Execute a command
    void execute(const OpenGLCommand& cmd);

    // Main loop
    void task(const BackendParameters& params, OpenGLContext context);

public:
    OpenGLCommandQueue();
    ~OpenGLCommandQueue();

    bool initialize(const BackendParameters& params, OpenGLContext context);

    virtual void submit(CommandBuffer* cmdBuffer, Fence* fence) override;
    virtual void waitIdle() override;
};

}  // namespace gfx
