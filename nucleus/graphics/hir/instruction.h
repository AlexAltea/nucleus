/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/hir/hir.h"

#include <string>
#include <vector>

namespace gfx {
namespace hir {

class Instruction {
public:
    int opcode;
    Literal typeId;
    Literal resultId;
    std::vector<Literal> operands;
    std::string comment;

    Instruction();
    Instruction(int opcode, Literal typeId, Literal resultId);

    // Operand management
    void addOperandLiteral(Literal id);
    void addOperandImmediate(unsigned int imm);
    void addOperandString(const char* str);
    void addOperandArray(const std::vector<Literal>& arr);

    /**
     * Save a human-readable version of this HIR instruction
     * @return  String containing the readable version of this HIR block
     */
    std::string dump();
};

}  // namespace hir
}  // namespace gfx
