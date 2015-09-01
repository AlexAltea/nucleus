/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "block.h"
#include "nucleus/cpu/hir/function.h"

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

}  // namespace hir
}  // namespace cpu
