/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_fp.h"
#include "nucleus/format.h"
#include "nucleus/emulator.h"
#include "nucleus/gpu/rsx_fp.h"

// OpenGL dependencies
#include <GL/glew.h>

// Input register
struct fp_input_register_t {
    const char* glReg;
    const char* rsxReg;
    int rsxIndex;
    bool predefined;
};

const fp_input_register_t input_regs[] = {
    // OpenGL predefined inputs
    { "gl_Position",            "f[0]",    0, true },

    // OpenGL custom inputs
    { "rsx_BackDiffuseColor",   "f[1]",    1, false },
    { "rsx_BackSpecularColor",  "f[2]",    2, false },
    { "rsx_FrontDiffuseColor",  "f[3]",    3, false },
    { "rsx_TEX0",               "f[4]",    4, false },
    { "rsx_Fog",                "f[5]",    5, false },
    { "rsx_Texture0",           "f[6]",    6, false },
    { "rsx_Texture1",           "f[7]",    7, false },
    { "rsx_Texture2",           "f[8]",    8, false },
    { "rsx_Texture3",           "f[9]",    9, false },
    { "rsx_Texture4",           "f[10]",  10, false },
    { "rsx_Texture5",           "f[11]",  11, false },
    { "rsx_Texture6",           "f[12]",  12, false },
    { "rsx_Texture7",           "f[13]",  13, false },
    { "rsx_Texture8",           "f[14]",  14, false },
};

struct fp_output_register_t {
    const char* glReg;
    int rsxRegIndex32; // Full-width temporary register index
    int rsxRegIndex16; // Half-width temporary register index
};

const fp_output_register_t output_regs[] = {
    { "rsx_COL0",  0,  0 },
    { "rsx_COL1",  2,  4 },
    { "rsx_COL2",  3,  6 },
    { "rsx_COL3",  4,  8 },
};

// Transform the 4-bit mask of the [x,y,z,w] values into the GLSL equivalent
const char* get_fp_mask(u8 maskValue)
{
    static const char* maskString[] = {
        "",     //  0 -> 0000 [....]
        ".x",   //  1 -> 0001 [...x]
        ".y",   //  2 -> 0010 [..y.]
        ".xy",  //  3 -> 0011 [..yx]
        ".z",   //  4 -> 0100 [.z..]
        ".xz",  //  5 -> 0101 [.z.x]
        ".yz",  //  6 -> 0110 [.zy.]
        ".xyz", //  7 -> 0111 [.zyx]
        ".w",   //  8 -> 1000 [w...]
        ".xw",  //  9 -> 1001 [w..x]
        ".yw",  // 10 -> 1010 [w.y.]
        ".xyw", // 11 -> 1011 [w.yx]
        ".zw",  // 12 -> 1100 [wz..]
        ".xzw", // 13 -> 1101 [wz.x]
        ".yzw", // 14 -> 1110 [wzy.]
        ""      // 15 -> 1111 [wzyx]
    };
    return maskString[maskValue % 16];
}

std::string OpenGLFragmentProgram::get_header()
{
    std::string header = "#version 420\n";

    // Input registers
    for (const auto& reg : input_regs) {
        if (usedInputs & (1 << reg.rsxIndex) && !reg.predefined) {
            header += format("in vec4 %s;", reg.glReg);
        }
    }

    // Output registers
    for (const auto& reg : output_regs) {
        if (usedOutputs & (1 << reg.rsxRegIndex32)) {
            header += format("layout (location = %d) out vec4 %s;", reg.rsxRegIndex32, reg.glReg);
        }
    }

    // Sampler registers
    for (int i = 0; i < 16; i++) {
        if (usedSamplers & (1 << i)) {
            header += format("layout (binding = %d) uniform sampler2D tex%d;", i, i);
        }
    }

    header += "vec4 f[15];"; // Input attribute registers
    header += "vec4 r[48];"; // Temporary register: Full-width (4 x 32-bit float) register
    header += "vec4 h[48];"; // Temporary register: Half-width (4 x 16-bit float) register (TODO: Should be overlapped with the full-width registers above)
    return header;
}

