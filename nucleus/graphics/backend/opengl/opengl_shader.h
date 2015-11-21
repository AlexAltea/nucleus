/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/shader.h"
#include "nucleus/graphics/backend/opengl/opengl.h"
#include "nucleus/graphics/hir/type.h"
#include "nucleus/graphics/hir/value.h"

#include <vector>

namespace gfx {

// Forward declarations
namespace hir {
class Instruction;
class Block;
class Function;
class Module;
}  // namespace hir

class OpenGLShader : public Shader {
    // Conversion
    const char* getType(hir::Type type);
    const char* getBuiltin(hir::ValueBuiltin builtin);

    // Emitters
    std::string getDst(hir::Value* value);
    std::string getSrc(hir::Value* value);
    std::string emitOp(const char* fmt);
    std::string emitOp(const char* fmt, hir::Value* v0);
    std::string emitOp(const char* fmt, hir::Value* v0, hir::Value* v1);
    std::string emitOp(const char* fmt, hir::Value* v0, hir::Value* v1, hir::Value* v2);

    // Utilities
    std::string getConstant(hir::Value* constant);
    std::string getName(hir::Value* value);
    std::string getName(hir::Function* function);
    std::string getDeclaration(hir::Value* value);
    std::string getDeclaration(hir::Function* function);

    // Compile HIR components into GLSL
    std::string compile(hir::Instruction* i);
    std::string compile(hir::Block* block);
    std::string compile(hir::Function* function);
    std::string compile(hir::Module* module);

public:
    GLuint id;

    bool initialize(const ShaderDesc& desc);
};

}  // namespace gfx
