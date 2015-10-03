/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "block.h"
#include "nucleus/cpu/hir/function.h"
#include "nucleus/cpu/hir/instruction.h"

namespace cpu {
namespace hir {

Block::Block(Function* parent) : parent(parent) {
    if (parent->flags & FUNCTION_IS_DECLARED) {
        parent->flags |= FUNCTION_IS_DEFINING;
        parent->entry = this;
    }
    parent->blocks.push_back(this);
}

Block::~Block() {
    for (auto instruction : instructions) {
        delete instruction;
    }
}

S32 Block::getId() {
    if (id < 0) {
        id = parent->valueIdCounter++;
    }
    return id;
}

std::string Block::dump() {
    std::string output;
    output += "  b" + std::to_string(getId()) + ":\n";
    for (const auto& instruction : instructions) {
        output += instruction->dump();
    }
    return output;
}

}  // namespace hir
}  // namespace cpu
