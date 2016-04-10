/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "vulkan_shader_parser.h"
#include "nucleus/assert.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/frontend/vulkan/vulkan.h"

#include "nucleus/graphics/hir/block.h"
#include "nucleus/graphics/hir/builder.h"
#include "nucleus/graphics/hir/function.h"
#include "nucleus/graphics/hir/instruction.h"
#include "nucleus/graphics/hir/hir.h"
#include "nucleus/graphics/hir/module.h"

using namespace gfx::hir;

namespace gfx {
namespace frontend {
namespace vulkan {

std::vector<VulkanShaderParser::OpcodeInfo> VulkanShaderParser::opcodeInfo;

void VulkanShaderParser::initialize() {
    static bool initialized = false;
    if (initialized) {
        return;
    }

    const int opcodeCount = 320;
    opcodeInfo.resize(opcodeCount);
    opcodeInfo[spv::OpNop] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpSource] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpSourceContinued] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpSourceExtension] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpExtension] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpExtInstImport] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpCapability] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpMemoryModel] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpEntryPoint] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpExecutionMode] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpTypeVoid] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeBool] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeInt] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeFloat] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeVector] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeMatrix] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeImage] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeSampler] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeSampledImage] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeArray] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeRuntimeArray] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeStruct] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeOpaque] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypePointer] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeForwardPointer] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpTypeFunction] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeEvent] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeDeviceEvent] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeReserveId] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypeQueue] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpTypePipe] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpFunctionEnd] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpStore] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpImageWrite] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpDecorationGroup] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpDecorate] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpMemberDecorate] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpGroupDecorate] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpGroupMemberDecorate] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpName] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpMemberName] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpString] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpLine] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpNoLine] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpCopyMemory] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpCopyMemorySized] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpEmitVertex] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpEndPrimitive] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpEmitStreamVertex] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpEndStreamPrimitive] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpControlBarrier] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpMemoryBarrier] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpAtomicStore] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpLoopMerge] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpSelectionMerge] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpLabel] = OpcodeInfo(false, true);
    opcodeInfo[spv::OpBranch] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpBranchConditional] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpSwitch] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpKill] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpReturn] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpReturnValue] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpUnreachable] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpLifetimeStart] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpLifetimeStop] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpCommitReadPipe] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpCommitWritePipe] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpGroupCommitWritePipe] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpGroupCommitReadPipe] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpCaptureEventProfilingInfo] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpSetUserEventStatus] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpRetainEvent] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpReleaseEvent] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpGroupWaitEvents] = OpcodeInfo(false, false);
    opcodeInfo[spv::OpAtomicFlagClear] = OpcodeInfo(false, false);

    initialized = true;
}

bool VulkanShaderParser::hasTypeId(U16 opcode) {
    return opcodeInfo[opcode].hasTypeId;
}

bool VulkanShaderParser::hasResultId(U16 opcode) {
    return opcodeInfo[opcode].hasResultId;
}

bool VulkanShaderParser::checkHeader(const U32* header) {
    const U32 magic = header[0];
    const U32 version = header[1];
    const U32 generator = header[2];
    const U32 bound = header[3];
    const U32 schema = header[4];

    if (magic == 0x03022307) {
        assert_always("SPIR-V parser does not support loading reverse-endian binaries");
        return false;
    }
    if (magic != 0x07230203) {
        assert_always("SPIR-V magic number mismatch");
        return false;
    }
    if (version > 0x00010000) {
        const U08 major = version >> 16;
        const U08 minor = version >> 8;
        logger.warning(LOG_GRAPHICS, "VulkanShaderParser::parse: Unexpected SPIR-V version (%d.%02d)", major, minor);
    }
    if (schema != 0) {
        logger.warning(LOG_GRAPHICS, "VulkanShaderParser::parse: Unexpected SPIR-V schema (0x%X)", schema);
    }
    return true;
}

Module* VulkanShaderParser::parse(const char* data, size_t size) {
    initialize();
    assert_true(size % 4 == 0, "SPIR-V binary size should be a multiple of 4");
    assert_true(size > 0x30, "SPIR-V binary size should be at least 0x30");

    const U32 totalWordCount = size / 4;
    const U32* words = reinterpret_cast<const U32*>(data);
    if (!checkHeader(&words[0])) {
        return nullptr;
    }

    block = nullptr;
    function = nullptr;
    module = new Module();
    module->idInstructions.resize(words[3]);

    // Parse instruction stream
    size_t i = 5;
    while (i < totalWordCount) {
        // Instruction header
        const U32 header = words[i];
        constexpr Literal mask = (1 << spv::WordCountShift) - 1;
        U16 wordCount = (header >> spv::WordCountShift) & mask;
        size_t operandCount = wordCount - 1;
        assert_true(i + wordCount <= totalWordCount, "Instruction operands do not fit in the given buffer");
        i += 1;

        // Additional information
        spv::Op opcode = static_cast<spv::Op>(header & mask);
        U32 typeId = 0;
        U32 resultId = 0;
        if (hasTypeId(opcode)) {
            typeId = words[i++];
            operandCount -= 1;
        }
        if (hasResultId(opcode)) {
            resultId = words[i++];
            operandCount -= 1;
        }

        // Special instructions
        switch (opcode) {
        case spv::OpLabel:
            block = new Block(*function, resultId);
            break;
        case spv::OpFunction:
            function = new Function(*module, typeId, resultId);
            break;
        }

        // Create instruction and push operands
        Instruction* instruction = new Instruction(opcode, typeId, resultId);
        for (size_t operandIndex = 0; operandIndex < operandCount; operandIndex++) {
            instruction->operands.push_back(words[i++]);
        }
        module->idInstructions[resultId] = instruction;
        if (block) {
            block->instructions.push_back(instruction);
        } else if (function) {
            function->parameters.push_back(instruction);
        } else if (module) {
            module->header.push_back(instruction);
        }

        // Special instructions
        switch (opcode) {
        case spv::OpBranch:
        case spv::OpBranchConditional:
        case spv::OpSwitch:
        case spv::OpKill:
        case spv::OpReturn:
        case spv::OpReturnValue:
        case spv::OpUnreachable:
            block = nullptr;
            break;
        case spv::OpFunctionEnd:
            function = nullptr;
            break;
        }
    }
    return module;
}

}  // namespace vulkan
}  // namespace frontend
}  // namespace gfx
