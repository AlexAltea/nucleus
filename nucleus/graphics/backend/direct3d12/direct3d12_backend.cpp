/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_backend.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"

#include "nucleus/graphics/backend/direct3d12/direct3d12_command_buffer.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_command_queue.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_heap.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_target.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_texture.h"

namespace gfx {

Direct3D12Backend::Direct3D12Backend() : IBackend() {
}

Direct3D12Backend::~Direct3D12Backend() {
}

bool Direct3D12Backend::initialize(const BackendParameters& params) {
    if (!initializeDirect3D12()) {
        logger.warning(LOG_GRAPHICS, "Direct3D12Backend::initialize: Could load Direct3D12 dynamic library");
        return false;
    }

    _D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

    IDXGIFactory4* factory;
    _CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    // Create swap chain
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.Width = params.width;
    swapChainDesc.BufferDesc.Height = params.height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.OutputWindow = params.hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;

    if (FAILED(factory->CreateSwapChain(device, &swapChainDesc, &swapChain))) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::initialize: factory->CreateSwapChain failed");
        return false;
    }

    // Get render target buffers from swap chain
    swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainRenderBuffer[0]));
    swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainRenderBuffer[1]));

    parameters = params;
    return true;
}

ICommandQueue* Direct3D12Backend::createCommandQueue() {
    auto* commandQueue = new Direct3D12CommandQueue();

    if (!commandQueue->initialize(device)) {
        logger.error(LOG_GRAPHICS, "OpenGLBackend::createCommandQueue: Could not initialize OpenGLCommandQueue");
        return nullptr;
    }
    return commandQueue;
}

ICommandBuffer* Direct3D12Backend::createCommandBuffer() {
    auto* commandBuffer = new Direct3D12CommandBuffer();
    return commandBuffer;
}

IHeap* Direct3D12Backend::createHeap(const HeapDesc& desc) {
    auto* heap = new Direct3D12Heap();

    D3D12_DESCRIPTOR_HEAP_DESC d3dDesc = {};
    d3dDesc.NumDescriptors = desc.size;
    d3dDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    switch (desc.type) {
    case HEAP_TYPE_CT:
        d3dDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; break;
    case HEAP_TYPE_DST:
        d3dDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; break;
    default:
        logger.error(LOG_GRAPHICS, "Unimplemented descriptor heap type");
    }

    if (FAILED(device->CreateDescriptorHeap(&d3dDesc, IID_PPV_ARGS(&heap->heap)))) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::createHeap: device->CreateDescriptorHeap failed");
        return nullptr;
    }

    return heap;
}

void Direct3D12Backend::createPipeline() {
}

void Direct3D12Backend::createShader() {
}

ITexture* Direct3D12Backend::createTexture(const TextureDesc& desc) {
    auto* texture = new Direct3D12Texture(); 

    // Create resource description
    D3D12_RESOURCE_DESC d3dDesc = {};
    d3dDesc.Alignment = desc.alignment;
    d3dDesc.Height = desc.height;
    d3dDesc.Width = desc.width;
    d3dDesc.MipLevels = desc.mipmapLevels;

    // Pick appropriate format
    switch (desc.format) {
    case TEXTURE_FORMAT_R8G8B8A8:
        d3dDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT; break;
    default:
        logger.error(LOG_GRAPHICS, "Unimplemented texture format");
    }

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask = 1;
    heapProps.VisibleNodeMask = 1;

    if (FAILED(device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &d3dDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&texture->resource)))) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::createTexture: device->CreateCommittedResource failed");
        return nullptr;
    }

    return texture;
}

}  // namespace gfx
