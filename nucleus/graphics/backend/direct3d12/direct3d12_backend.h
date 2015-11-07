/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"

// Forward Direct3D12 declarations
class ID3D12Device;
class ID3D12DescriptorHeap;
class ID3D12Resource;
class IDXGISwapChain;

namespace gfx {

class Direct3D12Backend : public IBackend {
    ID3D12Device* device;

    // Swap chain
    IDXGISwapChain* swapChain;
    ID3D12Resource* swapChainRenderBuffer[2];
    unsigned int swapChainIndex;

public:
    Direct3D12Backend();
    ~Direct3D12Backend();

    virtual bool initialize(const BackendParameters& params) override;

    ICommandQueue* createCommandQueue() override;
    ICommandBuffer* createCommandBuffer() override;
    IHeap* createHeap(const HeapDesc& desc) override;
    void createPipeline() override;
    void createShader() override;
    ITexture* createTexture(const TextureDesc& desc) override;
};

}  // namespace gfx
