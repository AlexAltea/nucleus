/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <string>

namespace gpu {

// RSX Fragment Program opcodes
enum {
    RSX_FP_OPCODE_NOP        = 0x00, // No-Operation
    RSX_FP_OPCODE_MOV        = 0x01, // Move
    RSX_FP_OPCODE_MUL        = 0x02, // Multiply
    RSX_FP_OPCODE_ADD        = 0x03, // Add
    RSX_FP_OPCODE_MAD        = 0x04, // Multiply-Add
    RSX_FP_OPCODE_DP3        = 0x05, // 3-component Dot Product
    RSX_FP_OPCODE_DP4        = 0x06, // 4-component Dot Product
    RSX_FP_OPCODE_DST        = 0x07, // Distance
    RSX_FP_OPCODE_MIN        = 0x08, // Minimum
    RSX_FP_OPCODE_MAX        = 0x09, // Maximum
    RSX_FP_OPCODE_SLT        = 0x0A, // Set-If-LessThan
    RSX_FP_OPCODE_SGE        = 0x0B, // Set-If-GreaterEqual
    RSX_FP_OPCODE_SLE        = 0x0C, // Set-If-LessEqual
    RSX_FP_OPCODE_SGT        = 0x0D, // Set-If-GreaterThan
    RSX_FP_OPCODE_SNE        = 0x0E, // Set-If-NotEqual
    RSX_FP_OPCODE_SEQ        = 0x0F, // Set-If-Equal
    RSX_FP_OPCODE_FRC        = 0x10, // Fraction (fract)
    RSX_FP_OPCODE_FLR        = 0x11, // Floor
    RSX_FP_OPCODE_KIL        = 0x12, // Kill fragment
    RSX_FP_OPCODE_PK4        = 0x13, // Pack four signed 8-bit values
    RSX_FP_OPCODE_UP4        = 0x14, // Unpack four signed 8-bit values
    RSX_FP_OPCODE_DDX        = 0x15, // Partial-derivative in x (Screen space derivative w.r.t. x)
    RSX_FP_OPCODE_DDY        = 0x16, // Partial-derivative in y (Screen space derivative w.r.t. y)
    RSX_FP_OPCODE_TEX        = 0x17, // Texture lookup
    RSX_FP_OPCODE_TXP        = 0x18, // Texture sample with projection (Projective texture lookup)
    RSX_FP_OPCODE_TXD        = 0x19, // Texture sample with partial differentiation (Texture lookup with derivatives)
    RSX_FP_OPCODE_RCP        = 0x1A, // Reciprocal
    RSX_FP_OPCODE_RSQ        = 0x1B, // Reciprocal Square Root
    RSX_FP_OPCODE_EX2        = 0x1C, // Exponentiation base 2
    RSX_FP_OPCODE_LG2        = 0x1D, // Log base 2
    RSX_FP_OPCODE_LIT        = 0x1E, // Lighting coefficients
    RSX_FP_OPCODE_LRP        = 0x1F, // Linear Interpolation
    RSX_FP_OPCODE_STR        = 0x20, // Set-If-True
    RSX_FP_OPCODE_SFL        = 0x21, // Set-If-False
    RSX_FP_OPCODE_COS        = 0x22, // Cosine
    RSX_FP_OPCODE_SIN        = 0x23, // Sine
    RSX_FP_OPCODE_PK2        = 0x24, // Pack two 16-bit floats
    RSX_FP_OPCODE_UP2        = 0x25, // Unpack two 16-bit floats
    RSX_FP_OPCODE_POW        = 0x26, // Power
    RSX_FP_OPCODE_PKB        = 0x27, // Pack bytes
    RSX_FP_OPCODE_UPB        = 0x28, // Unpack bytes
    RSX_FP_OPCODE_PK16       = 0x29, // Pack 16 bits
    RSX_FP_OPCODE_UP16       = 0x2A, // Unpack 16
    RSX_FP_OPCODE_BEM        = 0x2B, // Bump-environment map (a.k.a. 2D coordinate transform)
    RSX_FP_OPCODE_PKG        = 0x2C, // Pack with sRGB transformation
    RSX_FP_OPCODE_UPG        = 0x2D, // Unpack gamma
    RSX_FP_OPCODE_DP2A       = 0x2E, // 2-component dot product with scalar addition
    RSX_FP_OPCODE_TXL        = 0x2F, // Texture sample with LOD
    RSX_FP_OPCODE_TXB        = 0x31, // Texture sample with bias
    RSX_FP_OPCODE_TEXBEM     = 0x33,
    RSX_FP_OPCODE_TXPBEM     = 0x34,
    RSX_FP_OPCODE_BEMLUM     = 0x35,
    RSX_FP_OPCODE_REFL       = 0x36, // Reflection vector
    RSX_FP_OPCODE_TIMESWTEX  = 0x37,
    RSX_FP_OPCODE_DP2        = 0x38, // 2-component dot product
    RSX_FP_OPCODE_NRM        = 0x39, // Normalize
    RSX_FP_OPCODE_DIV        = 0x3A, // Division
    RSX_FP_OPCODE_DIVSQ      = 0x3B, // Divide by Square Root
    RSX_FP_OPCODE_LIF        = 0x3C, // Final part of LIT
    RSX_FP_OPCODE_FENCT      = 0x3D, // Fence T?
    RSX_FP_OPCODE_FENCB      = 0x3E, // Fence B?
    RSX_FP_OPCODE_BRK        = 0x40, // Break
    RSX_FP_OPCODE_CAL        = 0x41, // Subroutine call
    RSX_FP_OPCODE_IFE        = 0x42, // If
    RSX_FP_OPCODE_LOOP       = 0x43, // Loop
    RSX_FP_OPCODE_REP        = 0x44, // Repeat
    RSX_FP_OPCODE_RET        = 0x45, // Return
};

// RSX Fragment Program inlined vector constant
struct rsx_fp_constant_t {
    union {
        U32 word[4];
        struct {
            F32 x;
            F32 y;
            F32 z;
            F32 w;
        };
    };
};

// RSX Fragment Program instruction
union rsx_fp_instruction_t
{
#define FIELD(from, to, type) struct{ U32:(32-to-1); type:(to-from+1); U32:from; }

