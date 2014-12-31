/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/gpu/rsx_vp.h"

#include <string>

class OpenGLVertexProgram
{
    // OpenGL shader source
    std::string source;

public:
    // OpenGL shader ID
    u32 id = 0;

    bool decompile(rsx_vp_instruction_t* buffer, u32 start);

    bool compile();
};
