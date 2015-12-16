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

#include <vector>

namespace gfx {

Direct3D12Backend::Direct3D12Backend() : IBackend() {
}

Direct3D12Backend::~Direct3D12Backend() {
}

bool Direct3D12Backend::initialize(const BackendParameters& params) {
    HRESULT hr;
    if (!initializeDirect3D12()) {
        logger.warning(LOG_GRAPHICS, "Direct3D12Backend::initialize: Could not load Direct3D12 dynamic library");
        return false;
    }

    IDXGIFactory4* factory;
    hr = _CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(hr)) {
        logger.warning(LOG_GRAPHICS, "Direct3D12Backend::initialize: CreateDXGIFactory1 failed (0x%X)", hr);
        return false;
    }

    hr = _D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
    if (FAILED(hr)) {
        logger.warning(LOG_GRAPHICS, "Direct3D12Backend::initialize: D3D12CreateDevice failed (0x%X)", hr);
        return false;
    }

    // Create swap chain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = params.width;
    swapChainDesc.Height = params.height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = false;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Scaling = DXGI_SCALING_NONE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    swapChainDesc.Flags = 0;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreenDesc = {};
    swapChainFullscreenDesc.RefreshRate.Numerator = 60;
    swapChainFullscreenDesc.RefreshRate.Denominator = 1;
    swapChainFullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainFullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainFullscreenDesc.Windowed = TRUE;

    if (parameters.window) {
        hr = factory->CreateSwapChainForCoreWindow(device, params.window, &swapChainDesc, nullptr, &swapChain);
    } else {
        hr = factory->CreateSwapChainForHwnd(device, parameters.hwnd, &swapChainDesc,  &swapChainFullscreenDesc, nullptr, &swapChain);
    }

    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::initialize: Could not create swap chain (0x%X)", hr);
        return false;
    }

    // Get render target buffers from swap chain
    swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainRenderBuffer[0]));
    swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainRenderBuffer[1]));

    parameters = params;
    return true;
}

CommandQueue* Direct3D12Backend::createCommandQueue() {
    auto* commandQueue = new Direct3D12CommandQueue();

    if (!commandQueue->initialize(device)) {
        logger.error(LOG_GRAPHICS, "OpenGLBackend::createCommandQueue: Could not initialize OpenGLCommandQueue");
        return nullptr;
    }
    return commandQueue;
}

CommandBuffer* Direct3D12Backend::createCommandBuffer() {
    auto* commandBuffer = new Direct3D12CommandBuffer();
    return commandBuffer;
}

Fence* Direct3D12Backend::createFence(const FenceDesc& desc) {
    return nullptr;
}

Heap* Direct3D12Backend::createHeap(const HeapDesc& desc) {
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

ColorTarget* Direct3D12Backend::createColorTarget(Texture* texture) {
    auto d3dTexture = static_cast<Direct3D12Texture*>(texture);
    if (!d3dTexture) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::createColorTarget: Invalid texture specified");
        return nullptr;
    }

    auto* target = new Direct3D12ColorTarget();
    device->CreateRenderTargetView(d3dTexture->resource, NULL, target->handle);
    return target;
}

DepthStencilTarget* Direct3D12Backend::createDepthStencilTarget(Texture* texture) {
    return nullptr;
}

Pipeline* Direct3D12Backend::createPipeline(const PipelineDesc& desc) {
    return nullptr;
}

Shader* Direct3D12Backend::createShader(const ShaderDesc& desc) {
    return nullptr;
}

Texture* Direct3D12Backend::createTexture(const TextureDesc& desc) {
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

bool Direct3D12Backend::doSwapBuffers() {
    if (FAILED(swapChain->Present(0, 0))) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::doSwapBuffers: swapChain->Present failed");
        return false;
    }
    std::swap(screenBackBuffer, screenFrontBuffer);
    return true;
}

}  // namespace gfx