std::string OpenGLFragmentProgram::get_src(u32 n)
{
    std::string src;
    const rsx_fp_instruction_source_t source = { instr.word[n+1] & 0x3FFFF };

    switch (source.type) {
    case 0: // Temporary register
        src = format("%s[%d]", source.half ? "h" : "r", source.index);
        break;

    case 1: // Input register
        usedInputs |= (1 << instr.input_index);
        src = format("f[%d]", instr.input_index);
        break;

    case 2: // Constant register
        src = format("vec4(%f, %f, %f, %f)",
            get_word<f32>(instr_ptr->word[0]),
            get_word<f32>(instr_ptr->word[1]),
            get_word<f32>(instr_ptr->word[2]),
            get_word<f32>(instr_ptr->word[3]));
        instr_ptr++;
        break;
    }

    // Swizzling
    static const char* swizzleString[] = { "x", "y", "z", "w" };
    src += ".";
    src += swizzleString[source.swizzle_x];
    src += swizzleString[source.swizzle_y];
    src += swizzleString[source.swizzle_z];
    src += swizzleString[source.swizzle_w];

    // Absolute value
    // TODO

    // Negated value
    if (source.neg) {
        src = "-" + src;
    }

    return src;
}

std::string OpenGLFragmentProgram::get_dst()
{
    if (instr.dst_index >= 48) {
        nucleus.log.error(LOG_GPU, "Fragment program: Destination register out of range");
    }
    usedOutputs |= (1 << instr.dst_index);
    return format("%s[%d]%s", instr.dst_half ? "h" : "r", instr.dst_index, get_fp_mask(instr.dst_mask));
}

std::string OpenGLFragmentProgram::get_tex()
{
    usedSamplers |= (1 << instr.tex);
    return format("tex%d", instr.tex);
}

void OpenGLFragmentProgram::decompile(rsx_fp_instruction_t* buffer)
{
#define DST()  get_dst().c_str()
#define SRC(n) get_src(n).c_str()
#define TEX()  get_tex().c_str()

    source = "";
    usedInputs = 0;
    usedOutputs = 0;
    usedSamplers = 0;

    // Set pointer to current instruction
    instr_ptr = buffer;

    // Shader body
    while (true) {
        // Set current instruction
        instr.word[0] = get_word<u32>(instr_ptr->word[0]);
        instr.word[1] = get_word<u32>(instr_ptr->word[1]);
        instr.word[2] = get_word<u32>(instr_ptr->word[2]);
        instr.word[3] = get_word<u32>(instr_ptr->word[3]);
        instr_ptr++;

        switch (instr.opcode) {
        case RSX_FP_OPCODE_NOP:
            break;
        case RSX_FP_OPCODE_MOV:
            source += format("%s = %s%s;", DST(), SRC(0), get_fp_mask(instr.dst_mask));
            break;
        case RSX_FP_OPCODE_MUL:
            source += format("%s = (%s * %s)%s;\n", DST(), SRC(0), SRC(1), get_fp_mask(instr.dst_mask));
            break;
        case RSX_FP_OPCODE_TEX:
            source += format("%s = texture(%s, %s.xy)%s;", DST(), TEX(), SRC(0), get_fp_mask(instr.dst_mask));
            break;
        case RSX_FP_OPCODE_FENCB:
            break;
        default:
            nucleus.log.error(LOG_GPU, "Fragment program: Unknown opcode (%d)", instr.opcode);
        }

        // Final instruction
        if (instr.end) {
            break;
        }
    }

    // Map OpenGL input attributes to RSX input registers
    for (const auto& reg : input_regs) {
        if (usedInputs & (1 << reg.rsxIndex)) {
            source = format("%s = %s;", reg.rsxReg, reg.glReg) + source;
        }
    }

    // Map RSX output registers to OpenGL output attributes
    for (const auto& reg : output_regs) {
        if (usedOutputs & (1 << reg.rsxRegIndex32)) {
            source += format("%s = r[%d];", reg.glReg, reg.rsxRegIndex32);
        }
    }

    // Merge header and body
    source = get_header() + "void main() { " + source + "}";

#undef DST
#undef SRC
#undef TEX
}

bool OpenGLFragmentProgram::compile()
{
    // Check if the shader was already compiled
    if (id != 0) {
        glDeleteShader(id);
    }

    id = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile shader
    const GLchar* sourceString = source.data();
    const GLint sourceLength = source.length();
    glShaderSource(id, 1, &sourceString, &sourceLength);
    glCompileShader(id);

    // Check if shader compiled succesfully
    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        nucleus.log.error(LOG_GPU, "OpenGLFragmentProgram::compile: Can't compile shader");
        return false;
    }

    return true;
}
