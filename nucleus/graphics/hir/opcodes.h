/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace gfx {
namespace hir {

using OpcodeFlags = U8;

enum CompareFlags : OpcodeFlags {
    COMPARE_EQ   = 1 << 0,
    COMPARE_NE   = 1 << 1,
    COMPARE_SLT  = 1 << 2,
    COMPARE_SLE  = 1 << 3,
    COMPARE_SGE  = 1 << 4,
    COMPARE_SGT  = 1 << 5,
    COMPARE_ULT  = COMPARE_SLT | (1 << 6),
    COMPARE_ULE  = COMPARE_SLE | (1 << 6),
    COMPARE_UGE  = COMPARE_SGE | (1 << 6),
    COMPARE_UGT  = COMPARE_SGT | (1 << 6),
};

enum VectorFlags : OpcodeFlags {
    COMPONENT_NONE,
    COMPONENT_I16,
    COMPONENT_I32,
    COMPONENT_F16,
    COMPONENT_F32,
};

enum Opcode {
#define OPCODE(id, ...) OPCODE_##id,
#include "opcodes.inl"
#undef OPCODE

    __OPCODE_COUNT,
};

enum OpcodeSignatureType {
    OPCODE_SIG_TYPE_X = 0, // Void
    OPCODE_SIG_TYPE_I = 1, // Immediate
    OPCODE_SIG_TYPE_M = 2, // Maybe
    OPCODE_SIG_TYPE_V = 3, // Value
    OPCODE_SIG_TYPE_B = 4, // Block
    OPCODE_SIG_TYPE_F = 5, // Function
};

enum OpcodeSignature {
    OPCODE_SIG_X       = (OPCODE_SIG_TYPE_X),
    OPCODE_SIG_M       = (OPCODE_SIG_TYPE_M),
    OPCODE_SIG_V       = (OPCODE_SIG_TYPE_V),
    OPCODE_SIG_X_V     = (OPCODE_SIG_TYPE_X) | (OPCODE_SIG_TYPE_V << 3),
    OPCODE_SIG_X_M     = (OPCODE_SIG_TYPE_X) | (OPCODE_SIG_TYPE_M << 3),
    OPCODE_SIG_X_B     = (OPCODE_SIG_TYPE_X) | (OPCODE_SIG_TYPE_B << 3),
    OPCODE_SIG_X_F     = (OPCODE_SIG_TYPE_X) | (OPCODE_SIG_TYPE_F << 3),
    OPCODE_SIG_V_I     = (OPCODE_SIG_TYPE_V) | (OPCODE_SIG_TYPE_I << 3),
    OPCODE_SIG_V_V     = (OPCODE_SIG_TYPE_V) | (OPCODE_SIG_TYPE_V << 3),
    OPCODE_SIG_M_F     = (OPCODE_SIG_TYPE_M) | (OPCODE_SIG_TYPE_F << 3),
    OPCODE_SIG_X_I_V   = (OPCODE_SIG_TYPE_X) | (OPCODE_SIG_TYPE_I << 3) | (OPCODE_SIG_TYPE_V << 6),
    OPCODE_SIG_X_V_V   = (OPCODE_SIG_TYPE_X) | (OPCODE_SIG_TYPE_V << 3) | (OPCODE_SIG_TYPE_V << 6),
    OPCODE_SIG_X_V_B   = (OPCODE_SIG_TYPE_X) | (OPCODE_SIG_TYPE_V << 3) | (OPCODE_SIG_TYPE_B << 6),
    OPCODE_SIG_M_V_F   = (OPCODE_SIG_TYPE_M) | (OPCODE_SIG_TYPE_V << 3) | (OPCODE_SIG_TYPE_F << 6),
    OPCODE_SIG_V_I_V   = (OPCODE_SIG_TYPE_V) | (OPCODE_SIG_TYPE_I << 3) | (OPCODE_SIG_TYPE_V << 6),
    OPCODE_SIG_V_V_V   = (OPCODE_SIG_TYPE_V) | (OPCODE_SIG_TYPE_V << 3) | (OPCODE_SIG_TYPE_V << 6),
    OPCODE_SIG_X_V_V_V = (OPCODE_SIG_TYPE_X) | (OPCODE_SIG_TYPE_V << 3) | (OPCODE_SIG_TYPE_V << 6) | (OPCODE_SIG_TYPE_V << 9),
    OPCODE_SIG_V_V_V_V = (OPCODE_SIG_TYPE_V) | (OPCODE_SIG_TYPE_V << 3) | (OPCODE_SIG_TYPE_V << 6) | (OPCODE_SIG_TYPE_V << 9),
};

struct OpcodeInfo {
    const char* name;
    OpcodeFlags flags;
    OpcodeSignature signature;

    U8 getSignatureDest() const {
        return ((signature >> 0) & 0b111);
    }
    U8 getSignatureSrc1() const {
        return ((signature >> 3) & 0b111);
    }
    U8 getSignatureSrc2() const {
        return ((signature >> 6) & 0b111);
    }
    U8 getSignatureSrc3() const {
        return ((signature >> 9) & 0b111);
    }
};

extern OpcodeInfo opcodeInfo[__OPCODE_COUNT + 1];

}  // namespace hir
}  // namespace gfx
