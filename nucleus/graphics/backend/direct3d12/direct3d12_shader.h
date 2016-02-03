/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/shader.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"
#include "nucleus/graphics/hir/hir.h"

#include <vector>
#include <unordered_map>

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
namespace direct3d12 {

class Direct3D12Shader : public Shader {
    hir::Module* module;
    ShaderType shaderType;

    // Header
    int countArbitraryInput;
    int countArbitraryOutput;
    hir::Literal idEntryPoint;
    std::unordered_map<hir::Literal, std::vector<hir::Instruction*>> idDecoration;
    const char* getBuiltin(hir::Literal builtinDecoration);

    // Source
    std::vector<std::string> idCache;
    std::vector<std::string> userInputs;
    std::vector<std::string> userOutputs;
    std::string sourceTypes;
    std::string sourceInput;
    std::string sourceOutput;
    std::string sourceConstants;
    std::string sourceUniforms;

    void appendUniform(hir::Literal typeId, hir::Literal resultId);
    void appendInput(hir::Literal typeId, hir::Literal resultId);
    void appendOutput(hir::Literal typeId, hir::Literal resultId);

    // Conversion
    std::string getType(hir::Literal typeId);
    std::string getConstant(hir::Literal constantId);
    std::string getPointer(hir::Literal pointerId);

    // Emitters
    std::string emitBinaryOp(hir::Instruction* i, hir::Opcode type, char symbol);
    std::string emitOpCompositeConstruct(hir::Instruction* i);
    std::string emitOpCompositeExtract(hir::Instruction* i);
    std::string emitOpImageSample(hir::Instruction* i);
    std::string emitOpLoad(hir::Instruction* i);
    std::string emitOpStore(hir::Instruction* i);

    // Compile HIR components into HLSL
    std::string compile(hir::Instruction* i);
    std::string compile(hir::Block* block);
    std::string compile(hir::Function* function);
    std::string compile(hir::Module* module);

public:
    const void* bytecodeData;
    SIZE_T bytecodeSize;

    bool initialize(const ShaderDesc& desc);
};

}  // namespace direct3d12
}  // namespace gfx
