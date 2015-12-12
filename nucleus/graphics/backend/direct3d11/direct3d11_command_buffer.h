/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/graphics.h"
#include "nucleus/graphics/backend/direct3d11/direct3d11.h"


namespace gfx {

class Direct3D11CommandBuffer : public CommandBuffer {
public:
    ID3D11DeviceContext* context;

    Direct3D11CommandBuffer();
    ~Direct3D11CommandBuffer();

    virtual bool reset() override;

    // Commands
    virtual void cmdBindPipeline(Pipeline* pipeline) override;
    virtual void cmdClearColor(ColorTarget* target, const F32* colorValue) override;
    virtual void cmdClearDepthStencil(DepthStencilTarget* target, F32 depthValue, U8 stencilValue) override;
    virtual void cmdDraw(U32 firstVertex, U32 vertexCount, U32 firstInstance, U32 instanceCount) override;
    virtual void cmdDrawIndexed(U32 firstIndex, U32 indexCount, U32 vertexOffset, U32 firstInstance, U32 instanceCount) override;
    virtual void cmdSetVertexBuffers(U32 index, U32 vtxBufferCount, VertexBuffer** vtxBuffer, U32* offsets, U32* strides) override;
    virtual void cmdSetPrimitiveTopology(PrimitiveTopology topology) override;
    virtual void cmdSetTargets(U32 colorCount, ColorTarget** colorTargets, DepthStencilTarget* depthStencilTarget) override;
    virtual void cmdSetViewports(U32 viewportsCount, const Viewport* viewports) override;
    virtual void cmdSetScissors(U32 scissorsCount, const Rectangle* scissors) override;
};

}  // namespace gfx
