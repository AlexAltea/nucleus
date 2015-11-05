/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include "nucleus/graphics/command_buffer.h"
#include "nucleus/graphics/command_queue.h"
#include "nucleus/graphics/heap.h"

#if defined(NUCLEUS_PLATFORM_WINDOWS)
#include <Windows.h>
#endif

namespace gfx {

// Graphics backend parameters
struct BackendParameters {
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    HWND hwnd;
    HDC hdc;
    size_t width;
    size_t height;
#elif defined(NUCLEUS_PLATFORM_LINUX)
    Display* display;
#endif
};

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
protected:
    BackendParameters parameters;

public:
    // Constructor
    IBackend();

    virtual bool initialize(const BackendParameters& params) = 0;

    /**
     * Creates a new command queue
     * @return Command queue
     */
    virtual ICommandQueue* createCommandQueue() = 0;

    /**
     * Creates a new command buffer
     * @return Command buffer
     */
    virtual ICommandBuffer* createCommandBuffer() = 0;

    /**
     * Creates a new descriptor heap
     * @param[in]  desc  Describes the descriptor heap
     * @return           Descriptor heap
     */
    virtual IHeap* createHeap(const HeapDesc& desc) = 0;

    // Pipeline management
    virtual void createPipeline() = 0;

    // Shader management
    virtual void createShader() = 0;
};

}  // namespace gfx
