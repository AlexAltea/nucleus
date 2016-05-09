/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/backend/direct3d12/direct3d12.h"
#include "nucleus/graphics/format.h"
#include "nucleus/graphics/pipeline.h"
#include "nucleus/graphics/primitive.h"
#include "nucleus/graphics/resource.h"
#include "nucleus/graphics/texture.h"

namespace gfx {
namespace direct3d12 {

D3D12_BLEND convertBlend(gfx::Blend blend);
D3D12_BLEND_OP convertBlendOp(gfx::BlendOp blendOp);
D3D12_CULL_MODE convertCullMode(gfx::CullMode cullMode);
D3D12_FILL_MODE convertFillMode(gfx::FillMode fillMode);
UINT8 convertColorWriteMask(gfx::ColorWriteMask mask);
D3D12_DEPTH_WRITE_MASK convertDepthWriteMask(gfx::DepthWriteMask mask);
D3D12_COMPARISON_FUNC convertComparisonFunc(gfx::ComparisonFunc comparisonFunc);
D3D12_FILTER convertFilter(gfx::Filter filter);
DXGI_FORMAT convertFormat(gfx::Format format);
D3D12_INPUT_CLASSIFICATION convertInputClassification(gfx::InputClassification inputClassification);
D3D12_LOGIC_OP convertLogicOp(gfx::LogicOp logicOp);
D3D12_PRIMITIVE_TOPOLOGY_TYPE convertPrimitiveTopologyType(gfx::PrimitiveTopology primitiveTopology);
D3D12_RESOURCE_STATES convertResourceState(gfx::ResourceState resourceState);
D3D12_STENCIL_OP convertStencilOp(gfx::StencilOp stencilOp);
D3D12_TEXTURE_ADDRESS_MODE convertTextureAddressMode(gfx::TextureAddress address);
D3D12_RESOURCE_FLAGS convertTextureFlags(gfx::TextureFlags flags);

}  // namespace direct3d12
}  // namespace gfx
