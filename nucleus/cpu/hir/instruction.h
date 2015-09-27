/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/cpu/hir/opcodes.h"
#include "nucleus/cpu/hir/value.h"

#include <vector>
#include <map>

namespace cpu {
namespace hir {

class Value;
class Block;
class Function;

class Instruction {
public:
    // Immediate operand type
    using Immediate = U64;

    union Operand {
        Immediate immediate;
        Value* value;
        Block* block;
        Function* function;

        operator const Immediate() const {
            return immediate;
        }
        operator const Value*() const {
            return value;
        }
        operator const Block*() const {
            return block;
        }
        operator const Function*() const {
            return function;
        }
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
