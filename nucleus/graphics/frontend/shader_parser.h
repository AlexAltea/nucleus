/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

// Forward declarations
namespace gfx {
namespace hir {
class Module;
}  // namespace hir
}  // namespace gfx

namespace gfx {
namespace frontend {

/**
 * Shader Parser
 * =============
 * Takes an arbitrary shader source code as input and translates it to a module containing
 * the High-level Intermediate Representation, inferring the appropriate frontend to use.
 */
class ShaderParser {
public:
    static hir::Module* parse(const char* data, Size size);
};

}  // namespace frontend
}  // namespace gfx
