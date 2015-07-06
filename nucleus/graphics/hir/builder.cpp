/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "builder.h"

namespace graphics {
namespace hir {

void Builder::setInsertPoint(Block* block) {
    insertionBlock = block;
    insertionIp = block->end();
}

void Builder::setInsertPoint(Block* block, std::list<Instruction>::iterator ip) {
    insertionBlock = block;
    insertionIp = ip;
}

}  // namespace hir
}  // namespace graphics
