/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "vulkan_shader.h"
#include "nucleus/assert.h"
#include "nucleus/format.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/hir/block.h"
#include "nucleus/graphics/hir/function.h"
#include "nucleus/graphics/hir/instruction.h"

namespace gfx {
namespace vulkan {

using namespace gfx::hir;

void VulkanShader::dump(const Instruction& i) {
    Literal wordCount = 1;
    if (i.typeId)
        wordCount += 1;
    if (i.resultId)
        wordCount += 1;

    Literal header = (wordCount << spv::WordCountShift) | i.opcode;
    binary.push_back(header);
    if (i.typeId)
        binary.push_back(i.typeId);
    if (i.resultId)
        binary.push_back(i.resultId);
    for (const auto& operand : i.operands) {
        binary.push_back(operand);
    }
}

bool VulkanShader::initialize(const ShaderDesc& desc) {
    binary.insert(binary.end(), {
        spv::MagicNumber,
        spv::Version,
        0, 0, 0,
    });

    // Module
    Module* module = desc.module;
    for (const auto* instruction : module->header) {
        dump(*instruction);
    }
    for (const auto* function : module->functions) {
        for (const auto* block : function->blocks) {
            for (const auto* instruction : block->instructions) {
                dump(*instruction);
            }
        }
    }

    return true;
}

}  // namespace vulkan
}  // namespace gfx
