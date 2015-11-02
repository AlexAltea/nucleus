/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "block.h"
#include "nucleus/graphics/hir/function.h"
#include "nucleus/graphics/hir/instruction.h"

namespace gfx {
namespace hir {

Block::Block(Function* parent) : parent(parent), flags(0) {
    if (parent->flags & FUNCTION_IS_DECLARED) {
        parent->flags |= FUNCTION_IS_DEFINING;
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
        id = parent->blockIdCounter++;
    }
    return id;
}

std::string Block::dump() {
    std::string output;
    output += "  b" + std::to_string(getId());
    output += (flags & BLOCK_IS_ENTRY) ? " (entry):\n" : ":\n";
    for (const auto& instruction : instructions) {
        output += instruction->dump();
    }
    return output;
}

}  // namespace hir
}  // namespace gfx
