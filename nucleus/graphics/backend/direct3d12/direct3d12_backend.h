/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"

// Forward Direct3D12 declarations
class ID3D12CommandQueue;
class ID3D12Debug;
class ID3D12Device;
class ID3D12DescriptorHeap;
class ID3D12Resource;
class IDXGIAdapter1;
class IDXGISwapChain1;

namespace gfx {

class Direct3D12Backend : public IBackend {
    IDXGIAdapter1* adapter;
    ID3D12Debug* debugController;
    ID3D12Device* device;
    ID3D12CommandQueue* queue;

    // Swap chain
    IDXGISwapChain1* swapChain;
    ID3D12Resource* swapChainRenderBuffer[2];
    unsigned int swapChainIndex;

public:
    Direct3D12Backend();
    ~Direct3D12Backend();

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
