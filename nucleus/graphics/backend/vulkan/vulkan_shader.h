/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/shader.h"
#include "nucleus/graphics/backend/opengl/opengl.h"
#include "nucleus/graphics/hir/type.h"
#include "nucleus/graphics/hir/value.h"

#include <vector>

// Forward declarations
namespace gfx {
namespace hir {
class Instruction;
class Block;
class Function;
class Module;
}  // namespace hir
}  // namespace gfx

namespace gfx {
namespace vulkan {

class VulkanShader : public Shader {
public:
};

}  // namespace vulkan
}  // namespace gfx
