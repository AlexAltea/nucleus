/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_command_buffer.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_convert.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_resource.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_pipeline.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_target.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_vertex_buffer.h"

namespace gfx {
namespace direct3d12 {

Direct3D12CommandBuffer::Direct3D12CommandBuffer() {
}

Direct3D12CommandBuffer::~Direct3D12CommandBuffer() {
}

bool Direct3D12CommandBuffer::initialize(ID3D12Device* device) {
    HRESULT hr;
    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));
    if (FAILED(hr)) {
        return false;
    }
    hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, NULL, IID_PPV_ARGS(&list));
    if (FAILED(hr)) {
        return false;
    }
    hr = list->Close();
    if (FAILED(hr)) {
        return false;
    }
    return true;
}

bool Direct3D12CommandBuffer::reset() {
    HRESULT hr = list->Reset(allocator, nullptr);
    return SUCCEEDED(hr);
}

bool Direct3D12CommandBuffer::finalize() {
    HRESULT hr = list->Close();
    return SUCCEEDED(hr);
}

void Direct3D12CommandBuffer::cmdBindPipeline(Pipeline* pipeline) {
    auto d3dPipeline = static_cast<Direct3D12Pipeline*>(pipeline);
    if (!d3dPipeline) {
        logger.error(LOG_GRAPHICS, "Direct3D12CommandBuffer::cmdBindPipeline: Invalid pipeline specified");
        return;
    }
    list->SetGraphicsRootSignature(d3dPipeline->rootSignature);
    list->SetPipelineState(d3dPipeline->state);
}

void Direct3D12CommandBuffer::cmdClearColor(ColorTarget* target, const F32* colorValue) {
    auto d3dTarget = static_cast<Direct3D12ColorTarget*>(target);
    if (!d3dTarget) {
        logger.error(LOG_GRAPHICS, "Direct3D12CommandBuffer::cmdClearColor: Invalid target specified");
        return;
    }
    list->ClearRenderTargetView(d3dTarget->handle, colorValue, 0, nullptr);
}

void Direct3D12CommandBuffer::cmdClearDepthStencil(DepthStencilTarget* target, F32 depthValue, U8 stencilValue) {
    auto d3dTarget = static_cast<Direct3D12DepthStencilTarget*>(target);
    if (!d3dTarget) {
        logger.error(LOG_GRAPHICS, "Direct3D12CommandBuffer::cmdClearDepthStencil: Invalid target specified");
        return;
    }

    list->ClearDepthStencilView(d3dTarget->handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depthValue, stencilValue, 0, nullptr);
}

void Direct3D12CommandBuffer::cmdDraw(U32 firstVertex, U32 vertexCount, U32 firstInstance, U32 instanceCount) {
    list->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
}

void Direct3D12CommandBuffer::cmdDrawIndexed(U32 firstIndex, U32 indexCount, U32 vertexOffset, U32 firstInstance, U32 instanceCount) {
    list->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void Direct3D12CommandBuffer::cmdSetVertexBuffers(U32 index, U32 vtxBufferCount, VertexBuffer** vtxBuffer, U32* offsets, U32* strides) {
    std::vector<D3D12_VERTEX_BUFFER_VIEW> vtxBufferView(vtxBufferCount);
    for (UINT i = 0; i < vtxBufferCount; i++) {
        auto* d3dVertexBuffer = static_cast<Direct3D12VertexBuffer*>(vtxBuffer[i]);
        vtxBufferView[i].BufferLocation = d3dVertexBuffer->resource->GetGPUVirtualAddress();
        vtxBufferView[i].StrideInBytes = strides[i];
        vtxBufferView[i].SizeInBytes = 0xC; // TODO
    }
    UINT startSlot = index;
    UINT numViews = vtxBufferCount;
    list->IASetVertexBuffers(startSlot, numViews, vtxBufferView.data());
}

void Direct3D12CommandBuffer::cmdSetPrimitiveTopology(PrimitiveTopology topology) {
    D3D12_PRIMITIVE_TOPOLOGY d3dTopology;
    switch (topology) {
    case TOPOLOGY_POINT_LIST:
        d3dTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; break;
    case TOPOLOGY_LINE_LIST:
        d3dTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST; break;
    case TOPOLOGY_LINE_STRIP:
        d3dTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
    case TOPOLOGY_TRIANGLE_LIST:
        d3dTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
    case TOPOLOGY_TRIANGLE_STRIP:
        d3dTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
    default:
        logger.error(LOG_GRAPHICS, "Unimplemented primitive topology type");
        return;
    }
    list->IASetPrimitiveTopology(d3dTopology);
}

void Direct3D12CommandBuffer::cmdSetTargets(U32 colorCount, ColorTarget** colorTargets, DepthStencilTarget* depthStencilTarget) {
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> renderTargets(colorCount);
    for (UINT i = 0; i < colorCount; i++) {
        auto* d3dColorTarget = static_cast<Direct3D12ColorTarget*>(colorTargets[i]);
        renderTargets[i] = d3dColorTarget->handle;
    }
    list->OMSetRenderTargets(colorCount, renderTargets.data(), FALSE, nullptr); // TODO
}

void Direct3D12CommandBuffer::cmdSetViewports(U32 viewportsCount, const Viewport* viewports) {
    if (!viewports) {
        logger.error(LOG_GRAPHICS, "Direct3D12CommandBuffer::cmdSetViewports: Invalid viewport array specified");
        return;
    }

    std::vector<D3D12_VIEWPORT> d3dViewports(viewportsCount);
    for (U32 i = 0; i < viewportsCount; i++) {
        d3dViewports[i].TopLeftX = viewports[i].originX;
        d3dViewports[i].TopLeftY = viewports[i].originY;
        d3dViewports[i].Width = viewports[i].width;
        d3dViewports[i].Height = viewports[i].height;
        d3dViewports[i].MinDepth = 0.0f;
        d3dViewports[i].MaxDepth = 1.0f;
    }

    list->RSSetViewports(viewportsCount, d3dViewports.data());
}

void Direct3D12CommandBuffer::cmdSetScissors(U32 scissorsCount, const Rectangle* scissors) {
    if (!scissors) {
        logger.error(LOG_GRAPHICS, "Direct3D12CommandBuffer::cmdSetScissors: Invalid scissor rectangle array specified");
        return;
    }

    std::vector<D3D12_RECT> d3dRects(scissorsCount);
    for (U32 i = 0; i < scissorsCount; i++) {
        d3dRects[i].left = scissors[i].left;
        d3dRects[i].top = scissors[i].top;
        d3dRects[i].right = scissors[i].right;
        d3dRects[i].bottom = scissors[i].bottom;
    }

    list->RSSetScissorRects(scissorsCount, d3dRects.data());
}

void Direct3D12CommandBuffer::cmdResourceBarrier(U32 barrierCount, const ResourceBarrier* barriers) {
    std::vector<D3D12_RESOURCE_BARRIER> d3dBarriers(barrierCount);
    for (U32 i = 0; i < barrierCount; i++) {
        d3dBarriers[i].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        d3dBarriers[i].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        d3dBarriers[i].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        d3dBarriers[i].Transition.pResource = static_cast<Direct3D12Resource*>(barriers[i].transition.resource)->handle;
        d3dBarriers[i].Transition.StateBefore = convertResourceState(barriers[i].transition.before);
        d3dBarriers[i].Transition.StateAfter = convertResourceState(barriers[i].transition.after);
    }
    list->ResourceBarrier(barrierCount, d3dBarriers.data());
}

}  // namespace direct3d12
}  // namespace gfx
