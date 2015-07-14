/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/command_buffer.h"

namespace graphics {

enum PrimitiveTopology {
    TOPOLOGY_POINT_LIST,
    TOPOLOGY_LINE_LIST,
    TOPOLOGY_LINE_STRIP,
    TOPOLOGY_TRIANGLE_LIST,
    TOPOLOGY_TRIANGLE_STRIP,
    TOPOLOGY_QUAD_LIST,
    TOPOLOGY_QUAD_STRIP,
};

class IBackend {

    // Command queue management
    virtual void CreateCommandQueue() = 0;
    virtual void CommandQueueSubmit() = 0;

    /**
     * Create a new command buffer
     */
    virtual ICommandBuffer* CreateCommandBuffer() = 0;

    // Pipeline management
    virtual void CreatePipeline() = 0;

    // Shader management
    virtual void CreateShader() = 0;
};

}  // namespace graphics
