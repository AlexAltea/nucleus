/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "rsx_vp.h"
#include "nucleus/assert.h"
#include "nucleus/logger/logger.h"

using namespace gfx::hir;

namespace gpu {

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

Literal RSXVertexProgram::getMaskedValue(Literal dest, Literal source, U8 mask) {
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

Literal RSXVertexProgram::getSwizzledValue(Literal vector, U8 swizzle) {
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

Literal RSXVertexProgram::getSourceVector(int index) {
    // Get source descriptor
    rsx_vp_instruction_source_t source;
    switch (index) {
    case 0: source.value = (instr.src0_hi << 9) | instr.src0_lo; break;
    case 1: source.value = (instr.src1); break;
    case 2: source.value = (instr.src2_hi << 11) | instr.src2_lo; break;
    }

    // Get source value
    Literal sourceId = 0;
    switch (source.type) {
    case RSX_VP_REGISTER_TYPE_DATA:
        //src = format("r[%d]", source.index);
        break;
    case RSX_VP_REGISTER_TYPE_INPUT:
        usedInputs[instr.src_input] = true;
        //src = format("v%d", instr.src_input);
        break;
    case RSX_VP_REGISTER_TYPE_CONSTANT:
        //src = format("c[%d]", instr.src_const);
        break;
    }

    // Swizzling
    sourceId = getSwizzledValue(sourceId, source.swizzling);

    // Absolute value
    switch (index) {
    case 0: //src = instr.abs_src0 ? "abs(" + src + ")" : src;
    case 1: //src = instr.abs_src1 ? "abs(" + src + ")" : src;
    case 2: //src = instr.abs_src2 ? "abs(" + src + ")" : src;
        break;
    }

    // Negated value
    if (source.neg) {
        sourceId = builder.opFNegate(sourceId);
    }
    return sourceId;
}


void RSXVertexProgram::setDestVector(Literal value) {
    // Data register
    if (instr.dst == 0x1F) {
        //dst = format("r[%d]", instr.dst_data);
    }
    // Output register
    else {
        if (instr.dst >= 16) {
            logger.error(LOG_GPU, "VPE: Destination register out of range");
        }
        usedOutputs[instr.dst] = true;
        //dst = format("o[%d]", instr.dst);
    }
    //return dst + get_vp_mask(instr.masc_vec);
}
                       
/*std::string RSXVertexProgram::get_header()
{
    std::string header = "#version 330\n";

    // Input registers
    for (U32 i = 0; i < 16; i++) {
        if (usedInputs[i]) {
            header += format("layout (location = %d) in vec4 v%d;", i, i);
        }
    }

    // Output registers
    for (const auto& reg : output_regs) {
        if (usedOutputs[reg.rsxIndex] && !reg.predefined) {
            header += format("out vec4 %s;", reg.glReg);
        }
    }

    header += "vec4 o[16];";           // Output registers
    header += "vec4 r[48];";           // Data registers
    header += "uniform vec4 c[468];";  // Constant registers
    return header;
}*/

void RSXVertexProgram::decompile(const rsx_vp_instruction_t* buffer) {
    usedInputs.reset();
    usedOutputs.reset();

    // Shader body
    do {
        instr = *buffer;

        // SCA Opcodes
        switch (instr.opcode_sca) {
        case RSX_VP_OPCODE_SCA_NOP:
            break;
        default:
            logger.error(LOG_GPU, "VPE: Unknown SCA opcode (%d)", instr.opcode_sca);
        }

        // VEC Opcodes
        switch (instr.opcode_vec) {
        case RSX_VP_OPCODE_VEC_NOP:
            break;
        /*case RSX_VP_OPCODE_VEC_MOV:
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
            break;*/
        default:
            logger.error(LOG_GPU, "VPE: Unknown VEC opcode (%d)", instr.opcode_vec);
        }
    } while (!(buffer++)->end);

    // Map RSX output registers to shader output vectors
    /*for (const auto& reg : output_regs) {
        if (usedOutputs[reg.rsxIndex]) {
            source += format("%s = %s;", reg.glReg, reg.rsxReg);
        }
    }

    // Merge header and body
    source = get_header() + "void main() { " + source + "}";*/
}

}  // namespace gpu
