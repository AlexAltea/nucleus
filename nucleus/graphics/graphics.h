/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/command_buffer.h"
#include "nucleus/graphics/command_queue.h"

// Backends
#include "nucleus/graphics/backend/opengl/opengl_backend.h"

namespace gfx {

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
public:
    // Command queue management
    virtual ICommandQueue* createCommandQueue() = 0;

    // Command buffer management
    virtual ICommandBuffer* createCommandBuffer() = 0;

    // Pipeline management
    virtual void createPipeline() = 0;

    // Shader management
    virtual void createShader() = 0;
};

}  // namespace gfx
