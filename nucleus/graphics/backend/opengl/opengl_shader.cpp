/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "opengl_shader.h"
#include "nucleus/assert.h"
#include "nucleus/format.h"
#include "nucleus/graphics/hir/block.h"
#include "nucleus/graphics/hir/function.h"
#include "nucleus/graphics/hir/instruction.h"
#include "nucleus/graphics/hir/opcodes.h"

namespace gfx {

using namespace gfx::hir;

std::string OpenGLShader::getType(Type type) {
    switch (type) {
    case TYPE_I16:  return "int";
    case TYPE_I32:  return "int";
    case TYPE_F16:  return "float";
    case TYPE_F32:  return "float";
    case TYPE_V128: return "vec4";
    default:
        assert_always("Unimplemented case");
        return "UNIMPLEMENTED";
    }
}

std::string OpenGLShader::getFunctionDeclaration(S32 id, const std::vector<Type>& typeIn, Type typeOut) {
    std::string source;
    return source;
}

std::string OpenGLShader::compile(Instruction* instr) {
    std::string source;
    switch (instr->opcode) {
    case OPCODE_ADD:
        source = "v1 = v2 + v2"; break;
    }
    return source;
}

std::string OpenGLShader::compile(Block* block) {
    std::string source;
    for (auto* instruction : block->instructions) {
        source += compile(instruction);
        source += ";\n";
    }
    return source;
}

std::string OpenGLShader::compile(Function* function) {
    std::string source;

    // Function declaration
    const auto& id = function->getId();
    const auto& typeIn = function->typeIn;
    const auto& typeOut = function->typeOut;
    source += getFunctionDeclaration(id, typeIn, typeOut);

    source += "{\n";
    for (auto* block : function->blocks) {
        source += compile(block);
    }
    source += "}\n\n";
    return source;
}

std::string OpenGLShader::compile(Module* module) {
    std::string source;

    // Pipeline input and outputs
    for (auto* value : module->inputs) {
        std::string typeName = getType(value->type);
        source += format("in %s i%d;\n", typeName.c_str(), value->getId());
    }
    for (auto* value : module->outputs) {
        std::string typeName = getType(value->type);
        source += format("out %s o%d;\n", typeName.c_str(), value->getId());
    }

    // Compile functions
    for (auto* function : module->functions) {
        source += compile(function);
    }
    return source;
}

bool OpenGLShader::initialize(const ShaderDesc& desc) {
    return true;
}

}  // namespace gfx
