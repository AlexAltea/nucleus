/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/command_buffer.h"

namespace graphics {

class ICommandQueue {
public:
    /**
     * Submits a command buffer to a queue for execution
     * @param[in]  cmdBuffer  Command buffer to be submitted
     */
    virtual void submit(ICommandBuffer* cmdBuffer) = 0;

    /**
     * Makes the calling thread wait for the execution of all submitted command buffers
     */
    virtual void waitIdle() = 0;
};

}  // namespace graphics
