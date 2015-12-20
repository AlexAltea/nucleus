/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/pipeline.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"

namespace gfx {
namespace direct3d12 {

D3D12_BLEND convertBlend(gfx::Blend blend);
D3D12_BLEND_OP convertBlendOp(gfx::BlendOp blendOp);
D3D12_LOGIC_OP convertLogicOp(gfx::LogicOp logicOp);

}  // namespace direct3d12
}  // namespace gfx
