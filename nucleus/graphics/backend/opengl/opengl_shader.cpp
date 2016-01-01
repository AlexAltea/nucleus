/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
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
namespace opengl {

using namespace gfx::hir;

// Conversion
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

const char* OpenGLShader::getBuiltin(ValueBuiltin builtin) {
    switch (builtin) {
    case VALUE_BUILTIN_POSITION:      return "gl_Position";
    case VALUE_BUILTIN_CLIPDISTANCE:  return "gl_ClipDistance";
    case VALUE_BUILTIN_CULLDISTANCE:  return "gl_CullDistance";
    case VALUE_BUILTIN_FRAGCOORD:     return "gl_FragCoord";
    case VALUE_BUILTIN_FRAGDEPTH:     return "gl_FragDepth";
    case VALUE_BUILTIN_POINTCOORD:    return "gl_PointCoord";
    case VALUE_BUILTIN_POINTSIZE:     return "gl_PointSize";
    default:
        assert_always("Unimplemented case");
        return "UNIMPLEMENTED";
    }
}

// Emitters
std::string OpenGLShader::getDst(Value* value) {
    if (value->flags & (VALUE_IS_INPUT | VALUE_IS_OUTPUT | VALUE_IS_BUILTIN)) {
        return getName(value);
    } else {
        return getDeclaration(value);
    }
}

std::string OpenGLShader::getSrc(Value* value) {
    if (value->flags & VALUE_IS_CONSTANT) {
        return getConstant(value);
    } else {
        return getName(value);
    }
}

std::string OpenGLShader::emitOp(const char* fmt) {
    return fmt;
}

std::string OpenGLShader::emitOp(const char* fmt, hir::Value* v0) {
    std::string v0Name = getDst(v0);
    return format(fmt, v0Name.c_str());
}

std::string OpenGLShader::emitOp(const char* fmt, hir::Value* v0, hir::Value* v1) {
    std::string v0Name = getDst(v0);
    std::string v1Name = getSrc(v1);
    return format(fmt, v0Name.c_str(), v1Name.c_str());
}

std::string OpenGLShader::emitOp(const char* fmt, hir::Value* v0, hir::Value* v1, hir::Value* v2) {
    std::string v0Name = getDst(v0);
    std::string v1Name = getSrc(v1);
    std::string v2Name = getSrc(v2);
    return format(fmt, v0Name.c_str(), v1Name.c_str(), v2Name.c_str());
}

// Utilities
std::string OpenGLShader::getConstant(Value* value) {
    Value::Constant& c = value->constant;
    switch (value->type) {
    case TYPE_F32:
        return format("float(%f)", c.f32);
    case TYPE_V128:
        return format("vec4(%f, %f, %f, %f)", c.v128.f32[0], c.v128.f32[1], c.v128.f32[2], c.v128.f32[3]);
    default:
        assert_always("Unimplemented case");
        return "UNIMPLEMENTED";
    }
}

std::string OpenGLShader::getName(hir::Value* value) {
    const S32 valueId = value->getId();
    if (value->flags & VALUE_IS_ARGUMENT) {
        return format("a%d", valueId);
    } else if (value->flags & VALUE_IS_INPUT) {
        return format("i%d", valueId);
    } else if (value->flags & VALUE_IS_OUTPUT) {
        return format("o%d", valueId);
    } else if (value->flags & VALUE_IS_BUILTIN) {
        return getBuiltin(value->builtin);
    } else {
        return format("v%d", valueId);
    }
}

std::string OpenGLShader::getName(hir::Function* function) {
    if (function->flags & FUNCTION_IS_ENTRY) {
        return "main";
    } else {
        return format("func%d", function->getId());
    }
}

std::string OpenGLShader::getDeclaration(Value* value) {
    std::string valueType = getType(value->type);
    std::string valueName = getName(value);
    return format("%s %s", valueType.c_str(), valueName.c_str());
}

std::string OpenGLShader::getDeclaration(Function* function) {
    std::string source;

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
}

std::string OpenGLShader::compile(Instruction* i) {
    std::string source;
    switch (i->opcode) {
    case OPCODE_ADD:
        source = emitOp("%s = %s + %s", i->dest, i->src1.value, i->src2.value); break;
    case OPCODE_SUB:
        source = emitOp("%s = %s - %s", i->dest, i->src1.value, i->src2.value); break;
    case OPCODE_MUL:
        source = emitOp("%s = %s * %s", i->dest, i->src1.value, i->src2.value); break;
    case OPCODE_DIV:
        source = emitOp("%s = %s / %s", i->dest, i->src1.value, i->src2.value); break;
    case OPCODE_COS:
        source = emitOp("%s = cos(%s)", i->dest, i->src1.value); break;
    case OPCODE_SIN:
        source = emitOp("%s = sin(%s)", i->dest, i->src1.value); break;
    case OPCODE_LOAD:
        source = emitOp("%s = %s", i->dest, i->src1.value); break;
    case OPCODE_STORE:
        source = emitOp("%s = %s", i->src1.value, i->src2.value); break;
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

    // Shader input and outputs
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

}  // namespace opengl
}  // namespace gfx
