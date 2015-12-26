/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/pipeline.h"
#include "nucleus/graphics/primitive.h"
#include "nucleus/graphics/backend/opengl/opengl.h"

namespace gfx {
namespace opengl {

GLenum convertBlend(gfx::Blend blend);
GLenum convertBlendOp(gfx::BlendOp blendOp);
GLenum convertLogicOp(gfx::LogicOp logicOp);
GLenum convertPrimitiveTopology(gfx::PrimitiveTopology primitiveTopology);

}  // namespace opengl
}  // namespace gfx
