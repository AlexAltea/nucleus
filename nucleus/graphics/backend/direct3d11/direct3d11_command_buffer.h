/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/graphics.h"
#include "nucleus/graphics/backend/direct3d11/direct3d11.h"


namespace gfx {

class Direct3D11CommandBuffer : public ICommandBuffer {
public:
    ID3D11DeviceContext* context;

    Direct3D11CommandBuffer();
    ~Direct3D11CommandBuffer();

    virtual bool reset() override;

    virtual void cmdBindPipeline(IPipelineState* pipeline) override;
    virtual void cmdClearColor(IColorTarget* target, const F32* colorValue) override;
    virtual void cmdClearDepthStencil(IDepthStencilTarget* target, F32 depthValue, U8 stencilValue) override;
    virtual void cmdDraw() override;
    virtual void cmdDrawIndexed() override;
    virtual void cmdDrawIndirect() override;
    virtual void cmdDrawIndexedIndirect() override;
    virtual void cmdSetTargets(U32 colorCount, IColorTarget** colorTargets, IDepthStencilTarget* depthStencilTarget) override;
    virtual void cmdSetPrimitiveTopology(PrimitiveTopology topology) override;
    virtual void cmdSetViewports(U32 viewportsCount, const Viewport* viewports) override;
    virtual void cmdSetScissors(U32 scissorsCount, const Rectangle* scissors) override;
};

}  // namespace gfx
