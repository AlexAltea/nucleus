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

    // Arrays
    else if (instr->opcode == OP_TYPE_ARRAY) {
        typeName = format("t%d", instr->resultId);
        std::string type = getType(instr->operands[0]);
        std::string length = getConstant(instr->operands[1]);
        sourceTypes += format("typedef %s %s[%s];\n", type.c_str(), typeName.c_str(), length.c_str());
    }

    // Structs
    else if (instr->opcode == OP_TYPE_STRUCT) {
        std::string members;
        typeName = format("t%d", instr->resultId);
        for (size_t i = 0; i < instr->operands.size(); i++) {
            Literal operand = instr->operands[i];
            std::string memberType = getType(operand);
            members += format("  %s m%d;\n", memberType.c_str(), i);
        }
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
        Literal width = instr->operands[0];
        assert_true(width <= 32, "Support for floating-point constants different than 32-bits is not implemented");
        F32 constant = (F32&)(instr->operands[0]);
        constantString = format("%f", constant);
    }

    idCache[instrId] = constantString;
    return constantString;
}

/*const char* Direct3D12Shader::getBuiltin(ValueBuiltin builtin) {
    return "UNIMPLEMENTED";
}

// Emitters
std::string Direct3D12Shader::getDst(Value* value) {
    if (value->flags & (VALUE_IS_INPUT | VALUE_IS_OUTPUT | VALUE_IS_BUILTIN)) {
        return getName(value);
    } else {
        return getDeclaration(value);
    }
}

std::string Direct3D12Shader::getSrc(Value* value) {
    if (value->flags & VALUE_IS_CONSTANT) {
        return getConstant(value);
    } else {
        return getName(value);
    }
}

std::string Direct3D12Shader::emitOp(const char* fmt) {
    return fmt;
}

std::string Direct3D12Shader::emitOp(const char* fmt, hir::Value* v0) {
    std::string v0Name = getDst(v0);
    return format(fmt, v0Name.c_str());
}

std::string Direct3D12Shader::emitOp(const char* fmt, hir::Value* v0, hir::Value* v1) {
    std::string v0Name = getDst(v0);
    std::string v1Name = getSrc(v1);
    return format(fmt, v0Name.c_str(), v1Name.c_str());
}

std::string Direct3D12Shader::emitOp(const char* fmt, hir::Value* v0, hir::Value* v1, hir::Value* v2) {
    std::string v0Name = getDst(v0);
    std::string v1Name = getSrc(v1);
    std::string v2Name = getSrc(v2);
    return format(fmt, v0Name.c_str(), v1Name.c_str(), v2Name.c_str());
}

// Utilities
std::string Direct3D12Shader::getConstant(Value* value) {
    Value::Constant& c = value->constant;
    switch (value->type) {
    case TYPE_F32:
        return format("float(%f)", c.f32);
    case TYPE_V128:
        return format("float4(%f, %f, %f, %f)", c.v128.f32[0], c.v128.f32[1], c.v128.f32[2], c.v128.f32[3]);
    default:
        assert_always("Unimplemented case");
        return "UNIMPLEMENTED";
    }
}

std::string Direct3D12Shader::getName(hir::Value* value) {
    const S32 valueId = value->getId();
    if (value->flags & VALUE_IS_ARGUMENT) {
        return format("a%d", valueId);
    } else if (value->flags & VALUE_IS_INPUT) {
        return format("input.i%d", valueId);
    } else if (value->flags & VALUE_IS_OUTPUT) {
        return format("output.o%d", valueId);
    } else if (value->flags & VALUE_IS_BUILTIN) {
        return getBuiltin(value->builtin);
    } else {
        return format("v%d", valueId);
    }
}

std::string Direct3D12Shader::getName(hir::Function* function) {
    if (function->flags & FUNCTION_IS_ENTRY) {
        return "main";
    } else {
        return format("func%d", function->getId());
    }
}

std::string Direct3D12Shader::getDeclaration(Value* value) {
    std::string valueType = getType(value->type);
    std::string valueName = getName(value);
    return format("%s %s", valueType.c_str(), valueName.c_str());
}

std::string Direct3D12Shader::getDeclaration(Function* function) {
    std::string source;

    if (function->flags & FUNCTION_IS_ENTRY) {
        return "TOutput main(TInput input)";
    }

    // Function name
    std::string funcName = getName(function);
    source += format("%s %s(", getType(function->typeOut), funcName.c_str());

    // Function arguments
    for (auto* value : function->args) {
        source += getDeclaration(value);
        if (value->getId() != function->args.size() - 1) {
            source += format(", ");
        }
    }
    source += ")";
    return source;
}*/

std::string Direct3D12Shader::emitBinaryOp(hir::Literal lhs, hir::Literal rhs, hir::Opcode type, char symbol) {
#if defined(NUCLEUS_BUILD_DEBUG)
    Instruction* lhsType = module->idInstructions[module->idInstructions[lhs]->typeId];
    Instruction* rhsType = module->idInstructions[module->idInstructions[rhs]->typeId];
    assert(lhsType->opcode == rhsType->opcode);
    if (lhsType->opcode != OP_TYPE_VECTOR) {
        assert(type == module->idInstructions[lhsType->operands[0]]->opcode);
    } else {
        assert(type == lhsType->opcode);
    }
#endif

    return format(" %s = %s %c %s;\n", "TODO", "TODO", symbol, "TODO");
}

