/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "opengl_shader.h"
#include "nucleus/assert.h"
#include "nucleus/format.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/hir/block.h"
#include "nucleus/graphics/hir/function.h"
#include "nucleus/graphics/hir/instruction.h"
#include "nucleus/graphics/hir/opcodes.h"

#include <iostream>

namespace gfx {

using namespace gfx::hir;

const char* OpenGLShader::getType(Type type) {
    switch (type) {
    case TYPE_VOID: return "void";
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

std::string OpenGLShader::getDeclaration(Value* value) {
    std::string source;
    if (value->flags & VALUE_IS_ARGUMENT) {
        return format("%s a%d", getType(value->type), value->getId());
    } else if (value->flags & VALUE_IS_INPUT) {
        return format("%s i%d", getType(value->type), value->getId());
    } else if (value->flags & VALUE_IS_OUTPUT) {
        return format("%s o%d", getType(value->type), value->getId());
    } else {
        return format("%s v%d", getType(value->type), value->getId());
    }
    return source;
}

std::string OpenGLShader::getDeclaration(Function* function) {
    std::string source;
    source += getType(function->typeOut);

    // Function name
    if (function->flags & FUNCTION_IS_ENTRY) {
        source += " main(";
    } else {
        source += format(" func%d(", id);
    }

    // Function arguments
    for (auto* value : function->args) {
        source += getDeclaration(value);
        if (value->getId() != function->args.size() - 1) {
            source += format(", ");
        }
    }

    source += ")";
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
    source += getDeclaration(function);

    source += " {\n";
    for (auto* block : function->blocks) {
        source += compile(block);
    }
    source += "}\n\n";
    return source;
}

std::string OpenGLShader::compile(Module* module) {
    std::string source;

    // Version
    source += "#version 450\n";

    // Pipeline input and outputs
    for (auto* value : module->inputs) {
        source += format("in %s i%d;\n", getType(value->type), value->getId());
    }
    for (auto* value : module->outputs) {
        source += format("out %s o%d;\n", getType(value->type), value->getId());
    }

    // Compile functions
    for (auto* function : module->functions) {
        source += compile(function);
    }
    return source;
}

bool OpenGLShader::initialize(const ShaderDesc& desc) {
    const std::string source = compile(desc.module);
    std::cout << source.c_str() << std::endl;

    // Compile shader
    const GLchar* sourceString = source.data();
    const GLint sourceLength = source.length();
    glShaderSource(id, 1, &sourceString, &sourceLength);
    glCompileShader(id);

    // Check if shader compiled succesfully
    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GLint length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        
        std::vector<GLchar> infoLog(length);
        glGetShaderInfoLog(id, infoLog.size(), nullptr, infoLog.data());
        logger.error(LOG_GPU, "OpenGLShader::initialize: Cannot compile shader:\n%s", infoLog.data());
        return false;
    }
    return true;
}

}  // namespace gfx
