/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "builder.h"
#include "nucleus/graphics/hir/block.h"
#include "nucleus/graphics/hir/function.h"
#include "nucleus/graphics/hir/instruction.h"
#include "nucleus/graphics/hir/module.h"
#include "nucleus/assert.h"

namespace gfx {
namespace hir {

// HIR instruction generation
Instruction* Builder::appendInstr(Opcode opcode, bool hasResultId) {
    Instruction* instr = new Instruction();
    instr->opcode = opcode;
    instr->typeId = 0;

    if (hasResultId) {
        Literal resultId = module->instructions.size();
        module->instructions.push_back(instr);
        instr->resultId = resultId;
    } else {
        instr->resultId = 0;
    }
    return instr;
}

// Insertion
void Builder::setModule(Module* m) {
    module = m;
}

void Builder::setInsertPoint(Block* block) {
    ib = block;
    ip = block->instructions.end();
}

void Builder::setInsertPoint(Block* block, std::list<Instruction*>::iterator insertPoint) {
    ib = block;
    ip = insertPoint;
}

// HIR types
Literal Builder::opTypeVoid() {
    Instruction* i = appendInstr(OP_TYPE_VOID, true);
    return i->resultId;
}
Literal Builder::opTypeBool() {
    Instruction* i = appendInstr(OP_TYPE_BOOL, true);
    return i->resultId;
}
Literal Builder::opTypeInt(Literal width, bool signedness) {
    Instruction* i = appendInstr(OP_TYPE_INT, true);
    i->operands.push_back(width);
    i->operands.push_back(signedness);
    return i->resultId;
}
Literal Builder::opTypeFloat(Literal width) {
    Instruction* i = appendInstr(OP_TYPE_FLOAT, true);
    i->operands.push_back(width);
    return i->resultId;
}
Literal Builder::opTypeVector(Literal componentType, Literal componentCount) {
    Instruction* i = appendInstr(OP_TYPE_VECTOR, true);
    i->operands.push_back(componentType);
    i->operands.push_back(componentCount);
    return i->resultId;
}
Literal Builder::opTypeMatrix(Literal columnType, Literal columnCount) {
    Instruction* i = appendInstr(OP_TYPE_MATRIX, true);
    i->operands.push_back(columnType);
    i->operands.push_back(columnCount);
    return i->resultId;
}

}  // namespace hir
}  // namespace gfx
