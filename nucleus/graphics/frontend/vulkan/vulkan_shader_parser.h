/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/hir/builder.h"
#include "nucleus/graphics/hir/hir.h"

// Forward declarations
namespace gfx {
namespace hir {

class Block;
class Function;
class Module;

}  // namespace hir
}  // namespace gfx

namespace gfx {
namespace frontend {
namespace vulkan {

/**
 * Vulkan Shader Parser
 * ====================
 * Takes SPIR-V binary code as input and translates it to a module containing
 * the High-level Intermediate Representation of the given shader.
 */
class VulkanShaderParser {
private:
    struct OpcodeInfo {
        bool hasTypeId;
        bool hasResultId;

        OpcodeInfo() :
            hasTypeId(true), hasResultId(true) {}
        OpcodeInfo(bool hasTypeId, bool hasResultId) :
            hasTypeId(hasTypeId), hasResultId(hasResultId) {}
    };

    static std::vector<OpcodeInfo> opcodeInfo;

    static void initialize();

    // Current targets
    hir::Block* block;
    hir::Function* function;
    hir::Module* module;

    /**
     * Determines whether the instruction has type ID word based on its opcode
     * @param[in]  opcode  Instruction opcode
     * @return             Instruction has a type ID word
     */
    bool hasTypeId(U16 opcode);

    /**
     * Determines whether the instruction has result ID word based on its opcode
     * @param[in]  opcode  Instruction opcode
     * @return             Instruction has a result ID word
     */
    bool hasResultId(U16 opcode);

    /**
     * Checks the correctness of the SPIR-V binary header
     * @param[in]  header  Header of the SPIR-V binary
     * @return             Header is valid
     */
    bool checkHeader(const U32* header);

public:
    hir::Module* parse(const char* data, size_t size);
};

}  // namespace opengl
}  // namespace frontend
}  // namespace gfx
