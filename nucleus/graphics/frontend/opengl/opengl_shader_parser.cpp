/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_shader_parser.h"
#include "nucleus/graphics/hir/builder.h"
#include "nucleus/graphics/hir/hir.h"
#include "nucleus/graphics/hir/module.h"
#include "nucleus/graphics/frontend/opengl/glsl_parser.y.hpp"

using namespace gfx::hir;

int yylex(gfx::frontend::opengl::OpenGLShaderParser& parser) {
    return 0;
}

namespace gfx {
namespace frontend {
namespace opengl {

Literal OpenGLShaderParser::getOrInsertType(const std::string& typeName) {
    // Reuse type if possible
    Literal type;
    if (types.find(typeName) != types.end()) {
        return types.at(typeName);
    }

    // Basic types
    if (typeName == "void") {
        type = builder.opTypeVoid();
    } else if (typeName == "bool") {
        type = builder.opTypeBool();
    } else if (typeName == "int") {
        type = builder.opTypeInt(32, true);
    } else if (typeName == "uint") {
        type = builder.opTypeInt(32, false);
    } else if (typeName == "float") {
        type = builder.opTypeFloat(32);
    } else if (typeName == "double") {
        type = builder.opTypeFloat(64);
    }

    // Vector types
    if (typeName.compare(0, 4, "bvec")) {
        type = builder.opTypeVector(32, true);
    }
}

std::string OpenGLShaderParser::preprocess(const std::string& source) {
    // TODO: Pre-processor not supported
    return source;
}

Module* OpenGLShaderParser::parse(const std::string& source) {
    module = new Module();
    builder.setModule(module);

    std::string code = preprocess(source);
    yyparse(*this);

    return nullptr;
}

}  // namespace opengl
}  // namespace frontend
}  // namespace gfx
