/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "function.h"
#include "nucleus/graphics/hir/block.h"
#include "nucleus/graphics/hir/instruction.h"
#include "nucleus/graphics/hir/module.h"

namespace gfx {
namespace hir {

Function::Function(Module& parent, Literal type) : parent(parent) {
    Module& module = parent;
    module.addFunction(this);

    Literal typeId = module.idInstructions[type]->operands[0];
    Literal resultId = module.idInstructions.size();
    function = new Instruction(OP_FUNCTION, typeId, resultId);
    function->addOperandImmediate(FUNCTION_CONTROL_NONE);
    function->addOperandLiteral(type);

    // Update result ID table
    module.idInstructions.push_back(function);
}

Function::Function(Module& parent, Literal type, Literal id) : parent(parent) {
    Module& module = parent;
    module.addFunction(this);

    function = new Instruction(OP_FUNCTION, type, id);

    // Update result ID table
    module.idInstructions.push_back(function);
}

Literal Function::getId() const {
    return function->resultId;
}

Module& Function::getParent() const {
    return parent;
}

void Function::addLocalVariable(Instruction* instr) {
    blocks[0]->variables.push_back(instr);
}

std::string Function::dump() {
    std::string output;
    output += function->dump();
    for (const auto& param : parameters) {
        output += param->dump();
    }
    for (const auto& block : blocks) {
        output += block->dump();
    }
    return output;
}

}  // namespace hir
}  // namespace gfx
