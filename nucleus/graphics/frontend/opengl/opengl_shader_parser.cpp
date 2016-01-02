/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_shader_parser.h"
#include "nucleus/graphics/hir/module.h"

using namespace gfx::hir;

namespace gfx {
namespace frontend {
namespace opengl {

Module* OpenGLShaderParser::parse(const std::string& source) {
    size_t i = 0;

    // TODO: Pre-processor not supported
    preprocessedSource = source;

    // 
    while (i < source.size()) {
        char c = source[i];

        if (c == '#') {
            throw std::runtime_error("Unexpected preprocessor directive");
        }
    }
    return nullptr;
}

}  // namespace opengl
}  // namespace frontend
}  // namespace gfx
