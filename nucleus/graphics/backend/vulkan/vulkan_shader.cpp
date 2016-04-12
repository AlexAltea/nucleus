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
    wordCount += i.operands.size();

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

void VulkanShader::dump(const Block& block) {
    dump(*block.instructions[0]);
    for (const auto& var : block.variables) {
        dump(*var);
    }
    for (Size i = 1; i < block.instructions.size(); i++) {
        dump(*block.instructions[i]);
    }
}

void VulkanShader::dump(const Function& function) {
    dump(*function.function);
    for (const auto& param : function.parameters) {
        dump(*param);
    }
    for (const auto& block : function.blocks) {
        dump(*block);
    }
    dump(Instruction(OP_FUNCTION_END, 0, 0));
}

void VulkanShader::dump(const Module& module) {
    // Header
    for (const auto& i : module.hCapabilities)     { dump(*i); }
    for (const auto& i : module.hExtensions)       { dump(*i); }
    for (const auto& i : module.hImports)          { dump(*i); }
    for (const auto& i : module.hMemoryModel)      { dump(*i); }
    for (const auto& i : module.hEntryPoints)      { dump(*i); }
    for (const auto& i : module.hExecutions)       { dump(*i); }
    for (const auto& i : module.hDebugs)           { dump(*i); }
    for (const auto& i : module.hAnnotation)       { dump(*i); }
    for (const auto& i : module.hConstsTypesGlobs) { dump(*i); }

    // Functions
    for (const auto& function : module.functions) {
        dump(*function);
    }
}

bool VulkanShader::initialize(const ShaderDesc& desc) {
    const Module& module = *desc.module;

    // Binary header
    auto bound = static_cast<U32>(module.idInstructions.size());
    binary.insert(binary.end(), {
        spv::MagicNumber,
        spv::Version,
        0, bound, 0,
    });

    dump(module);
    return true;
}

}  // namespace vulkan
}  // namespace gfx
