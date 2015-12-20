/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_backend.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_convert.h"

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
    HRESULT hr;
    auto* pipeline = new Direct3D12Pipeline();

    // Root signature parameters
    D3D12_DESCRIPTOR_RANGE range;
    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    range.NumDescriptors = 1;
    range.BaseShaderRegister = 0;
    range.RegisterSpace = 0;
    range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_PARAMETER parameter = {};
    parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    parameter.DescriptorTable.NumDescriptorRanges = 1;
    parameter.DescriptorTable.pDescriptorRanges = &range;
    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    // Root signature
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.NumParameters = 1;
    rootSignatureDesc.pParameters = &parameter;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    ID3DBlob* signature;
    ID3DBlob* error;
    ID3D12RootSignature* rootSignature;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);

    hr = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    if (FAILED(hr)) {
        logger.error(LOG_GRAPHICS, "Direct3D12Backend::createPipeline: CreateRootSignature failed (0x%X)", hr);
        return nullptr;
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dDesc = {};
    d3dDesc.pRootSignature = rootSignature;

    // Shaders
    if (desc.vs) {
        auto* d3dShader = static_cast<Direct3D12Shader*>(desc.vs);
        d3dDesc.VS.pShaderBytecode = d3dShader->bytecodeData;
        d3dDesc.VS.BytecodeLength = d3dShader->bytecodeSize;
    }
    if (desc.hs) {
        auto* d3dShader = static_cast<Direct3D12Shader*>(desc.hs);
        d3dDesc.HS.pShaderBytecode = d3dShader->bytecodeData;
        d3dDesc.HS.BytecodeLength = d3dShader->bytecodeSize;
    }
    if (desc.ds) {
        auto* d3dShader = static_cast<Direct3D12Shader*>(desc.ds);
        d3dDesc.DS.pShaderBytecode = d3dShader->bytecodeData;
        d3dDesc.DS.BytecodeLength = d3dShader->bytecodeSize;
    }
    if (desc.gs) {
        auto* d3dShader = static_cast<Direct3D12Shader*>(desc.gs);
        d3dDesc.GS.pShaderBytecode = d3dShader->bytecodeData;
        d3dDesc.GS.BytecodeLength = d3dShader->bytecodeSize;
    }
    if (desc.ps) {
        auto* d3dShader = static_cast<Direct3D12Shader*>(desc.ps);
        d3dDesc.PS.pShaderBytecode = d3dShader->bytecodeData;
        d3dDesc.PS.BytecodeLength = d3dShader->bytecodeSize;
    }

    // IA state
    std::vector<D3D12_INPUT_ELEMENT_DESC> d3dInputElements;
    for (const auto& element : desc.iaState.inputLayout) {
        DXGI_FORMAT format = convertFormat(element.format);
        D3D12_INPUT_CLASSIFICATION inputClassification = convertInputClassification(element.inputClassification);
        d3dInputElements.emplace_back(D3D12_INPUT_ELEMENT_DESC{
            "TEXCOORD", element.semanticIndex, format, element.inputSlot, element.offset,
            inputClassification, element.instanceStepRate
        });
    }
    d3dDesc.InputLayout.NumElements = d3dInputElements.size();
    d3dDesc.InputLayout.pInputElementDescs = d3dInputElements.data();
    d3dDesc.PrimitiveTopologyType = convertPrimitiveTopologyType(desc.iaState.topology);

    // RS state
    d3dDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // TODO
    d3dDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // TODO
    d3dDesc.RasterizerState.DepthClipEnable = TRUE; // TODO

    // CB state
    d3dDesc.BlendState.AlphaToCoverageEnable = desc.cbState.enableAlphaToCoverage;
    d3dDesc.BlendState.IndependentBlendEnable = desc.cbState.enableIndependentBlend;
    UINT sizeColorTargetBlendArray = d3dDesc.BlendState.IndependentBlendEnable ? 8 : 1;
    for (UINT i = 0; i < sizeColorTargetBlendArray; i++) {
        const auto& source = desc.cbState.colorTarget[i];
        auto& d3dTarget = d3dDesc.BlendState.RenderTarget[i];
        d3dTarget.BlendEnable = source.enableBlend;
        d3dTarget.LogicOpEnable = source.enableLogicOp;
        d3dTarget.SrcBlend = convertBlend(source.srcBlend);
        d3dTarget.DestBlend = convertBlend(source.destBlend);
        d3dTarget.BlendOp = convertBlendOp(source.blendOp);
        d3dTarget.SrcBlendAlpha = convertBlend(source.srcBlendAlpha);
        d3dTarget.DestBlendAlpha = convertBlend(source.destBlendAlpha);
        d3dTarget.BlendOpAlpha = convertBlendOp(source.blendOpAlpha);
        d3dTarget.LogicOp = convertLogicOp(source.logicOp);
        d3dTarget.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // TODO
    }

    hr = device->CreateGraphicsPipelineState(&d3dDesc, IID_PPV_ARGS(&pipeline->state));
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
    case FORMAT_R8G8B8A8:
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
