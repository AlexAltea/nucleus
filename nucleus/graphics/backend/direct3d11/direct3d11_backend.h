/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"

// Forward Direct3D11 declarations
class ID3D11Device;
class ID3D11DeviceContext;
class ID3D11DescriptorHeap;
class ID3D11Resource;
class IDXGIAdapter1;
class IDXGISwapChain;

namespace gfx {

class Direct3D11Backend : public IBackend {
    IDXGIAdapter1* adapter;
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;

    // Swap chain
    IDXGISwapChain* swapChain;
    ID3D11Resource* swapChainRenderBuffer[2];
    unsigned int swapChainIndex;

public:
    Direct3D11Backend();
    ~Direct3D11Backend();

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
