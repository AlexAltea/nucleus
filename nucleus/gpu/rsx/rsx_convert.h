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
gfx::ColorWriteEnable convertColorMask(ColorMask mask);
gfx::PrimitiveTopology convertPrimitiveTopology(Primitive primitive);
gfx::PrimitiveTopologyType convertPrimitiveTopologyType(Primitive primitive);
gfx::Format convertFormat(Surface::ColorFormat format);
gfx::Format convertFormat(Surface::DepthStencilFormat format);
gfx::Format convertFormat(TextureFormat format);

}  // namespace rsx
}  // namespace gpu
