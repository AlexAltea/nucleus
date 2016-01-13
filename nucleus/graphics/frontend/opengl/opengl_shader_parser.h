/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/hir/builder.h"
#include "nucleus/graphics/hir/hir.h"

#include <string>
#include <unordered_map>

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
namespace opengl {

/**
 * OpenGL Shader Parser
 * ====================
 * Takes GLSL 4.50 source code as input and translates it to a module containing
 * the High-level Intermediate Representation of the given shader.
 */
class OpenGLShaderParser {
    hir::Builder builder;

private:
    hir::Module* module;

    // Types
    static std::unordered_map<std::string, hir::Literal> types;
    hir::Literal getOrInsertType(const std::string& typeName);

    // Preprocessor
    std::unordered_map<std::string, std::string> defines;
    std::string preprocess(const std::string& source);

public:
    int nextToken();

    hir::Module* parse(const std::string& source);
};

}  // namespace opengl
}  // namespace frontend
}  // namespace gfx
