/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d11_backend.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/backend/direct3d11/direct3d11.h"

#include "nucleus/graphics/backend/direct3d11/direct3d11_command_buffer.h"
#include "nucleus/graphics/backend/direct3d11/direct3d11_command_queue.h"
#include "nucleus/graphics/backend/direct3d11/direct3d11_heap.h"
#include "nucleus/graphics/backend/direct3d11/direct3d11_target.h"
#include "nucleus/graphics/backend/direct3d11/direct3d11_texture.h"

#include <vector>

namespace gfx {

Direct3D11Backend::Direct3D11Backend() : IBackend() {
}

Direct3D11Backend::~Direct3D11Backend() {
}

bool Direct3D11Backend::initialize(const BackendParameters& params) {
    if (!initializeDirect3D11()) {
        logger.warning(LOG_GRAPHICS, "Direct3D11Backend::initialize: Could load Direct3D11 dynamic library");
        return false;
    }

    IDXGIFactory4* factory;
    if (FAILED(_CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory)))) {
        logger.warning(LOG_GRAPHICS, "Direct3D11Backend::initialize: CreateDXGIFactory1 failed");
        return false;
    }

    UINT createDeviceFlags;
#ifdef NUCLEUS_BUILD_DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    
    D3D_FEATURE_LEVEL featureLevel;
    if (FAILED(_D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &device, &featureLevel, &deviceContext))) {
        logger.warning(LOG_GRAPHICS, "Direct3D11Backend::initialize: D3D11CreateDevice failed");
        return false;
    }

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
        logger.error(LOG_GRAPHICS, "Direct3D11Backend::initialize: factory->CreateSwapChain failed");
        return false;
    }

    // Get render target buffers from swap chain
    swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainRenderBuffer[0]));
    swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainRenderBuffer[1]));

    parameters = params;
    return true;
}

CommandQueue* Direct3D11Backend::createCommandQueue() {
    auto* commandQueue = new Direct3D11CommandQueue();

    if (!commandQueue->initialize(device)) {
        logger.error(LOG_GRAPHICS, "OpenGLBackend::createCommandQueue: Could not initialize OpenGLCommandQueue");
        return nullptr;
    }
    return commandQueue;
}

CommandBuffer* Direct3D11Backend::createCommandBuffer() {
    auto* commandBuffer = new Direct3D11CommandBuffer();
    return commandBuffer;
}

Fence* Direct3D11Backend::createFence(const FenceDesc& desc) {
    return nullptr;
}

Heap* Direct3D11Backend::createHeap(const HeapDesc& desc) {
    auto* heap = new Direct3D11Heap();
    return heap;
}

ColorTarget* Direct3D11Backend::createColorTarget(Texture* texture) {
    auto d3dTexture = static_cast<Direct3D11Texture*>(texture);
    if (!d3dTexture) {
        logger.error(LOG_GRAPHICS, "Direct3D11Backend::createColorTarget: Invalid texture specified");
        return nullptr;
    }

    auto* target = new Direct3D11ColorTarget();
    device->CreateRenderTargetView(d3dTexture->texture, NULL, &target->view);
    return target;
}

DepthStencilTarget* Direct3D11Backend::createDepthStencilTarget(Texture* texture) {
    return nullptr;
}

void Direct3D11Backend::createPipeline() {
}

void Direct3D11Backend::createShader() {
}

Texture* Direct3D11Backend::createTexture(const TextureDesc& desc) {
    auto* texture = new Direct3D11Texture();

    // Create resource description
    D3D11_TEXTURE2D_DESC d3dDesc = {};
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

    if (FAILED(device->CreateTexture2D(&d3dDesc, nullptr, &texture->texture))) {
        logger.error(LOG_GRAPHICS, "Direct3D11Backend::createTexture: device->CreateCommittedResource failed");
        return nullptr;
    }

    return texture;
}

bool Direct3D11Backend::doSwapBuffers() {
    if (FAILED(swapChain->Present(0, 0))) {
        logger.error(LOG_GRAPHICS, "Direct3D11Backend::doSwapBuffers: swapChain->Present failed");
        return false;
    }
    std::swap(screenBackBuffer, screenFrontBuffer);
    return true;
}

}  // namespace gfx
