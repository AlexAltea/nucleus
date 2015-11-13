/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_command_buffer.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12_target.h"

namespace gfx {

Direct3D12CommandBuffer::Direct3D12CommandBuffer() {
}

Direct3D12CommandBuffer::~Direct3D12CommandBuffer() {
}

bool Direct3D12CommandBuffer::reset() {
    return true;
}

void Direct3D12CommandBuffer::cmdBindPipeline(IPipelineState* pipeline) {
}

void Direct3D12CommandBuffer::cmdClearColor(IColorTarget* target, const F32* colorValue) {
    auto d3dTarget = static_cast<Direct3D12ColorTarget*>(target);
    if (!d3dTarget) {
        logger.error(LOG_GRAPHICS, "Direct3D12CommandBuffer::cmdClearColor: Invalid target specified");
        return;
    }

    list->ClearRenderTargetView(d3dTarget->handle, colorValue, 0, nullptr);
}

void Direct3D12CommandBuffer::cmdClearDepthStencil(IDepthStencilTarget* target, F32 depthValue, U8 stencilValue) {
    auto d3dTarget = static_cast<Direct3D12DepthStencilTarget*>(target);
    if (!d3dTarget) {
        logger.error(LOG_GRAPHICS, "Direct3D12CommandBuffer::cmdClearDepthStencil: Invalid target specified");
        return;
    }

    list->ClearDepthStencilView(d3dTarget->handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depthValue, stencilValue, 0, nullptr);
}

void Direct3D12CommandBuffer::cmdDraw() {
}

void Direct3D12CommandBuffer::cmdDrawIndexed() {
}

void Direct3D12CommandBuffer::cmdDrawIndirect() {
}

void Direct3D12CommandBuffer::cmdDrawIndexedIndirect() {
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

void Direct3D12CommandBuffer::cmdSetTargets(U32 colorCount, IColorTarget** colorTargets, IDepthStencilTarget* depthStencilTarget) {
    list->OMSetRenderTargets(colorCount, nullptr, FALSE, nullptr); // TODO
}

void Direct3D12CommandBuffer::cmdSetViewports(U32 viewportsCount, const Viewport* viewports) {
    if (!viewports) {
        logger.error(LOG_GRAPHICS, "Direct3D12CommandBuffer::cmdSetViewports: Invalid viewport array specified");
        return;
    }

    auto* d3dViewports = new D3D12_VIEWPORT[viewportsCount];
    for (U32 i = 0; i < viewportsCount; i++) {
        d3dViewports[i].TopLeftX = viewports[i].originX;
        d3dViewports[i].TopLeftY = viewports[i].originY;
        d3dViewports[i].Width = viewports[i].width;
        d3dViewports[i].Height = viewports[i].height;
        d3dViewports[i].MinDepth = viewports[i].minDepth;
        d3dViewports[i].MaxDepth = viewports[i].maxDepth;
    }

    list->RSSetViewports(viewportsCount, d3dViewports);
    delete[] d3dViewports;
}

void Direct3D12CommandBuffer::cmdSetScissors(U32 scissorsCount, const Rectangle* scissors) {
    if (!scissors) {
        logger.error(LOG_GRAPHICS, "Direct3D12CommandBuffer::cmdSetScissors: Invalid scissor rectangle array specified");
        return;
    }

    auto* d3dRects = new D3D12_RECT[scissorsCount];
    for (U32 i = 0; i < scissorsCount; i++) {
        d3dRects[i].left = scissors[i].left;
        d3dRects[i].top = scissors[i].top;
        d3dRects[i].right = scissors[i].right;
        d3dRects[i].bottom = scissors[i].bottom;
    }

    list->RSSetScissorRects(scissorsCount, d3dRects);
    delete[] d3dRects;
}

}  // namespace gfx
