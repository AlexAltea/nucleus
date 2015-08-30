/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace cpu {
namespace hir {

using OpcodeFlags = U8;

enum ArithmeticFlags : OpcodeFlags {
    ARITHMETIC_SIGNED    = 0 << 0,
    ARITHMETIC_UNSIGNED  = 1 << 0,
};

enum MemoryFlags : OpcodeFlags {
    ENDIAN_DEFAULT   = 0,
    ENDIAN_BIG       = 1 << 0,  // Big Endian memory access
    ENDIAN_LITTLE    = 1 << 1,  // Little Endian memory access
};

enum VectorFlags : OpcodeFlags {
    COMPONENT_I8,
    COMPONENT_I16,
    COMPONENT_I32,
    COMPONENT_I64,
    COMPONENT_F32,
    COMPONENT_F64,
};

enum Opcode {
#define OPCODE(name, ...) OPCODE_##name,
#include "opcodes.inl"
#undef OPCODE

    _OPCODE_COUNT,
};

struct OpcodeInfo {
    U32 flags;
    U32 signature;
};

}  // namespace hir
}  // namespace cpu
