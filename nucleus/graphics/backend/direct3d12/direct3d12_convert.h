/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/format.h"
#include "nucleus/graphics/pipeline.h"
#include "nucleus/graphics/primitive.h"
#include "nucleus/graphics/resource.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"

namespace gfx {
namespace direct3d12 {

D3D12_BLEND convertBlend(gfx::Blend blend);
D3D12_BLEND_OP convertBlendOp(gfx::BlendOp blendOp);
DXGI_FORMAT convertFormat(gfx::Format format);
D3D12_INPUT_CLASSIFICATION convertInputClassification(gfx::InputClassification inputClassification);
D3D12_LOGIC_OP convertLogicOp(gfx::LogicOp logicOp);
D3D12_PRIMITIVE_TOPOLOGY_TYPE convertPrimitiveTopologyType(gfx::PrimitiveTopology primitiveTopology);
D3D12_RESOURCE_STATES convertResourceState(gfx::ResourceState resourceState);

}  // namespace direct3d12
}  // namespace gfx
