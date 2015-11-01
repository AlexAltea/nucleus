/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_command_buffer.h"

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
}

void Direct3D12CommandBuffer::cmdClearDepthStencil(IDepthStencilTarget* target, F32 depthValue, U8 stencilValue) {
}

void Direct3D12CommandBuffer::cmdDraw() {
}

void Direct3D12CommandBuffer::cmdDrawIndexed() {
}

void Direct3D12CommandBuffer::cmdDrawIndirect() {
}

void Direct3D12CommandBuffer::cmdDrawIndexedIndirect() {
}

}  // namespace gfx
