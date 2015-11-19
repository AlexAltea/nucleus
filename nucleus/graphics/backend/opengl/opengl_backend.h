/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"

namespace gfx {

// OpenGL context handler
#if defined(NUCLEUS_PLATFORM_WINDOWS)
using OpenGLContext = HGLRC;
#elif defined(NUCLEUS_PLATFORM_LINUX)
using OpenGLContext = GLXContext;
#elif defined(NUCLEUS_PLATFORM_OSX)
using OpenGLContext = GLXContext;
#elif defined(NUCLEUS_PLATFORM_ANDROID)
using OpenGLContext = EGLContext;
#elif defined(NUCLEUS_PLATFORM_IOS)
using OpenGLContext = EGLContext;
#endif

// Forward declarations
class OpenGLCommandQueue;

class OpenGLBackend : public IBackend {
private:
    OpenGLContext context = 0;
    OpenGLContext subContext[8];
    OpenGLCommandQueue* queue;

    /**
     * Picks an available sub-context and makes it current
     */
    void useAvailableContext();

    /**
     * Creates a context for the calling thread sharing lists with the main context
     * @return  OpenGL context
     */
    OpenGLContext createContext();

public:
    OpenGLBackend();
    ~OpenGLBackend();

    virtual bool initialize(const BackendParameters& params) override;

    virtual CommandQueue* createCommandQueue() override;
    virtual CommandBuffer* createCommandBuffer() override;
    virtual Fence* createFence(const FenceDesc& desc) override;
    virtual Heap* createHeap(const HeapDesc& desc) override;
    virtual ColorTarget* createColorTarget(Texture* texture) override;
    virtual DepthStencilTarget* createDepthStencilTarget(Texture* texture) override;
    virtual Pipeline* createPipeline(const PipelineDesc& desc) override;
    virtual Shader* createShader(const ShaderDesc& desc) override;
    virtual Texture* createTexture(const TextureDesc& desc) override;
    virtual VertexBuffer* createVertexBuffer(const VertexBufferDesc& desc) override;
    virtual bool doSwapBuffers() override;
};

}  // namespace gfx
