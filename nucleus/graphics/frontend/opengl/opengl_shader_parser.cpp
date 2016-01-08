/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_shader_parser.h"
#include "nucleus/graphics/hir/builder.h"
#include "nucleus/graphics/hir/module.h"
#include "nucleus/graphics/frontend/opengl/glsl_parser.tab.hpp"

using namespace gfx::hir;

namespace gfx {
namespace frontend {
namespace opengl {

int yylex(const GLSLParser::semantic_type* token, OpenGLShaderParser& parser) {
    return parser.nextToken();
}

// Static members
std::unordered_map<std::string, int> OpenGLShaderParser::keywordMap;

void OpenGLShaderParser::initialize() {
    static bool initialized = false;
    if (initialized) {
        return;
    }

    keywordMap["const"]       = GLSLParser::token::CONST;
    keywordMap["uniform"]     = GLSLParser::token::UNIFORM;
    keywordMap["in"]          = GLSLParser::token::IN;
    keywordMap["out"]         = GLSLParser::token::OUT;
    keywordMap["inout"]       = GLSLParser::token::INOUT;
    keywordMap["struct"]      = GLSLParser::token::STRUCT;
    keywordMap["break"]       = GLSLParser::token::BREAK;
    keywordMap["continue"]    = GLSLParser::token::CONTINUE;
    keywordMap["do"]          = GLSLParser::token::DO;
    keywordMap["for"]         = GLSLParser::token::FOR;
    keywordMap["while"]       = GLSLParser::token::WHILE;
    keywordMap["switch"]      = GLSLParser::token::SWITCH;
    keywordMap["case"]        = GLSLParser::token::CASE;
    keywordMap["default"]     = GLSLParser::token::DEFAULT;
    keywordMap["if"]          = GLSLParser::token::IF;
    keywordMap["else"]        = GLSLParser::token::ELSE;
    keywordMap["discard"]     = GLSLParser::token::DISCARD;
    keywordMap["return"]      = GLSLParser::token::RETURN;
    //keywordMap["true"]      = GLSLParser::token::BOOLCONSTANT;
    //keywordMap["false"]     = GLSLParser::token::BOOLCONSTANT;
    keywordMap["attribute"]   = GLSLParser::token::ATTRIBUTE;
    keywordMap["varying"]     = GLSLParser::token::VARYING;
    keywordMap["buffer"]      = GLSLParser::token::BUFFER;
    keywordMap["coherent"]    = GLSLParser::token::COHERENT;
    keywordMap["restrict"]    = GLSLParser::token::RESTRICT;
    keywordMap["readonly"]    = GLSLParser::token::READONLY;
    keywordMap["writeonly"]   = GLSLParser::token::WRITEONLY;
    keywordMap["atomic_uint"] = GLSLParser::token::ATOMIC_UINT;
    keywordMap["volatile"]    = GLSLParser::token::VOLATILE;
    keywordMap["layout"]      = GLSLParser::token::LAYOUT;
    keywordMap["shared"]      = GLSLParser::token::SHARED;
    keywordMap["patch"]       = GLSLParser::token::PATCH;
    keywordMap["sample"]      = GLSLParser::token::SAMPLE;
    keywordMap["subroutine"]  = GLSLParser::token::SUBROUTINE;

    // Primitive types
    keywordMap["bool"]     = GLSLParser::token::BOOL;
    keywordMap["int"]      = GLSLParser::token::INT;
    keywordMap["uint"]     = GLSLParser::token::UINT;
    keywordMap["float"]    = GLSLParser::token::FLOAT;
    keywordMap["double"]   = GLSLParser::token::DOUBLE;
    keywordMap["bvec2"]    = GLSLParser::token::BVEC2;
    keywordMap["bvec3"]    = GLSLParser::token::BVEC3;
    keywordMap["bvec4"]    = GLSLParser::token::BVEC4;
    keywordMap["ivec2"]    = GLSLParser::token::IVEC2;
    keywordMap["ivec3"]    = GLSLParser::token::IVEC3;
    keywordMap["ivec4"]    = GLSLParser::token::IVEC4;
    keywordMap["uvec2"]    = GLSLParser::token::UVEC2;
    keywordMap["uvec3"]    = GLSLParser::token::UVEC3;
    keywordMap["uvec4"]    = GLSLParser::token::UVEC4;
    keywordMap["vec2"]     = GLSLParser::token::VEC2;
    keywordMap["vec3"]     = GLSLParser::token::VEC3;
    keywordMap["vec4"]     = GLSLParser::token::VEC4;
    keywordMap["dvec2"]    = GLSLParser::token::DVEC2;
    keywordMap["dvec3"]    = GLSLParser::token::DVEC3;
    keywordMap["dvec4"]    = GLSLParser::token::DVEC4;
    keywordMap["mat2"]     = GLSLParser::token::MAT2;
    keywordMap["mat3"]     = GLSLParser::token::MAT3;
    keywordMap["mat4"]     = GLSLParser::token::MAT4;
    keywordMap["mat2x2"]   = GLSLParser::token::MAT2X2;
    keywordMap["mat2x3"]   = GLSLParser::token::MAT2X3;
    keywordMap["mat2x4"]   = GLSLParser::token::MAT2X4;
    keywordMap["mat3x2"]   = GLSLParser::token::MAT3X2;
    keywordMap["mat3x3"]   = GLSLParser::token::MAT3X3;
    keywordMap["mat3x4"]   = GLSLParser::token::MAT3X4;
    keywordMap["mat4x2"]   = GLSLParser::token::MAT4X2;
    keywordMap["mat4x3"]   = GLSLParser::token::MAT4X3;
    keywordMap["mat4x4"]   = GLSLParser::token::MAT4X4;
    keywordMap["dmat2"]    = GLSLParser::token::DMAT2;
    keywordMap["dmat3"]    = GLSLParser::token::DMAT3;
    keywordMap["dmat4"]    = GLSLParser::token::DMAT4;
    keywordMap["dmat2x2"]  = GLSLParser::token::DMAT2X2;
    keywordMap["dmat2x3"]  = GLSLParser::token::DMAT2X3;
    keywordMap["dmat2x4"]  = GLSLParser::token::DMAT2X4;
    keywordMap["dmat3x2"]  = GLSLParser::token::DMAT3X2;
    keywordMap["dmat3x3"]  = GLSLParser::token::DMAT3X3;
    keywordMap["dmat3x4"]  = GLSLParser::token::DMAT3X4;
    keywordMap["dmat4x2"]  = GLSLParser::token::DMAT4X2;
    keywordMap["dmat4x3"]  = GLSLParser::token::DMAT4X3;
    keywordMap["dmat4x4"]  = GLSLParser::token::DMAT4X4;
    /*
    patAccepted.assign("[0-9A-Za-z\.\+\-\/\*\%\[\]\(\)_<>{}|^&~=:;,!?]", std::regex::optimize);
    patIgnored.assign("[\t\r\n ]", std::regex::optimize);
    patInt.assign("[-+]0[xX][0-9a-fA-F]+", std::regex::optimize);*/

    initialized = true;
}

Type* OpenGLShaderParser::getOrInsertType(const std::string& typeName) {
    // Reuse type if possible
    Type* type;
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
    if (typeName.compare(0, 4, "bvec") {
        type = builder.opTypeVector(32, true);
    }
}

std::string OpenGLShaderParser::preprocess(const std::string& source) {
    // TODO: Pre-processor not supported
    return source;
}

int OpenGLShaderParser::nextToken() {

}

Module* OpenGLShaderParser::parse(const std::string& source) {
    module = new Module();
    builder.setModule(module);


    std::string code = preprocess(source);
    
    GLSLParser parser(*this);
    parser.parse();

    return nullptr;
}

}  // namespace opengl
}  // namespace frontend
}  // namespace gfx
