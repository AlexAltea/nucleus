/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/hir/module.h"

namespace gfx {

enum ShaderType {
    SHADER_TYPE_VERTEX,
    SHADER_TYPE_HULL,
    SHADER_TYPE_DOMAIN,
    SHADER_TYPE_GEOMETRY,
    SHADER_TYPE_PIXEL,
};

enum ShaderFlags {
    SHADER_FLAGS_NONE = 0,
};

struct ShaderDesc {
    ShaderType type;
    ShaderFlags flags;
    hir::Module* module;
};

class Shader {
public:
};

}  // namespace gfx
