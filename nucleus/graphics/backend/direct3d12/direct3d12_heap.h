/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/heap.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"

// Forward declarations
class Direct3D12Texture;
class Direct3D12VertexBuffer;

namespace gfx {
namespace direct3d12 {

class Direct3D12Heap : public Heap {
    ID3D12Device* device;

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
    Size handleIncrement;

public:
    ID3D12DescriptorHeap* heap;

    Direct3D12Heap(ID3D12Device* device);
    ~Direct3D12Heap();

    bool initialize(const HeapDesc& desc);

    D3D12_GPU_DESCRIPTOR_HANDLE getGpuHandle(Size offset) const;

    virtual void reset() override;
    virtual void pushTexture(Texture* texture) override;
    virtual void pushVertexBuffer(VertexBuffer* buffer) override;
};

}  // namespace direct3d12
}  // namespace gfx
