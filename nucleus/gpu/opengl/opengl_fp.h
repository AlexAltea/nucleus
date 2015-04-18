/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/gpu/rsx_fp.h"

#include <string>

namespace gpu {

class OpenGLFragmentProgram
{
    // OpenGL shader source
    std::string source;

    // Input/Output/Sampler registers used in the program
    u32 usedInputs;
    u64 usedOutputs;
    u32 usedSamplers;

    // Current instruction being processed and pointer to the next instruction
    rsx_fp_instruction_t instr;
    rsx_fp_instruction_t* instr_ptr;

    // Generate the GLSL header and register declarations based on the decompilation
    std::string get_header();

    // Get the source, destination and sampler registers of the current instruction
    std::string get_src(u32 n);
    std::string get_dst();
    std::string get_tex();

    // Get 32-bit of data reversing its byte and half-word endianness
    template <typename T>
    T get_word(u32 word) {
        u32 result = (re32(word) >> 16) | (re32(word) << 16);
        return (T&)result;
    }

public:
    // OpenGL shader ID
    u32 id = 0;

    // Generate a GLSL fragment shader equivalent to the given buffer
    void decompile(rsx_fp_instruction_t* buffer);

    // Compile the generated GLSL code for the host GPU
    bool compile();
};

}  // namespace gpu
