/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

//#include <regex>
#include <string>
#include <unordered_map>

// Forward declarations
namespace gfx {
namespace hir {

class Block;
class Function;
class Module;
class Type;

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
    static std::unordered_map<std::string, int> keywordMap;

    /*static std::regex patAccepted;
    static std::regex patIgnored;
    static std::regex patSkip;
    static std::regex patSymbol;
    static std::regex patFloat;
    static std::regex patInt;*/
    static void initialize();

    // Types
    static std::unordered_map<std::string, hir::Type*> types;

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
