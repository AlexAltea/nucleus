/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d11_command_buffer.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/backend/direct3d11/direct3d11_target.h"

namespace gfx {
namespace direct3d11 {

Direct3D11CommandBuffer::Direct3D11CommandBuffer() {
}

Direct3D11CommandBuffer::~Direct3D11CommandBuffer() {
}

bool Direct3D11CommandBuffer::reset() {
    return true;
}

void Direct3D11CommandBuffer::cmdBindPipeline(Pipeline* pipeline) {
}

void Direct3D11CommandBuffer::cmdClearColor(ColorTarget* target, const F32* colorValue) {
    auto d3dTarget = static_cast<Direct3D11ColorTarget*>(target);
    if (!d3dTarget) {
        logger.error(LOG_GRAPHICS, "Direct3D11CommandBuffer::cmdClearColor: Invalid target specified");
        return;
    }

    context->ClearRenderTargetView(d3dTarget->view, colorValue);
}

void Direct3D11CommandBuffer::cmdClearDepthStencil(DepthStencilTarget* target, F32 depthValue, U8 stencilValue) {
    auto d3dTarget = static_cast<Direct3D11DepthStencilTarget*>(target);
    if (!d3dTarget) {
        logger.error(LOG_GRAPHICS, "Direct3D11CommandBuffer::cmdClearDepthStencil: Invalid target specified");
        return;
    }

    context->ClearDepthStencilView(d3dTarget->view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthValue, stencilValue);
}

void Direct3D11CommandBuffer::cmdDraw(U32 firstVertex, U32 vertexCount, U32 firstInstance, U32 instanceCount) {
}

void Direct3D11CommandBuffer::cmdDrawIndexed(U32 firstIndex, U32 indexCount, U32 vertexOffset, U32 firstInstance, U32 instanceCount) {
}

void Direct3D11CommandBuffer::cmdSetVertexBuffers(U32 index, U32 vtxBufferCount, VertexBuffer** vtxBuffer, U32* offsets, U32* strides) {
}

void Direct3D11CommandBuffer::cmdSetTargets(U32 colorCount, ColorTarget** colorTargets, DepthStencilTarget* depthStencilTarget) {
    context->OMSetRenderTargets(colorCount, nullptr, nullptr); // TODO
}

void Direct3D11CommandBuffer::cmdSetPrimitiveTopology(PrimitiveTopology topology) {
    D3D11_PRIMITIVE_TOPOLOGY d3dTopology;
    switch (topology) {
    case TOPOLOGY_POINT_LIST:
        d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST; break;
    case TOPOLOGY_LINE_LIST:
        d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST; break;
    case TOPOLOGY_LINE_STRIP:
        d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
    case TOPOLOGY_TRIANGLE_LIST:
        d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
    case TOPOLOGY_TRIANGLE_STRIP:
        d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
    default:
        logger.error(LOG_GRAPHICS, "Unimplemented primitive topology type");
        return;
    }
    context->IASetPrimitiveTopology(d3dTopology);
}

void Direct3D11CommandBuffer::cmdSetViewports(U32 viewportsCount, const Viewport* viewports) {
    if (!viewports) {
        logger.error(LOG_GRAPHICS, "Direct3D11CommandBuffer::cmdSetViewports: Invalid viewport array specified");
        return;
    }

    auto* d3dViewports = new D3D11_VIEWPORT[viewportsCount];
    for (U32 i = 0; i < viewportsCount; i++) {
        d3dViewports[i].TopLeftX = viewports[i].originX;
        d3dViewports[i].TopLeftY = viewports[i].originY;
        d3dViewports[i].Width = viewports[i].width;
        d3dViewports[i].Height = viewports[i].height;
        d3dViewports[i].MinDepth = viewports[i].minDepth;
        d3dViewports[i].MaxDepth = viewports[i].maxDepth;
    }

    context->RSSetViewports(viewportsCount, d3dViewports);
    delete[] d3dViewports;
}

void Direct3D11CommandBuffer::cmdSetScissors(U32 scissorsCount, const Rectangle* scissors) {
    if (!scissors) {
        logger.error(LOG_GRAPHICS, "Direct3D11CommandBuffer::cmdSetScissors: Invalid scissor rectangle array specified");
        return;
    }

    auto* d3dRects = new D3D11_RECT[scissorsCount];
    for (U32 i = 0; i < scissorsCount; i++) {
        d3dRects[i].left = scissors[i].left;
        d3dRects[i].top = scissors[i].top;
        d3dRects[i].right = scissors[i].right;
        d3dRects[i].bottom = scissors[i].bottom;
    }

    context->RSSetScissorRects(scissorsCount, d3dRects);
    delete[] d3dRects;
}

}  // namespace direct3d11
}  // namespace gfx
