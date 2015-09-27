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

struct Parameters {
    virtual ~Parameters() {};
};

struct ParametersCall : Parameters {
    std::vector<Value*> args;

    ParametersCall(const std::vector<Value*>& args) : args(args) {}
    ~ParametersCall() {};
};

struct ParametersSwitch {
    std::map<U64, Block*> cases;

    ParametersSwitch(const std::map<U64, Block*>& cases) : cases(cases) {}
    ~ParametersSwitch() {};
};

class Instruction {
public:
    // Immediate operand type
    using Immediate = U64;

    union Operand {
        Immediate immediate;
        Value* value;
        Block* block;
        Function* function;
        Parameters* parameters;

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
        operator const Parameters*() const {
            return parameters;
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
