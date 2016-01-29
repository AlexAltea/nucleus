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

// Instruction utilities
Instruction* Builder::appendInstr(Opcode opcode, bool hasResultId) {
    Instruction* instr = new Instruction();
    instr->opcode = opcode;
    instr->typeId = 0;

    if (hasResultId) {
        Literal resultId = module->idInstructions.size();
        module->idInstructions.push_back(instr);
        instr->resultId = resultId;
    } else {
        instr->resultId = 0;
    }
    return instr;
}

Instruction* Builder::appendInstr(U16 opcode, Literal typeId, Literal resultId) {
    Instruction* instr = new Instruction();
    instr->opcode = opcode;
    instr->typeId = typeId;
    instr->resultId = resultId;

    if (resultId != 0) {
        module->idInstructions[resultId] = instr;
    }
    return instr;
}

Instruction* Builder::getInstr(Literal instrId) {
    if (instrId >= module->idInstructions.size()) {
        return nullptr;
    }
    return module->idInstructions[instrId];
}

// Insertion
void Builder::setModule(Module* m) {
    module = m;
}

void Builder::setInsertPoint(Block* block) {
    ib = block;
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

// HIR operations
Literal Builder::opFNegate(Literal value) {
    Instruction* i = appendInstr(OP_FNEGATE, true);
    i->typeId = getInstr(value)->typeId;
    return i->resultId;
}

Literal Builder::opVectorShuffle(Literal resType, Literal vec1, Literal vec2, std::vector<Literal> components) {
    Instruction* i = appendInstr(OP_VECTOR_SHUFFLE, true);
    i->typeId = resType;
    i->operands.push_back(vec1);
    i->operands.push_back(vec2);
    i->operands.insert(i->operands.end(), components.begin(), components.end());
    return i->resultId;
}

}  // namespace hir
}  // namespace gfx
