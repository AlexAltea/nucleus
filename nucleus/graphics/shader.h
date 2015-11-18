/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/hir/module.h"

namespace gfx {

enum ShaderFlags {
    SHADER_FLAGS_NONE = 0,
};

struct ShaderDesc {
    ShaderFlags flags;
    hir::Module* module;
};

class Shader {
public:
};

}  // namespace gfx
