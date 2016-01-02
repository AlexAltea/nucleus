/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "shader_parser.h"
#include "nucleus/graphics/hir/module.h"
#include "nucleus/graphics/frontend/opengl/opengl_shader_parser.h"

using namespace gfx::hir;

namespace gfx {
namespace frontend {

Module* ShaderParser::parse(const char* data, size_t size) {
    std::string source(data, size);

    // NOTE: Assuming the input shader is designed for the only available frontend
    opengl::OpenGLShaderParser parser;
    return parser.parse(source);
}

}  // namespace frontend
}  // namespace gfx
