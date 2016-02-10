/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "block.h"
#include "nucleus/graphics/hir/function.h"
#include "nucleus/graphics/hir/instruction.h"
#include "nucleus/graphics/hir/module.h"

namespace gfx {
namespace hir {

Block::Block(Function& parent) : parent(parent) {
    Module& module = parent.getParent();
    parent.blocks.push_back(this);

    Literal id = module.idInstructions.size();
    Instruction* label = new Instruction(OP_LABEL, 0, id);
    instructions.push_back(label);

    // Update result ID table
    module.idInstructions.push_back(label);
}

Block::Block(Function& parent, Literal id) : parent(parent) {
    Module& module = parent.getParent();
    parent.blocks.push_back(this);

    Instruction* label = new Instruction(OP_LABEL, 0, id);
    instructions.push_back(label);

    // Update result ID table
    module.idInstructions.push_back(label);
}

Block::~Block() {
    for (auto instruction : instructions) {
        delete instruction;
    }
}

Literal Block::getId() {
    return instructions.front()->resultId;
}

Function& Block::getParent() const {
    return parent;
}

std::string Block::dump() {
    std::string output;
    output += "  b" + std::to_string(getId()) + ":\n";
    for (const auto& instruction : instructions) {
        //output += instruction->dump();
    }
    return output;
}

}  // namespace hir
}  // namespace gfx
