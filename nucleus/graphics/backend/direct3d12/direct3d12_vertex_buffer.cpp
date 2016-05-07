/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "nucleus/graphics/backend/direct3d12/direct3d12_vertex_buffer.h"
#include "nucleus/logger/logger.h"

namespace gfx {
namespace direct3d12 {

Direct3D12VertexBuffer::Direct3D12VertexBuffer(ID3D12Device* device, UINT64 size) {
    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = size;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12VertexBuffer::Direct3D12VertexBuffer: Could not create resource");
    }

    cbvDesc.BufferLocation = resource->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = size;
}

Direct3D12VertexBuffer::~Direct3D12VertexBuffer() {
    resource->Release();
}

}  // namespace direct3d12
}  // namespace gfx
