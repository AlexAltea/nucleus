/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_vp.h"
#include "nucleus/format.h"
#include "nucleus/emulator.h"
#include "nucleus/gpu/rsx_vp.h"

// OpenGL dependencies
#include "nucleus/opengl.h"

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
    { "rsx_TEX0",               "o[7]",    7, false },
    { "rsx_TEX1",               "o[8]",    8, false },
    { "rsx_TEX2",               "o[9]",    9, false },
    { "rsx_TEX3",               "o[10]",  10, false },
    { "rsx_TEX4",               "o[11]",  11, false },
    { "rsx_TEX5",               "o[12]",  12, false },
    { "rsx_TEX6",               "o[13]",  13, false },
    { "rsx_TEX7",               "o[14]",  14, false },
    { "rsx_TEX8",               "o[15]",  15, false },
    { "rsx_TEX9",               "o[6]",    6, false },
};

// Transform the 4-bit mask of the [x,y,z,w] values into the GLSL equivalent
const char* get_vp_mask(u8 maskValue)
{
    static const char* maskString[] = {
        "",     //  0 -> 0000 [....]
        ".w",   //  1 -> 0001 [...w]
        ".z",   //  2 -> 0010 [..z.]
        ".zw",  //  3 -> 0011 [..zw]
        ".y",   //  4 -> 0100 [.y..]
        ".yw",  //  5 -> 0101 [.y.w]
        ".yz",  //  6 -> 0110 [.yz.]
        ".yzw", //  7 -> 0111 [.yzw]
        ".x",   //  8 -> 1000 [x...]
        ".xw",  //  9 -> 1001 [x..w]
        ".xz",  // 10 -> 1010 [x.z.]
        ".xzw", // 11 -> 1011 [x.zw]
        ".xy",  // 12 -> 1100 [xy..]
        ".xyw", // 13 -> 1101 [xy.w]
        ".xyz", // 14 -> 1110 [xyz.]
        ""      // 15 -> 1111 [xyzw]
    };
    return maskString[maskValue % 16];
}

// Transform a the 2-bit swizzle masks for [x,y,z,w] packed as a u8 into the GLSL equivalent
std::string get_vp_swizzling(u8 swizzleValue)
{
    static const char* swizzleString[] = {
        "x", "y", "z", "w"
    };

    // Check if swizzling is required (0x1B -> 0b00011011 -> [0,1,2,3] -> ".xyzw")
    if (swizzleValue == 0x1B) {
        return "";
    }

    std::string swizzling = ".";
    swizzling += swizzleString[(swizzleValue >> 6) & 0x3];
    swizzling += swizzleString[(swizzleValue >> 4) & 0x3];
    swizzling += swizzleString[(swizzleValue >> 2) & 0x3];
    swizzling += swizzleString[(swizzleValue >> 0) & 0x3];
    return swizzling;
}

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
        src = format("r[%d]", source.index);
        break;

    case 2: // Input register
        usedInputs |= (1 << instr.src_input);
        src = format("v%d", instr.src_input);
        break;

    case 3: // Constant register
        src = format("c[%d]", instr.src_const);
        break;
    }

    // Swizzling
    src += get_vp_swizzling(source.swizzling);

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
    std::string dst;

    // Data register
    if (instr.dst == 0x1F) {
        dst = format("r[%d]", instr.dst_data);
    }

    // Output register
    else {
        if (instr.dst >= 16) {
            nucleus.log.error(LOG_GPU, "VPE: Destination register out of range");
        }
        usedOutputs |= (1 << instr.dst);
        dst = format("o[%d]", instr.dst);
    }

    return dst + get_vp_mask(instr.masc_vec);
}

void OpenGLVertexProgram::decompile(rsx_vp_instruction_t* buffer)
{
#define DST()  get_dst().c_str()
#define SRC(n) get_src(n).c_str()

    source = "";
    usedInputs = 0;
    usedOutputs = 0;

    // Shader body
    do {
        instr = *buffer;

        // SCA Opcodes
        switch(instr.opcode_sca) {
        case RSX_VP_OPCODE_SCA_NOP:
            break;
        default:
            nucleus.log.error(LOG_GPU, "VPE: Unknown SCA opcode (%d)", instr.opcode_sca);
        }

        // VEC Opcodes
        switch(instr.opcode_vec) {
        case RSX_VP_OPCODE_VEC_NOP:
            break;
        case RSX_VP_OPCODE_VEC_MOV:
            source += format("%s = %s%s;\n", DST(), SRC(0), get_vp_mask(instr.masc_vec));
            break;
        case RSX_VP_OPCODE_VEC_MUL:
            source += format("%s = (%s * %s)%s;\n", DST(), SRC(0), SRC(1), get_vp_mask(instr.masc_vec));
            break;
        case RSX_VP_OPCODE_VEC_MAD:
            source += format("%s = ((%s * %s) + %s)%s;\n", DST(), SRC(0), SRC(1), SRC(2), get_vp_mask(instr.masc_vec));
            break;
        case RSX_VP_OPCODE_VEC_FRC:
            source += format("%s = fract(%s)%s;", DST(), SRC(0), get_vp_mask(instr.masc_vec));
            break;
        case RSX_VP_OPCODE_VEC_FLR:
            source += format("%s = floor(%s)%s;", DST(), SRC(0), get_vp_mask(instr.masc_vec));
            break;
        case RSX_VP_OPCODE_VEC_DP4:
            source += format("%s = vec4(dot(%s, %s))%s;\n", DST(), SRC(0), SRC(1), get_vp_mask(instr.masc_vec));
            break;
        default:
            nucleus.log.error(LOG_GPU, "VPE: Unknown VEC opcode (%d)", instr.opcode_vec);
        }
    } while (!(buffer++)->end);

    // Map RSX output registers to shader output vectors
    for (const auto& reg : output_regs) {
        if (usedOutputs & (1 << reg.rsxIndex)) {
            source += format("%s = %s;", reg.glReg, reg.rsxReg);
        }
    }

    // Merge header and body
    source = get_header() + "void main() { " + source + "}";

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
