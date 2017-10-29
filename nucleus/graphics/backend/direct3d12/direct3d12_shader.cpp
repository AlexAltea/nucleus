/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "direct3d12_shader.h"
#include "nucleus/assert.h"
#include "nucleus/format.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/hir/block.h"
#include "nucleus/graphics/hir/function.h"
#include "nucleus/graphics/hir/instruction.h"

#include <iostream>

namespace gfx {
namespace direct3d12 {

using namespace gfx::hir;

// Defines
#define PADDING "  "

// Decoration
const char* Direct3D12Shader::getBuiltin(Literal builtinDecoration) {
    switch (builtinDecoration) {
    case BUILTIN_POSITION:      return "SV_Position";
    case BUILTIN_POINTSIZE:     return "PSIZE";
    case BUILTIN_CLIPDISTANCE:  return "SV_ClipDistance";
    case BUILTIN_CULLDISTANCE:  return "SV_CullDistance";
    case BUILTIN_VERTEXID:      return "SV_VertexID";
    case BUILTIN_INSTANCEID:    return "SV_InstanceID";
    case BUILTIN_PRIMITIVEID:   return "SV_PrimitiveID";
    case BUILTIN_FRAGCOORD:     return "SV_Position";
    case BUILTIN_POINTCOORD:    return "SV_Position";
    case BUILTIN_FRONTFACING:   return "SV_IsFrontFace";
    case BUILTIN_FRAGDEPTH:     return "SV_Depth";

    case BUILTIN_INVOCATIONID:
        assert_always("Unavailable");
        return nullptr;

    default:
        assert_always("Unimplemented");
        return nullptr;
    }
}

void Direct3D12Shader::appendUniform(Literal typeId, Literal resultId) {
    Instruction* underlyingType = module->idInstructions[module->idInstructions[typeId]->operands[1]];
    if (underlyingType->opcode == OP_TYPE_SAMPLED_IMAGE) {
        Instruction* imageType = module->idInstructions[underlyingType->operands[0]];
        Literal dimension = imageType->operands[1];
        std::string texture;
        std::string sampler = format("SamplerState v%ds", resultId);
        switch (dimension) {
        case DIMENSION_1D: texture = format("Texture1D v%d", resultId); break;
        case DIMENSION_2D: texture = format("Texture2D v%d", resultId); break;
        case DIMENSION_3D: texture = format("Texture3D v%d", resultId); break;
        }
        if (idDecoration.find(resultId) != idDecoration.end()) {
            Literal binding = idDecoration[resultId][0]->operands[2];
            texture += format(" : register(t%d)", binding);
            sampler += format(" : register(s%d)", binding);
        }
        sourceUniforms += format("%s;\n%s;\n", texture.c_str(), sampler.c_str());
    }
    else {
        std::string type = getType(underlyingType->resultId);
        sourceUniforms += format("cbuffer C%d : register(b%d) { %s v%d; };\n", resultId, countArbitraryCBV++, type.c_str(), resultId);
    }
}

void Direct3D12Shader::appendInput(Literal typeId, Literal resultId) {
    Literal location;
    bool definedLocation = false;

    std::string type = getType(typeId);
    if (module->idInstructions[module->idInstructions[typeId]->operands[1]]->opcode == OP_TYPE_STRUCT) {
        sourceInput += format(PADDING "%s v%d;\n", type.c_str(), resultId);
        return;
    } else if (idDecoration.find(resultId) != idDecoration.end()) {
        Literal decorationType = idDecoration[resultId][0]->operands[1];
        if (decorationType == DECORATION_BUILTIN) {
            Literal builtinType = idDecoration[resultId][0]->operands[2];
            if (builtinType == BUILTIN_VERTEXID || builtinType == BUILTIN_INSTANCEID) {
                type = "uint";
            }
            if (shaderType == SHADER_TYPE_PIXEL) {
                // NOTE: This is a workaround for Glslang-emitted shaders. Prepend to ensure that SV_Position appears on top and linking succeeds.
                sourceInput = format(PADDING "%s v%d : %s;\n", type.c_str(), resultId, getBuiltin(builtinType)) + sourceInput;
            } else {
                sourceInput += format(PADDING "%s v%d : %s;\n", type.c_str(), resultId, getBuiltin(builtinType));
            }
            return;
        }
        if (decorationType == DECORATION_LOCATION) {
            location = idDecoration[resultId][0]->operands[2];
            definedLocation = true;
        }
    }

    if (!definedLocation) {
        location = countArbitraryInput++;
    }
    if (location >= userInputs.size()) {
        userInputs.resize(location + 1);
    }
    assert_true(userInputs[location].empty());
    if (shaderType == SHADER_TYPE_VERTEX) {
        userInputs[location] = format(PADDING "%s v%d : INPUT%d;\n", type.c_str(), resultId, location);
    } else {
        userInputs[location] = format(PADDING "%s v%d : LINK%d;\n", type.c_str(), resultId, location);
    }
}

void Direct3D12Shader::appendOutput(Literal typeId, Literal resultId) {
    Literal location;
    bool definedLocation = false;

    std::string type = getType(typeId);
    if (module->idInstructions[module->idInstructions[typeId]->operands[1]]->opcode == OP_TYPE_STRUCT) {
        sourceOutput += format(PADDING "%s v%d;\n", type.c_str(), resultId);
        return;
    } else if (idDecoration.find(resultId) != idDecoration.end()) {
        Literal decorationType = idDecoration[resultId][0]->operands[1];
        if (decorationType == DECORATION_BUILTIN) {
            Literal builtinType = idDecoration[resultId][0]->operands[2];
            if (builtinType == BUILTIN_VERTEXID || builtinType == BUILTIN_INSTANCEID) {
                type = "uint";
            }
            if (builtinType == BUILTIN_POSITION) {
                idBuiltinPosition = resultId;
            }
            sourceOutput += format(PADDING "%s v%d : %s;\n", type.c_str(), resultId, getBuiltin(builtinType));
            return;
        }
        if (decorationType == DECORATION_LOCATION) {
            location = idDecoration[resultId][0]->operands[2];
            definedLocation = true;
        }
    }

    if (!definedLocation) {
        location = countArbitraryOutput++;
    }
    if (location >= userOutputs.size()) {
        userOutputs.resize(location + 1);
    }
    assert_true(userOutputs[location].empty());
    if (shaderType == SHADER_TYPE_PIXEL) {
        userOutputs[location] = format(PADDING "%s v%d : SV_Target%d;\n", type.c_str(), resultId, location);
    } else {
        userOutputs[location] = format(PADDING "%s v%d : LINK%d;\n", type.c_str(), resultId, location);
    }
}

void Direct3D12Shader::appendStatic(hir::Literal typeId, hir::Literal resultId) {
    // TODO: Value initialization is hardcoded for float4
    std::string type = getType(typeId);
    sourceStatics += format("static %s v%d = %s(0.0, 0.0, 0.0, 0.0);\n", type.c_str(), resultId, type.c_str());
}

// Conversion
std::string Direct3D12Shader::getType(Literal instrId) {
    if (!idCache[instrId].empty()) {
        return idCache[instrId];
    }

    std::string typeName;
    Instruction* instr = module->idInstructions[instrId];

    // Pointer
    if (instr->opcode == OP_TYPE_POINTER) {
        Literal storageClass = instr->operands[0];
        Literal type = instr->operands[1];
        return getType(type);
    }

    // Basic types
    else if (instr->opcode == OP_TYPE_BOOL) {
        typeName = "bool";
    }
    else if (instr->opcode == OP_TYPE_INT) {
        Literal width = instr->operands[0];
        Literal signedness = instr->operands[1];
        assert_true(width == 32, "HLSL 5.0 only supports integer widths of 32");
        typeName = !signedness ? "uint" : "int";
    }
    else if (instr->opcode == OP_TYPE_FLOAT) {
        Literal width = instr->operands[0];
        switch (width) {
        case 16:  typeName = "half"; break;
        case 32:  typeName = "float"; break;
        case 64:  typeName = "double"; break;
        default:
            assert_true(width == 32, "HLSL 5.0 only supports floating-point widths of 16, 32 or 64");
        }
    }

    // Vectors
    else if (instr->opcode == OP_TYPE_VECTOR) {
        Literal componentType = instr->operands[0];
        Literal componentCount = instr->operands[1];
        assert_true(1 <= componentCount && componentCount <= 4, "HLSL 5.0 only supports between 1 and 4 vector components");
        std::string componentTypeName = getType(componentType);
        typeName = format("%s%d", componentTypeName.c_str(), componentCount);
    }

    // Matrices
    else if (instr->opcode == OP_TYPE_MATRIX) {
        Literal columnType = instr->operands[0];
        Literal columnCount = instr->operands[1];
        assert_true(1 <= columnType && columnType <= 4, "HLSL 5.0 only supports between 1 and 4 matrix columns");
        std::string componentTypeName = getType(columnType);
        typeName = format("%sx%d", componentTypeName.c_str(), columnCount);
    }

    // Arrays
    else if (instr->opcode == OP_TYPE_ARRAY) {
        typeName = format("t%d", instr->resultId);
        std::string type = getType(instr->operands[0]);
        std::string length = getConstant(instr->operands[1]);
        sourceTypes += format("typedef %s %s[%s];\n", type.c_str(), typeName.c_str(), length.c_str());
    }

    // Structs
    else if (instr->opcode == OP_TYPE_STRUCT) {
        Instruction** decorations = nullptr;
        if (idDecoration.find(instrId) != idDecoration.end()) {
            decorations = idDecoration[instrId].data();
        }
        std::string members;
        for (size_t i = 0; i < instr->operands.size(); i++) {
            Literal operand = instr->operands[i];
            std::string memberType = getType(operand);
            if (decorations && decorations[i]) {
                Literal builtinType = decorations[i]->operands[3];
                if (builtinType == BUILTIN_POINTSIZE ||
                    builtinType == BUILTIN_CLIPDISTANCE ||
                    builtinType == BUILTIN_CULLDISTANCE) {
                    members += "//";
                }
                members += format(PADDING "%s m%d : %s;\n", memberType.c_str(), i, getBuiltin(builtinType));
            } else {
                members += format(PADDING "%s m%d;\n", memberType.c_str(), i);
            }
        }
        typeName = format("t%d", instr->resultId);
        sourceTypes += format("struct %s {\n%s};\n", typeName.c_str(), members.c_str());
    }

    // Save type
    idCache[instrId] = typeName;
    return typeName;
}

std::string Direct3D12Shader::getConstant(Literal instrId) {
    if (!idCache[instrId].empty()) {
        return idCache[instrId];
    }

    std::string constantString;
    Instruction* instr = module->idInstructions[instrId];

    Instruction* type = module->idInstructions[instr->typeId];
    if (type->opcode == OP_TYPE_INT) {
        Literal width = type->operands[0];
        Literal signedness = type->operands[1];
        assert_true(width <= 32, "Support for integer constants bigger than 32-bits is not implemented");
        S32 constant = (S32&)(instr->operands[0]);
        constantString = format("%d%c", constant, !signedness ? 'U' : 0);
    }
    if (type->opcode == OP_TYPE_FLOAT) {
        Literal width = type->operands[0];
        assert_true(width <= 32, "Support for floating-point constants different than 32-bits is not implemented");
        F32 constant = (F32&)(instr->operands[0]);
        constantString = format("%f", constant);
    }

    idCache[instrId] = constantString;
    return constantString;
}

std::string Direct3D12Shader::getPointer(Literal pointerId) {
    if (!idCache[pointerId].empty()) {
        return idCache[pointerId];
    }

    Instruction* pointerInstr = module->idInstructions[pointerId];
    Instruction* typeInstr = module->idInstructions[pointerInstr->typeId];
    assert(typeInstr->opcode == OP_TYPE_POINTER);

    std::string pointerString;
    switch (typeInstr->operands[0]) {
    case STORAGE_CLASS_INPUT:
        pointerString = "input."; break;
    case STORAGE_CLASS_OUTPUT:
        pointerString = "output."; break;
    case STORAGE_CLASS_UNIFORM_CONSTANT:
    case STORAGE_CLASS_FUNCTION:
    case STORAGE_CLASS_PRIVATE:
        break;
    default:
        assert_always("Unimplemented");
    }

    if (pointerInstr->opcode == OP_VARIABLE) {
        pointerString += format("v%d", pointerInstr->resultId);
    }
    if (pointerInstr->opcode == OP_ACCESS_CHAIN) {
        pointerString += format("v%d", pointerInstr->operands[0]);
        Instruction* varType = module->idInstructions[pointerInstr->operands[0]];
        Instruction* ptrType = module->idInstructions[varType->typeId];
        Instruction* underlyingType = module->idInstructions[ptrType->operands[1]];
        if (underlyingType->opcode == OP_TYPE_STRUCT) {
            for (size_t i = 1; i < pointerInstr->operands.size(); i++) {
                std::string constant = getConstant(pointerInstr->operands[i]);
                pointerString += format(".m%s", constant.c_str());
            }
        } else if (underlyingType->opcode == OP_TYPE_ARRAY || underlyingType->opcode == OP_TYPE_VECTOR) {
            std::string constant = getConstant(pointerInstr->operands[1]);
            pointerString += format("[%s]", constant.c_str());
        }
    }

    idCache[pointerId] = pointerString;
    return pointerString;
}

std::string Direct3D12Shader::emitBinaryOp(Instruction* i, Opcode type, char symbol) {
    Literal result = i->resultId;
    Literal lhs = i->operands[0];
    Literal rhs = i->operands[1];

#if defined(NUCLEUS_BUILD_DEBUG)
    Instruction* lhsType = module->idInstructions[module->idInstructions[lhs]->typeId];
    Instruction* rhsType = module->idInstructions[module->idInstructions[rhs]->typeId];
    assert(lhsType->opcode == rhsType->opcode);
    if (lhsType->opcode == OP_TYPE_VECTOR) {
        assert(type == module->idInstructions[lhsType->operands[0]]->opcode);
    } else {
        assert(type == lhsType->opcode);
    }
#endif

    std::string typeStr = getType(i->typeId);
    return format(PADDING "%s v%d = v%d %c v%d;\n", typeStr.c_str(), result, lhs, symbol, rhs);
}

std::string Direct3D12Shader::emitBinaryOp(hir::Instruction* i, hir::Opcode type, const char* symbol) {
    Literal result = i->resultId;
    Literal lhs = i->operands[0];
    Literal rhs = i->operands[1];

    std::string typeStr = getType(i->typeId);
    return format(PADDING "%s v%d = v%d %s v%d;\n", typeStr.c_str(), result, lhs, symbol, rhs);
}

std::string Direct3D12Shader::emitBinaryFunctionOp(Instruction* i, Opcode type, const char* function, bool reverseArgs) {
    Literal result = i->resultId;
    Literal lhs = i->operands[reverseArgs ? 1 : 0];
    Literal rhs = i->operands[reverseArgs ? 0 : 1];

    std::string typeStr = getType(i->typeId);
    return format(PADDING "%s v%d = %s(v%d, v%d);\n", typeStr.c_str(), result, function, lhs, rhs);
}

std::string Direct3D12Shader::emitOpCompositeConstruct(Instruction* i) {
    assert_true(i->operands.size() >= 2);
    assert_true(module->idInstructions[i->typeId]->opcode == OP_TYPE_VECTOR);

    std::string type = getType(i->typeId);
    std::string constitutents = format("v%d", i->operands[0]);
    for (size_t idx = 1; idx < i->operands.size(); idx++) {
        constitutents += format(", v%d", i->operands[idx]);
    }
    Literal result = i->resultId;
    return format(PADDING "%s v%d = %s(%s);\n", type.c_str(), result, type.c_str(), constitutents.c_str());
}

std::string Direct3D12Shader::emitOpCompositeExtract(Instruction* i) {
    assert_true(i->operands.size() >= 2);

    Literal result = i->resultId;
    Literal composite = i->operands[0];
    std::string type = getType(i->typeId);
    return format(PADDING "%s v%d = v%d[%d];\n", type.c_str(), result, composite, i->operands[1]);
}

std::string Direct3D12Shader::emitOpImageSample(Instruction* i) {
    assert_true(i->operands.size() >= 2);

    hir::Instruction* target = module->idInstructions[i->operands[0]];
    while (target->opcode != OP_VARIABLE) {
        switch (target->opcode) {
        case OP_LOAD:
            target = module->idInstructions[target->operands[0]]; break;
        default:
            assert_always("Unexpected opcode");
        }
    }
    std::string type = getType(i->typeId);
    Literal result = i->resultId;
    Literal image = target->resultId;
    Literal coord = i->operands[1];
    return format(PADDING "%s v%d = v%d.Sample(v%ds, v%d);\n", type.c_str(), result, image, image, coord);
}

std::string Direct3D12Shader::emitOpLoad(Instruction* i) {
    assert_true(i->operands.size() >= 1);

    Literal result = i->resultId;
    std::string type = getType(i->typeId);
    std::string pointerStr = getPointer(i->operands[0]);
    if (type.empty()) {
        return "";
    }
    return format(PADDING "%s v%d = %s;\n", type.c_str(), result, pointerStr.c_str());
}

std::string Direct3D12Shader::emitOpStore(Instruction* i) {
    assert_true(i->operands.size() >= 2);

    Literal object = i->operands[1];
    std::string pointerStr = getPointer(i->operands[0]);
    return format(PADDING "%s = v%d;\n", pointerStr.c_str(), object);
}

std::string Direct3D12Shader::emitOpVariable(hir::Instruction* i) {
    assert_true(i->operands.size() >= 1);
    assert_true(i->operands[0] == STORAGE_CLASS_FUNCTION);

    Literal result = i->resultId;
    Literal derefType = module->idInstructions[i->typeId]->operands[1];
    std::string type = getType(derefType);
    return format(PADDING "%s v%d;\n", type.c_str(), result);
}

std::string Direct3D12Shader::emitOpVectorShuffle(hir::Instruction* i) {
    assert_true(i->operands.size() >= 2);
    Literal v1Id = i->operands[0];
    Literal v2Id = i->operands[1];
    Literal v1TypeId = module->idInstructions[v1Id]->typeId;
    Literal v2TypeId = module->idInstructions[v2Id]->typeId;

    std::string components;
    std::string v1Str = format("v%d.", v1Id);
    std::string v2Str = format("v%d.", v2Id);
    Literal v1CompCount = module->idInstructions[v1TypeId]->operands[1];
    Literal v2CompCount = module->idInstructions[v2TypeId]->operands[1];
    for (Size idx = 2; idx < i->operands.size(); idx++) {
        Literal comp = i->operands[idx];
        if (comp < v1CompCount) {
            components += (idx != 2 ? ", " : "") + v1Str + "xyzw"[comp];
        } else {
            components += (idx != 2 ? ", " : "") + v2Str + "xyzw"[comp - v1CompCount];
        }
    }
    Literal result = i->resultId;
    std::string type = getType(i->typeId);
    return format(PADDING "%s v%d = %s(%s);\n", type.c_str(), result, type.c_str(), components.c_str());
}

std::string Direct3D12Shader::emitUnaryOp(hir::Instruction* i, Literal valueId, const char* function) {
    Literal result = i->resultId;
    std::string typeStr = getType(i->typeId);
    return format(PADDING "%s v%d = %s(v%d);\n", typeStr.c_str(), result, function, valueId);
}

std::string Direct3D12Shader::emitOpExtInst(Instruction* i) {
    std::string source;
    // TODO: Assuming GLSL extensions
    Literal instruction = i->operands[1];
    switch (instruction) {
    case GLSLstd450FAbs:
        return emitUnaryOp(i, i->operands[2], "abs");
    case GLSLstd450Floor:
        return emitUnaryOp(i, i->operands[2], "floor");
    case GLSLstd450Fract:
        return emitUnaryOp(i, i->operands[2], "frac");
    default:
        assert_always("Unimplemented");
        return source;
    }
}

std::string Direct3D12Shader::compile(Instruction* i) {
    std::string source;
    switch (i->opcode) {
    case OP_FNEGATE:
        return emitUnaryOp(i, i->operands[0], "-");
    case OP_FADD:
        return emitBinaryOp(i, OP_TYPE_FLOAT, '+');
    case OP_FSUB:
        return emitBinaryOp(i, OP_TYPE_FLOAT, '-');
    case OP_FMUL:
        return emitBinaryOp(i, OP_TYPE_FLOAT, '*');
    case OP_FDIV:
        return emitBinaryOp(i, OP_TYPE_FLOAT, '/');
    case OP_FMOD:
        return emitBinaryOp(i, OP_TYPE_FLOAT, '%'); // TODO: Is this correct?
    case OP_FREM:
        return emitBinaryOp(i, OP_TYPE_FLOAT, '%'); // TODO: Is this correct?
    case OP_IADD:
        return emitBinaryOp(i, OP_TYPE_INT, '+');
    case OP_ISUB:
        return emitBinaryOp(i, OP_TYPE_INT, '-');
    case OP_IMUL:
        return emitBinaryOp(i, OP_TYPE_INT, '*');
    case OP_SDIV:
        return emitBinaryOp(i, OP_TYPE_INT, '/');
    case OP_UDIV:
        return emitBinaryOp(i, OP_TYPE_INT, '/');
    case OP_SMOD:
        return emitBinaryOp(i, OP_TYPE_INT, '%'); // TODO: Is this correct?
    case OP_UMOD:
        return emitBinaryOp(i, OP_TYPE_INT, '%'); // TODO: Is this correct?
    case OP_SREM:
        return emitBinaryOp(i, OP_TYPE_INT, '%'); // TODO: Is this correct?
    case OP_DOT:
        return emitBinaryFunctionOp(i, OP_TYPE_FLOAT, "dot");
    case OP_MATRIX_TIMES_VECTOR:
        return emitBinaryFunctionOp(i, OP_TYPE_FLOAT, "mul", true);

    // Comparison ops
    case OP_FORD_EQUAL:
        return emitBinaryOp(i, OP_TYPE_INT, "==");
    case OP_FORD_NOT_EQUAL:
        return emitBinaryOp(i, OP_TYPE_INT, "!=");
    case OP_FORD_LESS_THAN:
        return emitBinaryOp(i, OP_TYPE_INT,  "<");
    case OP_FORD_GREATER_THAN:
        return emitBinaryOp(i, OP_TYPE_INT,  ">");
    case OP_FORD_LESS_THAN_EQUAL:
        return emitBinaryOp(i, OP_TYPE_INT, "<=");
    case OP_FORD_GREATER_THAN_EQUAL:
        return emitBinaryOp(i, OP_TYPE_INT, ">=");

    case OP_VECTOR_SHUFFLE:
        return emitOpVectorShuffle(i);
    case OP_COMPOSITE_EXTRACT:
        return emitOpCompositeExtract(i);
    case OP_COMPOSITE_CONSTRUCT:
        return emitOpCompositeConstruct(i);
    case OP_IMAGE_SAMPLE_IMPLICIT_LOD:
        return emitOpImageSample(i);
    case OP_VARIABLE:
        return emitOpVariable(i);
    case OP_LOAD:
        return emitOpLoad(i);
    case OP_STORE:
        return emitOpStore(i);
    case OP_EXT_INST:
        return emitOpExtInst(i);
    }
    return source;
}

std::string Direct3D12Shader::compile(Block* block) {
    std::string source;
    for (auto* instruction : block->instructions) {
        source += compile(instruction);
    }
    return source;
}

std::string Direct3D12Shader::compile(Function* function) {
    std::string source;

    // Function declaration
    Literal funcId = function->function->resultId;
    if (idCache[funcId].empty()) {
        assert_always("Unimplemented");
    }

    source += idCache[funcId] + " {\n";
    if (idEntryPoint == funcId) {
        source += PADDING "TOutput output = (TOutput)(0);\n";
    }
    for (auto* var : function->blocks[0]->variables) {
        source += emitOpVariable(var);
    }
    for (auto* block : function->blocks) {
        source += compile(block);
    }
    if (idEntryPoint == funcId) {
        if (shaderType == SHADER_TYPE_VERTEX) {
            // Correct coordinate system (TODO: Implement proper system to detect BUILTIN_POSITION's)
            if (idBuiltinPosition != 0) {
                source += format(PADDING "output.v%d.y *= -1.0f;\n", idBuiltinPosition);
            }
        }
        source += PADDING "return output;\n";
    }
    source += "}\n\n";
    return source;
}

std::string Direct3D12Shader::compile(Module* module) {
    std::string source;

    countArbitraryInput = 0;
    countArbitraryOutput = 0;

    // Compile module header
    for (Instruction* i : module->header) {
        if (i->opcode == OP_ENTRY_POINT) {
            Literal funcId = i->operands[1];
            idCache[funcId] = "TOutput main(TInput input)";
            idEntryPoint = funcId;
        }
        if (i->opcode == OP_DECORATE) {
            Literal decoratedId = i->operands[0];
            if (idDecoration[decoratedId].empty()) {
                idDecoration[decoratedId] = { i };
            }
        }
        if (i->opcode == OP_MEMBER_DECORATE && i->operands[2] == DECORATION_BUILTIN) {
            Literal decoratedId = i->operands[0];
            Literal decoratedMember = i->operands[1];
            if (idDecoration.find(decoratedId) == idDecoration.end()) {
                idDecoration[decoratedId] = {};
            }
            if (idDecoration[decoratedId].size() <= decoratedMember) {
                idDecoration[decoratedId].resize(decoratedMember + 1);
            }
            idDecoration[decoratedId][decoratedMember] = i;
        }
        if (i->opcode == OP_CONSTANT) {
            std::string type = getType(i->typeId);
            std::string constant = getConstant(i->resultId);
            sourceConstants += format("static %s v%d = %s;\n", type.c_str(), i->resultId, constant.c_str());
        }
        if (i->opcode == OP_CONSTANT_COMPOSITE) {
            std::string type = getType(i->typeId);
            std::string components;
            for (const auto& c : i->operands) {
                components += getConstant(c) + ", ";
            }
            components = components.substr(0, components.size() - 2);
            sourceConstants += format("static %s v%d = %s(%s);\n", type.c_str(), i->resultId, type.c_str(), components.c_str());
        }
        if (i->opcode == OP_VARIABLE) {
            Literal storageClass = i->operands[0];
            switch (storageClass) {
            case STORAGE_CLASS_UNIFORM_CONSTANT:
                appendUniform(i->typeId, i->resultId);
                break;
            case STORAGE_CLASS_INPUT:
                appendInput(i->typeId, i->resultId);
                break;
            case STORAGE_CLASS_OUTPUT:
                appendOutput(i->typeId, i->resultId);
                break;
            case STORAGE_CLASS_PRIVATE:
                appendStatic(i->typeId, i->resultId);
                break;
            default:
                assert_always("Unimplemented");
            }
        }
    }

    // Append remaining Input+Output variables
    for (const auto& var : userInputs) {
        sourceInput += var;
    }
    for (const auto& var : userOutputs) {
        sourceOutput += var;
    }
    source = format("%s\nstruct TInput {\n%s};\nstruct TOutput {\n%s};\n%s\n%s\n%s\n",
        sourceTypes.c_str(), sourceInput.c_str(), sourceOutput.c_str(),
        sourceUniforms.c_str(), sourceConstants.c_str(), sourceStatics.c_str());

    // Compile functions
    for (auto* function : module->functions) {
        source += compile(function);
    }

    return source;
}

bool Direct3D12Shader::initialize(const ShaderDesc& desc) {
    // Initialization
    module = desc.module;
    shaderType = desc.type;
    idCache.resize(module->idInstructions.size());

    std::string source = compile(module);
    // std::cout << source << std::endl;
    static int i = 0;
    if (i == 8) {
        source = R"(
typedef float4 t6[468U];

struct TInput {
  float4 v29 : INPUT0;
};
struct TOutput {
  float4 v15 : SV_Position;
  float4 v17 : LINK1;
  float4 v18 : LINK2;
  float4 v19 : LINK6;
  float4 v20 : LINK7;
  float4 v21 : LINK8;
  float4 v22 : LINK9;
  float4 v23 : LINK10;
  float4 v24 : LINK11;
  float4 v25 : LINK12;
  float4 v26 : LINK13;
  float4 v27 : LINK14;
  float4 v28 : LINK15;
};
cbuffer C7 : register(b0) { t6 v7; };
cbuffer C9 : register(b1) { float4x4 v9; };

static uint v5 = 468U;
static int v41 = 467;
static int v101 = 466;

static float4 v34 = float4(0.0, 0.0, 0.0, 0.0);
static float4 v60 = float4(0.0, 0.0, 0.0, 0.0);

TOutput main(TInput input) {
  TOutput output = (TOutput)(0);
  /*float4 v31 = input.v29;
  float4 v32 = float4(v31.x, v31.y, v31.x, v31.x);
  float4 v33 = floor(v32);
  float4 v36 = v34;
  float4 v37 = float4(v33.x, v33.y, v36.z, v36.w);
  v34 = v37;
  float4 v38 = input.v29;
  float4 v39 = float4(v38.w, v38.w, v38.w, v38.z);
  float4 v44 = v7[467];
  float4 v45 = float4(v44.x, v44.x, v44.x, v44.x);
  float4 v47 = v7[467];
  float4 v48 = float4(v47.y, v47.y, v47.y, v47.y);
  float4 v49 = -(v48);
  float4 v50 = v39 * v45;
  float4 v51 = v50 + v49;
  float4 v52 = v34;
  float4 v53 = float4(v52.x, v52.y, v51.z, v51.w);
  v34 = v53;
  float4 v54 = input.v29;
  float4 v55 = float4(v54.x, v54.x, v54.x, v54.y);
  float4 v57 = v7[467];
  float4 v58 = float4(v57.z, v57.z, v57.z, v57.z);
  float4 v59 = v55 * v58;
  float4 v61 = v60;
  float4 v62 = float4(v61.x, v61.y, v59.z, v59.w);
  v60 = v62;
  float4 v63 = input.v29;
  float4 v64 = float4(v63.y, v63.x, v63.y, v63.y);
  float4 v65 = frac(v64);
  float4 v66 = v60;
  float4 v67 = float4(v65.x, v65.y, v66.z, v66.w);
  v60 = v67;
  float4 v68 = v60;
  float4 v69 = float4(v68.y, v68.x, v68.y, v68.y);
  float4 v71 = v7[467];
  float4 v72 = float4(v71.x, v71.x, v71.x, v71.x);
  float4 v73 = v69 * v72;
  float4 v74 = output.v20;
  float4 v75 = float4(v73.x, v73.y, v74.z, v74.w);
  output.v20 = v75;
  float4 v76 = v60;
  float4 v77 = float4(v76.w, v76.z, v76.w, v76.w);
  float4 v78 = floor(v77);
  float4 v79 = v60;
  float4 v80 = float4(v78.x, v78.y, v79.z, v79.w);
  v60 = v80;
  float4 v81 = v34;
  float4 v82 = float4(v81.z, v81.z, v81.z, v81.z);
  float4 v83 = -(v82);
  float4 v84 = output.v15;
  float4 v85 = float4(v84.x, v83.y, v84.z, v84.w);
  output.v15 = v85;
  float4 v86 = v34;
  float4 v87 = float4(v86.w, v86.w, v86.w, v86.w);
  float4 v88 = output.v15;
  float4 v89 = float4(v87.x, v88.y, v88.z, v88.w);
  output.v15 = v89;
  float4 v90 = v60;
  float4 v91 = float4(v90.y, v90.y, v90.y, v90.x);
  float4 v93 = v7[467];
  float4 v94 = float4(v93.w, v93.w, v93.w, v93.w);
  float4 v95 = v91 * v94;
  float4 v96 = v34;
  float4 v97 = float4(v96.x, v96.y, v95.z, v95.w);
  v34 = v97;
  float4 v98 = v34;
  float4 v99 = float4(v98.z, v98.w, v98.z, v98.z);
  float4 v100 = -(v99);
  float4 v103 = v7[466];
  float4 v104 = float4(v103.x, v103.x, v103.x, v103.x);
  float4 v105 = v34;
  float4 v106 = float4(v105.x, v105.y, v105.x, v105.x);
  float4 v107 = v100 * v104;
  float4 v108 = v107 + v106;
  float4 v109 = v34;
  float4 v110 = float4(v108.x, v108.y, v109.z, v109.w);
  v34 = v110;
  float4 v111 = v34;
  float4 v112 = float4(v111.z, v111.z, v111.z, v111.w);
  float4 v113 = output.v17;
  float4 v114 = float4(v113.x, v112.y, v113.z, v112.w);
  output.v17 = v114;
  float4 v115 = v34;
  float4 v116 = float4(v115.x, v115.x, v115.y, v115.x);
  float4 v118 = v7[467];
  float4 v119 = float4(v118.w, v118.w, v118.w, v118.w);
  float4 v120 = v116 * v119;
  float4 v121 = output.v17;
  float4 v122 = float4(v120.x, v121.y, v120.z, v121.w);
  output.v17 = v122;
  float4 v123 = output.v15;
  float4x4 v124 = v9;
  float4 v125 = mul(v123, v124);
  output.v15 = v125;
  output.v15.y *= -1.0f;*/


	float4 tmp0 = float4(0., 0., 0., 0.);;
	float4 tmp1 = float4(0., 0., 0., 0.);;
	float4 dst_reg7 = float4(0.0, 0.0, 0.0, 0.0);
	float4 dst_reg0 = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 dst_reg1 = float4(0.0, 0.0, 0.0, 0.0);
	float4 in_pos = input.v29;
	tmp0.xy = floor(in_pos.xyxx).xy;
	tmp0.zw = (in_pos.wwwz * v7[467].xxxx + -v7[467].yyyy).zw;
	tmp1.zw = (in_pos.xxxy * v7[467].zzzz).zw;
	tmp1.xy = frac(in_pos.yxyy).xy;
	dst_reg7.xy = (tmp1.yxyy * v7[467].xxxx).xy;
	tmp1.xy = floor(tmp1.wzww).xy;
	dst_reg0.y = -tmp0.zzzz.y;
	dst_reg0.x = tmp0.wwww.x;
	tmp0.zw = (tmp1.yyyx * v7[467].wwww).zw;
	tmp0.xy = (-tmp0.zwzz * v7[466].xxxx + tmp0.xyxx).xy;
	dst_reg1.yw = tmp0.zzzw.yw;
	dst_reg1.xz = (tmp0.xxyx * v7[467].wwww).xz;
   output.v15 = dst_reg0;
   output.v17 = dst_reg1;
   output.v20 = dst_reg7;



  return output;
}
)";
    }
    if (i == 999) {
        source = R"(
struct TInput {
  float4 v9 : SV_Position;
  float4 v11 : LINK1;
  float4 v12 : LINK2;
  float4 v22 : LINK6;
  float4 v13 : LINK7;
  float4 v14 : LINK8;
  float4 v15 : LINK9;
  float4 v16 : LINK10;
  float4 v17 : LINK11;
  float4 v18 : LINK12;
  float4 v19 : LINK13;
  float4 v20 : LINK14;
  float4 v21 : LINK15;
};
struct TOutput {
  float4 v42 : SV_Target0;
};
Texture2D v24 : register(t0);
SamplerState v24s : register(s0);

static float v32 = 0.000000;
static float v33 = 1.000000;
static float4 v34 = float4(0.000000, 1.000000, 0.000000, 0.000000);

TOutput main(TInput input) {
  TOutput output = (TOutput)(0);
  /*output.v42 = float4(1.0f, 1.0f, 0.0f, 1.0f);*/


    float4 tc0 = In.tc0;
	float4 diff_color = In.diff_color;
	float4 gl_FragCoord = In.Position;
	gl_FragCoord.y = (720 - gl_FragCoord.y);
	float4 ssa = is_front_face ? float4(1., 1., 1., 1.) : float4(-1., -1., -1., -1.);
	float2  tex0_scale = float2(1., 1.);
	r0.w = tex0.Sample(tex0sampler, tc0.xy * tex0_scale).w;
	h0 = diff_color;
	r0.w = (h0 * r0).w;
	r0.xyz = h0.xyz;


  return output;
}
)";
    }
    i++;

