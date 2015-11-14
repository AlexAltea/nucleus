/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/graphics.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"


namespace gfx {

class Direct3D12CommandBuffer : public CommandBuffer {
public:
    ID3D12GraphicsCommandList* list;

    Direct3D12CommandBuffer();
    ~Direct3D12CommandBuffer();

    virtual bool reset() override;

    virtual void cmdBindPipeline(IPipelineState* pipeline) override;
    virtual void cmdClearColor(ColorTarget* target, const F32* colorValue) override;
    virtual void cmdClearDepthStencil(DepthStencilTarget* target, F32 depthValue, U8 stencilValue) override;
    virtual void cmdDraw() override;
    virtual void cmdDrawIndexed() override;
    virtual void cmdDrawIndirect() override;
    virtual void cmdDrawIndexedIndirect() override;
    virtual void cmdSetTargets(U32 colorCount, ColorTarget** colorTargets, DepthStencilTarget* depthStencilTarget) override;
    virtual void cmdSetPrimitiveTopology(PrimitiveTopology topology) override;
    virtual void cmdSetViewports(U32 viewportsCount, const Viewport* viewports) override;
    virtual void cmdSetScissors(U32 scissorsCount, const Rectangle* scissors) override;
};

}  // namespace gfx
