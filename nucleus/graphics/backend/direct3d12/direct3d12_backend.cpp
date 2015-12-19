/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_backend.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"

#include "nucleus/graphics/backend/direct3d12/direct3d12_command_buffer.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_command_queue.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_fence.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_heap.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_pipeline.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_shader.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_target.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_texture.h"

#include <vector>

namespace gfx {
namespace direct3d12 {

Direct3D12Backend::Direct3D12Backend() : IBackend() {
}

Direct3D12Backend::~Direct3D12Backend() {
}

bool Direct3D12Backend::initialize(const BackendParameters& params) {
    HRESULT hr;
    parameters = params;

    if (!initializeDirect3D12()) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::initialize: Could not load Direct3D12 dynamic library");
        return false;
    }

    IDXGIFactory4* factory;
    hr = _CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::initialize: CreateDXGIFactory1 failed (0x%X)", hr);
        return false;
    }

#if defined(NUCLEUS_BUILD_DEBUG)
    hr = _D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
    if (SUCCEEDED(hr)) {
        debugController->EnableDebugLayer();
    }
#endif

    hr = _D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
    if (FAILED(hr)) {
        logger.warning(LOG_GRAPHICS, "Direct3D12Backend::initialize: Creating WARP device");
        factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
        hr = _D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
        if (FAILED(hr)) {
            logger.error(LOG_GRAPHICS, "Direct3D12Backend::initialize: D3D12CreateDevice failed (0x%X)", hr);
            return false;
        }
    }

    // Create main command queue
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&queue));
    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::initialize: Could not create main command queue (0x%X)", hr);
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

    IDXGISwapChain1* tempSwapChain;
    if (params.window) {
        hr = factory->CreateSwapChainForCoreWindow(queue, params.window, &swapChainDesc, nullptr, &tempSwapChain);
    } else {
        hr = factory->CreateSwapChainForHwnd(device, params.hwnd, &swapChainDesc,  &swapChainFullscreenDesc, nullptr, &tempSwapChain);
    }

    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::initialize: Could not create swap chain (0x%X)", hr);
        return false;
    }
    hr = tempSwapChain->QueryInterface(IID_PPV_ARGS(&swapChain));
    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::initialize: Could not request a IDXGISwapChain3 swap chain (0x%X)", hr);
        return false;
    }

    // Get render target buffers from swap chain
    D3D12_DESCRIPTOR_HEAP_DESC swapChainRTVHeapDesc = {};
    swapChainRTVHeapDesc.NumDescriptors = 2;
    swapChainRTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    swapChainRTVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    device->CreateDescriptorHeap(&swapChainRTVHeapDesc, IID_PPV_ARGS(&swapChainRTVHeap));

    Direct3D12ColorTarget swapChainColorTargets[2];
    D3D12_CPU_DESCRIPTOR_HANDLE swapChainRTVHeapStart = swapChainRTVHeap->GetCPUDescriptorHandleForHeapStart();
    unsigned int rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    for (UINT i = 0; i < swapChainRTVHeapDesc.NumDescriptors; i++) {
        swapChainColorTargets[i].handle.ptr = swapChainRTVHeapStart.ptr + (i * rtvDescriptorSize);
        swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainRenderBuffer[i]));
        device->CreateRenderTargetView(swapChainRenderBuffer[0], NULL, swapChainColorTargets[i].handle);
    }

    if (swapChain->GetCurrentBackBufferIndex() == 0) {
        screenBackBuffer = new Direct3D12ColorTarget(swapChainColorTargets[0]);
        screenFrontBuffer = new Direct3D12ColorTarget(swapChainColorTargets[1]);
    } else {
        screenBackBuffer = new Direct3D12ColorTarget(swapChainColorTargets[1]);
        screenFrontBuffer = new Direct3D12ColorTarget(swapChainColorTargets[0]);
    }

    return true;
}

CommandQueue* Direct3D12Backend::createCommandQueue() {
    auto* commandQueue = new Direct3D12CommandQueue();

    if (!commandQueue->initialize(queue)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::createCommandQueue: Could not initialize Direct3D12CommandQueue");
        return nullptr;
    }
    return commandQueue;
}

CommandBuffer* Direct3D12Backend::createCommandBuffer() {
    auto* commandBuffer = new Direct3D12CommandBuffer();

    if (!commandBuffer->initialize(device)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::createCommandBuffer: Could not initialize Direct3D12CommandBuffer");
        return nullptr;
    }
    return commandBuffer;
}

Fence* Direct3D12Backend::createFence(const FenceDesc& desc) {
    auto* fence = new Direct3D12Fence();

    if (!fence->initialize(device)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::createCommandQueue: Could not initialize Direct3D12Fence");
        return nullptr;
    }
    return fence;
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

    HRESULT hr = device->CreateDescriptorHeap(&d3dDesc, IID_PPV_ARGS(&heap->heap));
    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::createHeap: device->CreateDescriptorHeap failed (0x%X)", hr);
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
    auto* pipeline = new Direct3D12Pipeline();

    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dDesc = {};
    d3dDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    d3dDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

    HRESULT hr = device->CreateGraphicsPipelineState(&d3dDesc, IID_PPV_ARGS(&pipeline->state));
    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::createPipeline: CreateGraphicsPipelineState failed (0x%X)", hr);
        return nullptr;
    }
    return pipeline;
}

Shader* Direct3D12Backend::createShader(const ShaderDesc& desc) {
    auto* shader = new Direct3D12Shader();
    shader->initialize(desc);
    return shader;
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

    HRESULT hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &d3dDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&texture->resource));
    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::createTexture: device->CreateCommittedResource failed (0x%X)", hr);
        return nullptr;
    }
    return texture;
}

VertexBuffer* Direct3D12Backend::createVertexBuffer(const VertexBufferDesc& desc) {
    return nullptr;
}

bool Direct3D12Backend::doSwapBuffers() {
    if (FAILED(swapChain->Present(0, 0))) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::doSwapBuffers: swapChain->Present failed");
        return false;
    }
    std::swap(screenBackBuffer, screenFrontBuffer);
    return true;
}

}  // namespace direct3d12
}  // namespace gfx