    LPCSTR target;
    switch (desc.type) {
    case SHADER_TYPE_VERTEX:    target = "vs_5_0"; break;
    case SHADER_TYPE_HULL:      target = "hs_5_0"; break;
    case SHADER_TYPE_DOMAIN:    target = "ds_5_0"; break;
    case SHADER_TYPE_GEOMETRY:  target = "gs_5_0"; break;
    case SHADER_TYPE_PIXEL:     target = "ps_5_0"; break;
    default:
        assert_always("Unimplemented case");
        return false;
    }

    UINT compileFlags = 0;
#ifdef NUCLEUS_BUILD_DEBUG
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* bytecode;
    ID3DBlob* errors;
    HRESULT hr = _D3DCompile(source.data(), source.size(), "shader", nullptr, nullptr, "main", target, compileFlags, 0, &bytecode, &errors);
    if (FAILED(hr)) {
        LPVOID errorString = errors->GetBufferPointer();
        logger.error(LOG_GPU, "Direct3D12Shader::initialize: Cannot compile shader (0x%X):\n%s\nShader source:%s\n", hr, errorString, source.data());
        return false;
    }

    bytecodeData = bytecode->GetBufferPointer();
    bytecodeSize = bytecode->GetBufferSize();
    return true;
}

}  // namespace direct3d12
}  // namespace gfx
