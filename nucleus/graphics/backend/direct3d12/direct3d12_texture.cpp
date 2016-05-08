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

Direct3D12Texture::Direct3D12Texture(ID3D12Resource* resource) :
    Direct3D12Resource(resource) {}

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
    resourceDesc.Flags = convertTextureFlags(desc.flags);

    D3D12_CLEAR_VALUE clearValue = {};
    D3D12_CLEAR_VALUE* clearValuePtr = nullptr;
    D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON;
    if (desc.data) {
        resourceState = D3D12_RESOURCE_STATE_COPY_DEST;
    } else {
        clearValuePtr = &clearValue;
        clearValue.Format = resourceDesc.Format;
        if (desc.flags & TEXTURE_FLAG_COLOR_TARGET) {
            resourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;
        }
        if (desc.flags & TEXTURE_FLAG_DEPTHSTENCIL_TARGET) {
            resourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
        }
    }

    HRESULT hr;
    hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, resourceState, clearValuePtr, IID_PPV_ARGS(&resource));
    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Texture::Direct3D12Texture: Could not create resource (0x%X)", hr);
    }

    // Create SRV description heap for non-depth-stencil textures
    if (!(desc.flags & TEXTURE_FLAG_DEPTHSTENCIL_TARGET)) {
        srvDesc.Shader4ComponentMapping = desc.swizzle;
        srvDesc.Format = resourceDesc.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.PlaneSlice = 0;
        srvDesc.Texture2D.ResourceMinLODClamp = 0;
    }
}

Direct3D12Texture::~Direct3D12Texture() {
    resource->Release();
}

bool Direct3D12Texture::upload(ID3D12Device* device, ID3D12CommandQueue* queue, const TextureDesc& desc) {
    const D3D12_RESOURCE_DESC& resourceDesc = resource->GetDesc();

    D3D12_SUBRESOURCE_DATA srcData;
    srcData.pData = desc.data;
    srcData.RowPitch = desc.width * formatInfo[desc.format].bytesPerPixel;
    srcData.SlicePitch = srcData.RowPitch * desc.height;

    // Get resource footprints
    UINT64 resourceSize;
    UINT resourceNumRows;
    UINT64 resourceRowSizeInBytes;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT resourceLayout;
    device->GetCopyableFootprints(&resourceDesc, 0, 1, 0, &resourceLayout, &resourceNumRows, &resourceRowSizeInBytes, &resourceSize);

    D3D12_HEAP_PROPERTIES uploadHeapProps = {};
    uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    uploadHeapProps.CreationNodeMask = 1;
    uploadHeapProps.VisibleNodeMask = 1;
    uploadHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    uploadHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC uploadResDesc = {};
    uploadResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    uploadResDesc.Alignment = 0;
    uploadResDesc.Width = resourceSize;
    uploadResDesc.Height = 1;
    uploadResDesc.DepthOrArraySize = 1;
    uploadResDesc.MipLevels = 1;
    uploadResDesc.Format = DXGI_FORMAT_UNKNOWN;
    uploadResDesc.SampleDesc.Count = 1;
    uploadResDesc.SampleDesc.Quality = 0;
    uploadResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    uploadResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    HRESULT hr;
    ID3D12Resource* textureUploadRes;
    hr = device->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &uploadResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&textureUploadRes));
    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Texture::upload: Could not created intermediate upload resource");
        return false;
    }

    Byte* intermediateData;
    hr = textureUploadRes->Map(0, nullptr, reinterpret_cast<void**>(&intermediateData));
    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Texture::upload: Could not map intermediate upload resource");
        return false;
    }
    D3D12_MEMCPY_DEST dstData;
    dstData.pData = intermediateData;
    dstData.RowPitch = resourceLayout.Footprint.RowPitch;
    dstData.SlicePitch = resourceLayout.Footprint.RowPitch * resourceNumRows;
    for (UINT z = 0; z < resourceLayout.Footprint.Depth; z++) {
        BYTE* destSlice = reinterpret_cast<BYTE*>(dstData.pData) + dstData.SlicePitch * z;
        const BYTE* srcSlice = reinterpret_cast<const BYTE*>(srcData.pData) + srcData.SlicePitch * z;
        for (UINT y = 0; y < resourceNumRows; y++) {
            memcpy(destSlice + dstData.RowPitch * y, srcSlice + srcData.RowPitch * y, resourceRowSizeInBytes);
        }
    }
    textureUploadRes->Unmap(0, nullptr);

    ID3D12Fence* fence;
    ID3D12GraphicsCommandList* commandList;
    ID3D12CommandAllocator* commandAllocator;
    assert_true(SUCCEEDED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator))));
    assert_true(SUCCEEDED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, NULL, IID_PPV_ARGS(&commandList))));
    assert_true(SUCCEEDED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))));

    D3D12_TEXTURE_COPY_LOCATION textureSrc = {};
    D3D12_TEXTURE_COPY_LOCATION textureDst = {};
    textureSrc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    textureSrc.pResource = textureUploadRes;
    textureSrc.PlacedFootprint = resourceLayout;
    textureDst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    textureDst.pResource = resource;
    textureDst.SubresourceIndex = 0;
    commandList->CopyTextureRegion(&textureDst, 0, 0, 0, &textureSrc, nullptr);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = resource;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);

    commandList->Close();
    queue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList**>(&commandList));
    queue->Signal(fence, 1);

    // Wait until completion to delete intermediates
    HANDLE fenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
    assert_true(fenceEvent != 0);
    fence->SetEventOnCompletion(1, fenceEvent);
    WaitForSingleObject(fenceEvent, INFINITE);

    fence->Release();
    commandList->Release();
    commandAllocator->Release();
    textureUploadRes->Release();
    return true;
}

}  // namespace direct3d12
}  // namespace gfx
