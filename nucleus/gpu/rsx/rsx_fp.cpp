/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "rsx_fp.h"
#include "nucleus/format.h"
#include "nucleus/logger/logger.h"

using namespace gfx::hir;

namespace gpu {

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

Literal RSXFragmentProgram::getMaskedValue(Literal dest, Literal source, U8 mask) {
    // Check if mask enables all components
    if (mask == 0b1111) {
        return source;
    }
    // Check if mask disables all components
    if (mask == 0b0000) {
        return dest;
    }
    Literal x = (mask >> 3) ? 4 : 0;
    Literal y = (mask >> 2) ? 5 : 1;
    Literal z = (mask >> 1) ? 6 : 2;
    Literal w = (mask >> 0) ? 7 : 3;
    return builder.opVectorShuffle(vecTypeId, dest, source, {x,y,z,w});
}

Literal RSXFragmentProgram::getSwizzledValue(Literal vector, U8 swizzle) {
    // Check if swizzling is required. Note that: [00,01,10,11] -> [0,1,2,3] -> [x,y,z,w].
    if (swizzle == 0b00011011) {
        return vector;
    }
    Literal x = (swizzle >> 6) & 0b11;
    Literal y = (swizzle >> 4) & 0b11;
    Literal z = (swizzle >> 2) & 0b11;
    Literal w = (swizzle >> 0) & 0b11;
    return builder.opVectorShuffle(vecTypeId, vector, vector, {x,y,z,w});
}

std::string RSXFragmentProgram::get_header()
{
    std::string header = "#version 420\n";

    // Input registers
    for (const auto& reg : input_regs) {
        if (usedInputs[reg.rsxIndex] && !reg.predefined) {
            header += format("in vec4 %s;", reg.glReg);
        }
    }

    // Output registers
    for (const auto& reg : output_regs) {
        if (usedOutputs[reg.rsxRegIndex32]) {
            header += format("layout (location = %d) out vec4 %s;", reg.rsxRegIndex32, reg.glReg);
        }
    }

    // Sampler registers
    for (int i = 0; i < 16; i++) {
        if (usedSamplers[i]) {
            header += format("layout (binding = %d) uniform sampler2D tex%d;", i, i);
        }
    }

    header += "vec4 f[15];"; // Input attribute registers
    header += "vec4 r[48];"; // Temporary register: Full-width (4 x 32-bit float) register
    header += "vec4 h[48];"; // Temporary register: Half-width (4 x 16-bit float) register (TODO: Should be overlapped with the full-width registers above)
    return header;
}

Literal RSXFragmentProgram::getSourceVector(int index) {
    // Get source descriptor
    const rsx_fp_instruction_source_t source = { instr.word[index+1] & 0x3FFFF };

    // Get source value
    Literal sourceId = 0;
    switch (source.type) {
    case RSX_FP_REGISTER_TYPE_TEMP:
        //src = format("%s[%d]", source.half ? "h" : "r", source.index);
        break;
    case RSX_FP_REGISTER_TYPE_INPUT:
        usedInputs[instr.input_index] = true;
        //src = format("f[%d]", instr.input_index);
        break;
    case RSX_FP_REGISTER_TYPE_CONSTANT:
        /*src = format("vec4(%f, %f, %f, %f)",
            get_word<F32>(instr_ptr->word[0]),
            get_word<F32>(instr_ptr->word[1]),
            get_word<F32>(instr_ptr->word[2]),
            get_word<F32>(instr_ptr->word[3]));*/
        instr_ptr++;
        break;
    }

    // Swizzling
    sourceId = getSwizzledValue(sourceId, source.swizzling);

    // Absolute value
    // TODO

    // Negated value
    if (source.neg) {
        sourceId = builder.opFNegate(sourceId);
    }
    return sourceId;
}

Literal RSXFragmentProgram::getSourceSampler() {
    usedSamplers[instr.tex] = true;
    return 0;// TODO: format("tex%d", instr.tex);
}

void RSXFragmentProgram::setDestVector(Literal value) {
    if (instr.dst_index >= 48) {
        logger.error(LOG_GPU, "Fragment program: Destination register out of range");
    }
    usedOutputs[instr.dst_index] = true;
    //return format("%s[%d]%s", instr.dst_half ? "h" : "r", instr.dst_index, get_fp_mask(instr.dst_mask));
}

void RSXFragmentProgram::decompile(const rsx_fp_instruction_t* buffer) {
    usedInputs.reset();
    usedOutputs.reset();
    usedSamplers.reset();

    // Set pointer to current instruction
    instr_ptr = buffer;

    // Shader body
    while (true) {
        // Set current instruction
        instr.word[0] = get_word<U32>(instr_ptr->word[0]);
        instr.word[1] = get_word<U32>(instr_ptr->word[1]);
        instr.word[2] = get_word<U32>(instr_ptr->word[2]);
        instr.word[3] = get_word<U32>(instr_ptr->word[3]);
        instr_ptr++;

        switch (instr.opcode) {
        case RSX_FP_OPCODE_NOP:
            break;
        /*case RSX_FP_OPCODE_MOV:
            source += format("%s = %s%s;", DST(), SRC(0), get_fp_mask(instr.dst_mask));
            break;
        case RSX_FP_OPCODE_MUL:
            source += format("%s = (%s * %s)%s;\n", DST(), SRC(0), SRC(1), get_fp_mask(instr.dst_mask));
            break;
        case RSX_FP_OPCODE_TEX:
            source += format("%s = texture(%s, %s.xy)%s;", DST(), TEX(), SRC(0), get_fp_mask(instr.dst_mask));
            break;
        case RSX_FP_OPCODE_FENCB:
            break;*/
        default:
            logger.error(LOG_GPU, "Fragment program: Unknown opcode (%d)", instr.opcode);
        }

        // Final instruction
        if (instr.end) {
            break;
        }
    }

    /*// Map OpenGL input attributes to RSX input registers
    for (const auto& reg : input_regs) {
        if (usedInputs[reg.rsxIndex]) {
            source = format("%s = %s;", reg.rsxReg, reg.glReg) + source;
        }
    }

    // Map RSX output registers to OpenGL output attributes
    for (const auto& reg : output_regs) {
        if (usedOutputs[reg.rsxRegIndex32]) {
            source += format("%s = r[%d];", reg.glReg, reg.rsxRegIndex32);
        }
    }

    // Merge header and body
    source = get_header() + "void main() { " + source + "}";*/
}

}  // namespace gpu
