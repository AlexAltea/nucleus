/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "vulkan_shader.h"
#include "nucleus/assert.h"
#include "nucleus/format.h"
#include "nucleus/logger/logger.h"

namespace gfx {
namespace vulkan {

using namespace gfx::hir;

bool VulkanShader::initialize(const ShaderDesc& desc) {
    binary.insert(binary.end(), {
        spv::MagicNumber,
        spv::Version,
        0, 0, 0,
    });
}

}  // namespace vulkan
}  // namespace gfx
