/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/shader.h"
#include "nucleus/graphics/backend/vulkan/vulkan.h"

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
private:
    // Compile HIR components into SPIR-V bytecode
    void dump(const hir::Instruction& i);
    void dump(const hir::Block& block);
    void dump(const hir::Function& function);
    void dump(const hir::Module& module);

public:
    std::vector<U32> binary;

    bool initialize(const ShaderDesc& desc);
};

}  // namespace vulkan
}  // namespace gfx
