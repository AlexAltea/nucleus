/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_debug.h"

// Forward Direct3D12 declarations
struct ID3D12CommandQueue;
struct ID3D12Device;
struct ID3D12DescriptorHeap;
struct ID3D12Resource;
struct IDXGIAdapter1;
struct IDXGISwapChain3;

namespace gfx {
namespace direct3d12 {

class Direct3D12Backend : public IBackend {
private:
    // Debugging
    Direct3D12Debug debug;

    IDXGIAdapter1* adapter;
    ID3D12Device* device;
    ID3D12CommandQueue* queue;

    // Swap chain
    IDXGISwapChain3* swapChain;
    ID3D12Resource* swapChainRenderBuffer[2];
    ID3D12DescriptorHeap* swapChainRTVHeap;
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
    virtual bool doResizeBuffers(int width, int height) override;
    virtual bool doSwapBuffers() override;
};

}  // namespace direct3d12
}  // namespace gfx
