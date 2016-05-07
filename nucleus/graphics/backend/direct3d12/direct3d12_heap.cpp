/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_heap.h"
#include "nucleus/assert.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_texture.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_vertex_buffer.h"

namespace gfx {
namespace direct3d12 {

Direct3D12Heap::Direct3D12Heap(ID3D12Device* device) : device(device) {
}

Direct3D12Heap::~Direct3D12Heap() {
}

bool Direct3D12Heap::initialize(const HeapDesc& desc) {
    D3D12_DESCRIPTOR_HEAP_TYPE d3dType;
    switch (desc.type) {
    case HEAP_TYPE_CT:
        d3dType = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; break;
    case HEAP_TYPE_DST:
        d3dType = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; break;
    case HEAP_TYPE_RESOURCE:
        d3dType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; break;
    case HEAP_TYPE_SAMPLER:
        d3dType = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER; break;
    default:
        assert_true("Unimplemented descriptor heap type");
    }

    D3D12_DESCRIPTOR_HEAP_DESC d3dDesc = {};
    d3dDesc.NumDescriptors = desc.size;
    d3dDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    d3dDesc.Type = d3dType;

    HRESULT hr = device->CreateDescriptorHeap(&d3dDesc, IID_PPV_ARGS(&heap));
    if (FAILED(hr)) {
        return false;
    }

    cpuHandle = heap->GetCPUDescriptorHandleForHeapStart();
    cpuHandleIncrement = device->GetDescriptorHandleIncrementSize(d3dType);
    return true;
}

void Direct3D12Heap::reset() {
    cpuHandle = heap->GetCPUDescriptorHandleForHeapStart();
}

void Direct3D12Heap::pushTexture(Texture* texture) {
    auto* d3dTexture = static_cast<Direct3D12Texture*>(texture);
    auto* d3dResource = d3dTexture->resource;

    device->CreateShaderResourceView(d3dResource, &d3dTexture->srvDesc, cpuHandle);
    cpuHandle.ptr += cpuHandleIncrement;
}

void Direct3D12Heap::pushVertexBuffer(VertexBuffer* buffer) {
    auto* d3dVertexBuffer = static_cast<Direct3D12VertexBuffer*>(buffer);
    auto* d3dResource = d3dVertexBuffer->resource;

    device->CreateConstantBufferView(&d3dVertexBuffer->cbvDesc, cpuHandle);
    cpuHandle.ptr += cpuHandleIncrement;
}

}  // namespace direct3d12
}  // namespace gfx
