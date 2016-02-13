/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/gpu/rsx/rsx_enum.h"
#include "nucleus/gpu/rsx/rsx_pgraph.h"
#include "nucleus/gpu/rsx/rsx_texture.h"

#include "nucleus/graphics/format.h"
#include "nucleus/graphics/primitive.h"

namespace gpu {
namespace rsx {

gfx::Blend convertBlend(Blend func);
gfx::BlendOp convertBlendOp(BlendEquation equation);
gfx::ColorWriteMask convertColorMask(ColorMask mask);
gfx::LogicOp convertLogicOp(LogicOp op);
gfx::PrimitiveTopology convertPrimitiveTopology(Primitive primitive);
gfx::PrimitiveTopologyType convertPrimitiveTopologyType(Primitive primitive);
gfx::StencilOp convertStencilOp(StencilOp op);
gfx::Format convertFormat(Surface::ColorFormat format);
gfx::Format convertFormat(Surface::DepthStencilFormat format);
gfx::Format convertFormat(TextureFormat format);
gfx::Format convertVertexFormat(VertexType type, U8 size);

}  // namespace rsx
}  // namespace gpu
