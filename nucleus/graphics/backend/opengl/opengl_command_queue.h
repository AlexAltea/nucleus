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

class OpenGLCommandQueue : public ICommandQueue {
private:
    // Holds a queue of command buffers to execute
    std::queue<OpenGLCommandBuffer*> commandBuffers;

    // Command execution thread
    std::thread thread;
    std::mutex mutex;
    std::condition_variable cv;

    // Specific commands
    void executeClearColor(const OpenGLCommandData& data);
    void executeClearDepthStencil(const OpenGLCommandData& data);

    // Execute a command
    void execute(const OpenGLCommand& cmd);

public:
    OpenGLCommandQueue();
    ~OpenGLCommandQueue();

    bool initialize(DisplayHandler display, OpenGLContext context);

    virtual void submit(ICommandBuffer* cmdBuffer) override;
    virtual void waitIdle() override;
};

}  // namespace gfx
