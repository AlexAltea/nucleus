/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_vp.h"
#include "nucleus/format.h"
#include "nucleus/emulator.h"
#include "nucleus/gpu/rsx_vp.h"
#include "nucleus/gpu/opengl/opengl_program.h"

// OpenGL dependencies
#include <GL/glew.h>

// Output register
struct vp_output_register_t {
    const char* glReg;
    const char* rsxReg;
    int rsxIndex;
    bool predefined;
};

const vp_output_register_t output_regs[] = {
    // OpenGL predefined outputs
    { "gl_Position",            "o[0]",    0, true },
    { "gl_PointSize",           "o[6].x",  6, true },
    { "gl_ClipDistance[0]",     "o[5].y",  5, true },
    { "gl_ClipDistance[1]",     "o[5].z",  5, true },
    { "gl_ClipDistance[2]",     "o[5].w",  5, true },
    { "gl_ClipDistance[3]",     "o[6].y",  6, true },
    { "gl_ClipDistance[4]",     "o[6].z",  6, true },
    { "gl_ClipDistance[5]",     "o[6].w",  6, true },

    // OpenGL custom outputs
    { "rsx_BackDiffuseColor",   "o[1]",    1, false },
    { "rsx_BackSpecularColor",  "o[2]",    2, false },
    { "rsx_FrontDiffuseColor",  "o[3]",    3, false },
    { "rsx_FrontSpecularColor", "o[4]",    4, false },
    { "rsx_Fog",                "o[5].x",  5, false },
    { "rsx_Texture0",           "o[7]",    7, false },
    { "rsx_Texture1",           "o[8]",    8, false },
    { "rsx_Texture2",           "o[9]",    9, false },
    { "rsx_Texture3",           "o[10]",  10, false },
    { "rsx_Texture4",           "o[11]",  11, false },
    { "rsx_Texture5",           "o[12]",  12, false },
    { "rsx_Texture6",           "o[13]",  13, false },
    { "rsx_Texture7",           "o[14]",  14, false },
    { "rsx_Texture8",           "o[15]",  15, false },
    { "rsx_Texture9",           "o[6]",    6, false },
};

std::string OpenGLVertexProgram::get_header()
{
    std::string header = "#version 330\n";

    // Input registers
    for (u32 i = 0; i < 16; i++) {
        if (usedInputs & (1 << i)) {
            header += format("layout (location = %d) in vec4 v%d;", i, i);
        }
    }

    // Output registers
    for (const auto& reg : output_regs) {
        if (usedOutputs & (1 << reg.rsxIndex) && !reg.predefined) {
            header += format("out vec4 %s;", reg.glReg);
        }
    }

    header += "vec4 o[16];";           // Output registers
    header += "vec4 r[48];";           // Data registers
    header += "uniform vec4 c[468];";  // Constant registers
    return header;
}

std::string OpenGLVertexProgram::get_src(u32 n)
{
    std::string src;
    rsx_vp_instruction_source_t source;

    // Get source vector
    switch (n) {
    case 0: source.value = (instr.src0_hi << 9) | instr.src0_lo; break;
    case 1: source.value = instr.src1; break;
    case 2: source.value = (instr.src2_hi << 11) | instr.src2_lo; break;
    }

    switch (source.type) {
    case 1: // Data register
        src = format("r[%d]", source.index) + get_swizzling(source.swizzling);
        break;

    case 2: // Input register
        usedInputs |= (1 << instr.src_input);
        src = format("v%d", instr.src_input) + get_swizzling(source.swizzling);
        break;

    case 3: // Constant register
        src = format("c[%d]", instr.src_const) + get_swizzling(source.swizzling);
        break;
    }

    // Absolute value
    switch (n) {
    case 0: src = instr.abs_src0 ? "abs(" + src + ")" : src;
    case 1: src = instr.abs_src1 ? "abs(" + src + ")" : src;
    case 2: src = instr.abs_src2 ? "abs(" + src + ")" : src;
    }

    // Negated value
    if (source.neg) {
        src = "-" + src;
    }

    return src;
}

std::string OpenGLVertexProgram::get_dst()
{
    if (instr.dst >= 16) {
        nucleus.log.error(LOG_GPU, "VPE: Destination register out of range");
    }
    usedOutputs |= (1 << instr.dst);
    return format("o[%d]%s", instr.dst, get_mask(instr.masc_vec));
}

bool OpenGLVertexProgram::decompile(rsx_vp_instruction_t* buffer, u32 start)
{
#define DST()  get_dst().c_str()
#define SRC(n) get_src(n).c_str()

    source = "";
    usedInputs = 0;
    usedOutputs = 0;

    // Shader body
    for (u32 i = start; i < 512; i++) {
        instr = buffer[i];

        // SCA Opcodes
        switch(instr.opcode_sca) {
        case RSX_VP_OPCODE_SCA_NOP:
            break;
        default:
            nucleus.log.error(LOG_GPU, "VPE: Unknown SCA opcode (%d)", instr.opcode_sca);
            return false;
        }

        // VEC Opcodes
        switch(instr.opcode_vec) {
        case RSX_VP_OPCODE_VEC_NOP:
            break;
        case RSX_VP_OPCODE_VEC_MOV:
            source += format("%s = %s;\n", DST(), SRC(0));
            break;
        case RSX_VP_OPCODE_VEC_DP4:
            source += format("%s = vec4(dot(%s, %s));\n", DST(), SRC(0), SRC(1));
            break;
        default:
            nucleus.log.error(LOG_GPU, "VPE: Unknown VEC opcode (%d)", instr.opcode_vec);
            return false;
        }

        // Final instruction
        if (instr.end) {
            break;
        }
    }

    // Map RSX output registers to shader output vectors
    for (const auto& reg : output_regs) {
        if (usedOutputs & (1 << reg.rsxIndex)) {
            source += format("%s = %s;", reg.glReg, reg.rsxReg);
        }
    }

    // Merge header and body
    source = get_header() + "void main() { " + source + "}";
    return true;

#undef DST
#undef SRC
}

bool OpenGLVertexProgram::compile()
{
    // Check if the shader was already compiled
    if (id != 0) {
        glDeleteShader(id);
    }

    id = glCreateShader(GL_VERTEX_SHADER);

    // Compile shader
    const GLchar* sourceString = source.data();
    const GLint sourceLength = source.length();
    glShaderSource(id, 1, &sourceString, &sourceLength);
    glCompileShader(id);

    // Check if shader compiled succesfully
    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        nucleus.log.error(LOG_GPU, "OpenGLVertexProgram::compile: Can't compile shader");
        return false;
    }

    return true;
}
