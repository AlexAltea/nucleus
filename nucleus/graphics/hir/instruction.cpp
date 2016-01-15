/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "instruction.h"

namespace gfx {
namespace hir {

Instruction::Instruction() {
}

Instruction::Instruction(int opcode, Literal typeId, Literal resultId) :
    opcode(opcode), typeId(typeId), resultId(resultId) {
}

std::string Instruction::dump() {
    return std::string{}; // TODO
}

}  // namespace hir
}  // namespace gfx
