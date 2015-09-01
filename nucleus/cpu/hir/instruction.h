/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/cpu/hir/opcodes.h"
#include "nucleus/cpu/hir/value.h"

namespace cpu {
namespace hir {

class Value;
class Block;
class Function;

class Instruction {
public:
    union Operand {
        Value* value;
        Block* block;
        Function* function;
    };

    Opcode opcode;
    OpcodeFlags flags;

    Value* dest;
    Operand src1;
    Operand src2;
    Operand src3;
};

}  // namespace hir
}  // namespace cpu
