/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/gpu/rsx_fp.h"

#include <string>

class OpenGLFragmentProgram
{
    // OpenGL shader source
    std::string source;

public:
    // OpenGL shader ID
    u32 id = 0;

    bool decompile();

    bool compile();
};
