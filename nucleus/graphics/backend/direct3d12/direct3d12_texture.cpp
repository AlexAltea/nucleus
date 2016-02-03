/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_texture.h"
#include "nucleus/assert.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_convert.h"

#include <memory>

namespace gfx {
namespace direct3d12 {

Direct3D12Texture::Direct3D12Texture(ID3D12Device* device, const TextureDesc& desc) {
    // Create texture
    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Width = static_cast<UINT64>(desc.width);
    resourceDesc.Height = static_cast<UINT>(desc.height);
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = static_cast<UINT16>(desc.mipmapLevels);
    resourceDesc.Format = convertFormat(desc.format);
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    HRESULT hr;
    hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&resource));
    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Texture::Direct3D12Texture: Could not create resource (0x%X)", hr);
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = resourceDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    //device->CreateShaderResourceView(resource, &srvDesc, /*TODO*/);
}

Direct3D12Texture::~Direct3D12Texture() {
    resource->Release();
}

bool Direct3D12Texture::upload(ID3D12Device* device, ID3D12CommandQueue* queue, const Byte* data, Size size) {
    UINT64 requiredSize;
    const D3D12_RESOURCE_DESC& resourceDesc = resource->GetDesc();
    device->GetCopyableFootprints(&resourceDesc, 0, 1, 0, nullptr, nullptr, nullptr, &requiredSize);

    D3D12_HEAP_PROPERTIES uploadHeapProps = {};
    uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    uploadHeapProps.CreationNodeMask = 1;
    uploadHeapProps.VisibleNodeMask = 1;
    uploadHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    uploadHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC uploadRescDesc = {};
    uploadRescDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    uploadRescDesc.Alignment = 0;
    uploadRescDesc.Width = requiredSize;
    uploadRescDesc.Height = 1;
    uploadRescDesc.DepthOrArraySize = 1;
    uploadRescDesc.MipLevels = 1;
    uploadRescDesc.Format = DXGI_FORMAT_UNKNOWN;
    uploadRescDesc.SampleDesc.Count = 1;
    uploadRescDesc.SampleDesc.Quality = 0;
    uploadRescDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    uploadRescDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    HRESULT hr;
    ID3D12Resource* textureUploadResc;
    hr = device->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &uploadRescDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&textureUploadResc));
    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Texture::upload: Could not created intermediate upload resource");
        return false;
    }

    Byte* intermediateData;
    hr = textureUploadResc->Map(0, nullptr, reinterpret_cast<void**>(&intermediateData));
    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Texture::upload: Could not map intermediate upload resource");
        return false;
    }

    textureUploadResc->Unmap(0, nullptr);

    ID3D12GraphicsCommandList* commandList;
    ID3D12CommandAllocator* commandAllocator;
    assert_true(SUCCEEDED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator))));
    assert_true(SUCCEEDED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, NULL, IID_PPV_ARGS(&commandList))));

    D3D12_TEXTURE_COPY_LOCATION textureSrc = {};
    D3D12_TEXTURE_COPY_LOCATION textureDst = {};
    textureSrc.pResource = textureUploadResc;
    textureDst.pResource = resource;

    commandList->CopyTextureRegion(&textureDst, 0, 0, 0, &textureSrc, nullptr);
    commandList->Close();
    queue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList**>(&commandList));

    // Wait until completion to delete intermediates
    ID3D12Fence* fence;
    HANDLE fenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
    assert_true(SUCCEEDED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))));
    queue->Signal(fence, 1);
    fence->SetEventOnCompletion(1, fenceEvent);
    WaitForSingleObject(fenceEvent, INFINITE);

    delete fence;
    delete commandList;
    delete commandAllocator;
    delete textureUploadResc;
}

}  // namespace direct3d12
}  // namespace gfx