    // NOTE: To read the data with the fields below, reverse the byte and half-word endianness of these words
    // In other words, do following map for each word [A,B,C,D] -> [D,C,B,A] -> [B,A,D,C], where A,B,C,D represent bytes.
    U32 word[4];
    U64 dword[2];

    struct {
        union {
            FIELD( 0,  0, U32 saturate);
            FIELD( 2,  7, U32 opcode);
            FIELD(10, 13, U32 tex);
            FIELD(15, 18, U32 input_index);  // Input attribute register index
            FIELD(19, 22, U32 dst_mask);
            FIELD(24, 24, U32 dst_half);     // Half precision (f16)
            FIELD(25, 30, U32 dst_index);    // Destination register index
            FIELD(31, 31, U32 end);          // Last instruction flag
        };
        union {
            FIELD( 0, 31, U32 word1);
        };
        union {
            FIELD(29, 31, U32 type);
        };
        union {
            FIELD( 0, 31, U32 end_offset);
        };
    };

#undef FIELD
};

union rsx_fp_instruction_source_t
{
    U32 value;

    struct {
        U32 type      : 2; // Register type: { 0: Data, 1: Input, 2: Constant }
        U32 index     : 6; // Register input
        U32 half      : 1; // Half precision (f16)
        U32 swizzle_x : 2; // Swizzling mask on the component x
        U32 swizzle_y : 2; // Swizzling mask on the component y
        U32 swizzle_z : 2; // Swizzling mask on the component z
        U32 swizzle_w : 2; // Swizzling mask on the component w
        U32 neg       : 1; // Negated value
    };
};

class RSXFragmentProgram {
    // Input/Output/Sampler registers used in the program
    U32 usedInputs;
    U64 usedOutputs;
    U32 usedSamplers;

    // Current instruction being processed and pointer to the next instruction
    rsx_fp_instruction_t instr;
    rsx_fp_instruction_t* instr_ptr;

    // Generate the GLSL header and register declarations based on the decompilation
    std::string get_header();

    // Get the source, destination and sampler registers of the current instruction
    std::string get_src(U32 n);
    std::string get_dst();
    std::string get_tex();

    // Get 32-bit of data reversing its byte and half-word endianness
    template <typename T>
    T get_word(U32 word) {
        U32 result = (SE32(word) >> 16) | (SE32(word) << 16);
        return (T&)result;
    }

public:
    // OpenGL shader ID
    U32 id = 0;

    // Generate a GLSL fragment shader equivalent to the given buffer
    void decompile(rsx_fp_instruction_t* buffer);

    // Compile the generated GLSL code for the host GPU
    bool compile();
};

}  // namespace gpu
