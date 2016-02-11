/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "instruction.h"
#include "nucleus/format.h"

namespace gfx {
namespace hir {

Instruction::Instruction() {
}

Instruction::Instruction(int opcode, Literal typeId, Literal resultId) :
    opcode(opcode), typeId(typeId), resultId(resultId) {
}

void Instruction::addOperandLiteral(Literal id) {
    operands.push_back(id);
}

void Instruction::addOperandImmediate(unsigned int imm) {
    operands.push_back(imm);
}

void Instruction::addOperandArray(const std::vector<Literal>& arr) {
    operands.insert(operands.end(), arr.begin(), arr.end());
}

void Instruction::addOperandString(const char* str) {
    unsigned int word;
    char* wordString = (char*)&word;
    char* wordPtr = wordString;
    int charCount = 0;
    char c;
    do {
        c = *(str++);
        *(wordPtr++) = c;
        ++charCount;
        if (charCount == 4) {
            addOperandImmediate(word);
            wordPtr = wordString;
            charCount = 0;
        }
    } while (c != 0);

    // deal with partial last word
    if (charCount > 0) {
        // pad with 0s
        for (; charCount < 4; ++charCount)
            *(wordPtr++) = 0;
        addOperandImmediate(word);
    }
}

std::string Instruction::dump() {
    std::string output = "  ";
    if (resultId) {
        output += format("%%%d = ", resultId);
    }
    output += format("[%d]", opcode);
    if (typeId) {
        output += format(" %%%d", typeId);
    }
    for (const auto& operand : operands) {
        output += format(" %d", operand);
    }
    output += "\n";
    return output;
}

}  // namespace hir
}  // namespace gfx
