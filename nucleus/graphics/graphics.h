/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include "nucleus/graphics/command_buffer.h"
#include "nucleus/graphics/command_queue.h"
#include "nucleus/graphics/fence.h"
#include "nucleus/graphics/heap.h"
#include "nucleus/graphics/target.h"
#include "nucleus/graphics/texture.h"
#include "nucleus/graphics/vertex_buffer.h"

#if defined(NUCLEUS_PLATFORM_UWP)
#define NOMINMAX
#include <Windows.h>
#elif defined(NUCLEUS_PLATFORM_WINDOWS)
#define NOMINMAX
#include <Windows.h>
#endif

namespace gfx {

// Graphics backend parameters
struct BackendParameters {
#if defined(NUCLEUS_PLATFORM_ANDROID)
    Display* display;
#elif defined(NUCLEUS_PLATFORM_IOS)
    Display* display;
#elif defined(NUCLEUS_PLATFORM_LINUX)
    Display* display;
#elif defined(NUCLEUS_PLATFORM_OSX)
    Display* display;
#elif defined(NUCLEUS_PLATFORM_UWP)
    IUnknown* window;
    size_t width;
    size_t height;
#elif defined(NUCLEUS_PLATFORM_WINDOWS)
    HWND hwnd;
    HDC hdc;
    size_t width;
    size_t height;
#endif
};

class IBackend {
protected:
    BackendParameters parameters;

public:
    Resource* screenBackBuffer;
    Resource* screenFrontBuffer;
    ColorTarget* screenBackTarget;
    ColorTarget* screenFrontTarget;

    // Constructor
    IBackend();

    virtual bool initialize(const BackendParameters& params) = 0;

    /**
     * Creates a new command queue
     * @return Command queue
     */
    virtual CommandQueue* createCommandQueue() = 0;

    /**
     * Creates a new command buffer
     * @return Command buffer
     */
    virtual CommandBuffer* createCommandBuffer() = 0;

    /**
     * Creates a new fence
     * @param[in]  desc  Describes the fence
     * @return           Fence
     */
    virtual Fence* createFence(const FenceDesc& desc) = 0;

    /**
     * Creates a new descriptor heap
     * @param[in]  desc  Describes the descriptor heap
     * @return           Descriptor heap
     */
    virtual Heap* createHeap(const HeapDesc& desc) = 0;

    /**
     * Creates a new color target
     * @param[in]  texture  Texture resource
     * @return              Color target CPU descriptor handle
     */
    virtual ColorTarget* createColorTarget(Texture* texture) = 0;

    /**
     * Creates a new depth-stencil target
     * @param[in]  texture  Texture resource
     * @return              Depth-stencil target CPU descriptor handle
     */
    virtual DepthStencilTarget* createDepthStencilTarget(Texture* texture) = 0;

    /**
     * Creates a new pipeline state
     * @param[in]  desc  Describes the pipeline state
     * @return           Pipeline state
     */
    virtual Pipeline* createPipeline(const PipelineDesc& desc) = 0;

    /**
     * Creates a new shader
     * @param[in]  desc  Describes the shader
     * @return           Shader
     */
    virtual Shader* createShader(const ShaderDesc& desc) = 0;

    /**
     * Creates a new texture
     * @param[in]  desc  Describes the texture
     * @return           Texture
     */
    virtual Texture* createTexture(const TextureDesc& desc) = 0;

    /**
     * Creates a new vertex buffer
     * @param[in]  desc  Describes the vertex buffer
     * @return           Vertex buffer
     */
    virtual VertexBuffer* createVertexBuffer(const VertexBufferDesc& desc) = 0;

    /**
     * Resizes both the back buffer and front buffer
     * @return  True on success
     */
    virtual bool doResizeBuffers(int width, int height) = 0;

    /**
     * Swaps the back buffer and front buffer
     * @return  True on success
     */
    virtual bool doSwapBuffers() = 0;
};

}  // namespace gfx
