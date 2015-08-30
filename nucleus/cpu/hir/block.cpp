/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "block.h"

namespace cpu {
namespace hir {

Block::Block(Function* parent) : parent(parent) {
}

Block::~Block() {
    for (auto instruction : instructions) {
        delete instruction;
    }
}

}  // namespace hir
}  // namespace cpu
