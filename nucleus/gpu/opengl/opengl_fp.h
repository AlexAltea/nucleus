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

    // Input/Output registers used in the program
    u32 usedInputs;
    u64 usedOutputs;

    // Current instruction being processed
    rsx_fp_instruction_t instr;

    // Generate the GLSL header and register declarations based on the decompilation
    std::string get_header();

    // Get the source and destination registers of the current instruction
    std::string get_src(u32 n);
    std::string get_dst();

public:
    // OpenGL shader ID
    u32 id = 0;

    // Generate a GLSL fragment shader equivalent to the given buffer
    bool decompile(rsx_fp_instruction_t* buffer);

    // Compile the generated GLSL code for the host GPU
    bool compile();
};
