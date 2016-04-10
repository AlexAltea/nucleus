/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/opcodes.h"
#include "nucleus/cpu/hir/value.h"

#include <vector>
#include <map>

namespace cpu {
namespace hir {

// Forward declarations
class Value;
class Block;
class Function;

class Instruction {
public:
    Block* parent;

    // Immediate operand type
    using Immediate = U64;

    union Operand {
        Immediate immediate;
        Value* value;
        Block* block;
        Function* function;

        operator const Immediate() const { return immediate; }
        operator const Value*() const { return value; }
        operator const Block*() const { return block; }
        operator const Function*() const { return function; }

        void setValue(Value* v) {
            v->usage += 1;
            value = v;
        }

        /**
         * Return a human-readable version of this HIR operand
         * @param[in]  sigType  Signature type of this HIR operand
         * @return              String containing the readable version of this HIR operand
         */
        std::string dump(U08 sigType) const;
    };

    Opcode opcode;
    OpcodeFlags flags;

    Value* dest;
    Operand src1;
    Operand src2;
    Operand src3;

    /**
     * Save a human-readable version of this HIR instruction
     * @return String containing the readable version of this HIR instruction
     */
    std::string dump() const;
};

}  // namespace hir
}  // namespace cpu
