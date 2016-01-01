/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/command_buffer.h"
#include "nucleus/graphics/fence.h"

namespace gfx {

class CommandQueue {
public:
    /**
     * Submits a command buffer to this queue for execution
     * @param[in]  cmdBuffer  Command buffer to be submitted
     * @param[in]  fence      Fence to be signaled (Optional)
     */
    virtual void submit(CommandBuffer* cmdBuffer, Fence* fence) = 0;

    /**
     * Makes the calling thread wait for the execution of all submitted command buffers
     */
    virtual void waitIdle() = 0;
};

}  // namespace gfx
