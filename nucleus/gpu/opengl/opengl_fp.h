/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/gpu/rsx_fp.h"

#include <string>

class OpenGLFragmentProgram
{
    // OpenGL shader source
    std::string m_shader;

public:
    // OpenGL shader ID
    u32 m_id;

    bool decompile();

    bool compile();
};
