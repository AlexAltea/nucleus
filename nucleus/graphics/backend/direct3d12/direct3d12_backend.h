/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"

// Forward Direct3D12 declarations
class ID3D12Device;

namespace gfx {

class Direct3D12Backend : public IBackend {
    ID3D12Device* device;

    IDXGISwapChain* swapChain;
    ID3D12DescriptorHeap* renderTargetViewHeap;
    ID3D12Resource* backBufferRenderTarget[2];
    unsigned int bufferIndex;

public:
    Direct3D12Backend();
    ~Direct3D12Backend();

    virtual bool initialize(const BackendParameters& params) override;

    ICommandQueue* createCommandQueue() override;
    ICommandBuffer* createCommandBuffer() override;
    IHeap* createHeap(const HeapDesc& desc) override;
    void createPipeline() override;
    void createShader() override;
};

}  // namespace gfx