std::string Direct3D12Shader::compile(Instruction* i) {
    std::string source;
    switch (i->opcode) {
    case OP_FADD:
        return emitBinaryOp(i->operands[0], i->operands[1], OP_TYPE_FLOAT, '+');
    case OP_FSUB:
        return emitBinaryOp(i->operands[0], i->operands[1], OP_TYPE_FLOAT, '-');
    case OP_FMUL:
        return emitBinaryOp(i->operands[0], i->operands[1], OP_TYPE_FLOAT, '*');
    case OP_FDIV:
        return emitBinaryOp(i->operands[0], i->operands[1], OP_TYPE_FLOAT, '/');
    case OP_FMOD:
        return emitBinaryOp(i->operands[0], i->operands[1], OP_TYPE_FLOAT, '%'); // TODO: Is this correct?
    case OP_FREM:
        return emitBinaryOp(i->operands[0], i->operands[1], OP_TYPE_FLOAT, '%'); // TODO: Is this correct?
    case OP_IADD:
        return emitBinaryOp(i->operands[0], i->operands[1], OP_TYPE_INT, '+');
    case OP_ISUB:
        return emitBinaryOp(i->operands[0], i->operands[1], OP_TYPE_INT, '-');
    case OP_IMUL:
        return emitBinaryOp(i->operands[0], i->operands[1], OP_TYPE_INT, '*');
    case OP_SDIV:
        return emitBinaryOp(i->operands[0], i->operands[1], OP_TYPE_INT, '/');
    case OP_UDIV:
        return emitBinaryOp(i->operands[0], i->operands[1], OP_TYPE_INT, '/');
    case OP_SMOD:
        return emitBinaryOp(i->operands[0], i->operands[1], OP_TYPE_INT, '%'); // TODO: Is this correct?
    case OP_UMOD:
        return emitBinaryOp(i->operands[0], i->operands[1], OP_TYPE_INT, '%'); // TODO: Is this correct?
    case OP_SREM:
        return emitBinaryOp(i->operands[0], i->operands[1], OP_TYPE_INT, '%'); // TODO: Is this correct?
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

    }

    source += idCache[funcId] + " {\n";
    for (auto* block : function->blocks) {
        source += compile(block);
    }
    source += "}\n\n";
    return source;
}

std::string Direct3D12Shader::compile(Module* module) {
    std::string source;

    // Compile module header
    for (Instruction* i : module->header) {
        if (i->opcode == OP_ENTRY_POINT) {
            Literal funcId = i->operands[1];
            idCache[funcId] = "TOutput main(TInput input)";
        }
        if (i->opcode == OP_VARIABLE) {
            Literal storageClass = i->operands[0];
            std::string type = getType(i->typeId);
            switch (storageClass) {
            case StorageClass::INPUT:
                sourceInput += format("  %s v%d : INPUT0;\n", type.c_str(), i->resultId);
                break;
            case StorageClass::OUTPUT:
                sourceOutput += format("  %s v%d : OUTPUT0;\n", type.c_str(), i->resultId);
                break;
            default:
                assert_always("Unimplemented");
            }
        }
    }
    source = format("%s\nstruct TInput {\n%s};\nstruct TOutput {\n%s};\n\n",
        sourceTypes.c_str(), sourceInput.c_str(), sourceOutput.c_str());

    // Compile functions
    for (auto* function : module->functions) {
        source += compile(function);
    }

    return source;
}

bool Direct3D12Shader::initialize(const ShaderDesc& desc) {
    // Initialization
    module = desc.module;
    idCache.resize(module->idInstructions.size());

    std::string source = compile(module);
    std::cout << source << std::endl;
    if (desc.type == SHADER_TYPE_VERTEX) {
        source = "struct TInput { float4 i0 : INPUT0; float4 i1 : INPUT1; }; struct TOutput { float4 o0 : SV_POSITION; float4 o1: COLOR; }; TOutput main(TInput input) { TOutput output; output.o0 = input.i0; output.o1 = input.i1; return output; }";
    }
    if (desc.type == SHADER_TYPE_PIXEL) {
        source = "struct TInput { float4 i0 : SV_POSITION; float4 i1: COLOR; }; float4 main(TInput input) : SV_TARGET { return input.i1; }";
    }

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

    ID3DBlob* bytecode;
    ID3DBlob* errors;
    HRESULT hr = _D3DCompile(source.data(), source.size(), "shader", nullptr, nullptr, "main", target, 0, 0, &bytecode, &errors);
    if (FAILED(hr)) {
        LPVOID errorString = errors->GetBufferPointer();
        logger.error(LOG_GPU, "Direct3D12Shader::initialize: Cannot compile shader (0x%X):\n%s", hr, errorString);
        return false;
    }

    bytecodeData = bytecode->GetBufferPointer();
    bytecodeSize = bytecode->GetBufferSize();
    return true;
}

}  // namespace direct3d12
}  // namespace gfx
