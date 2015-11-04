/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_backend.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_command_buffer.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_command_queue.h"

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

    // Create render target view
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    ID3D12DescriptorHeap* rtvHeap;
    if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)))) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::initialize: device->CreateDescriptorHeap failed");
        return false;
    }

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
    ICommandBuffer* commandBuffer = new Direct3D12CommandBuffer();
    return commandBuffer;
}

void Direct3D12Backend::createPipeline() {
}

void Direct3D12Backend::createShader() {
}

}  // namespace gfx
