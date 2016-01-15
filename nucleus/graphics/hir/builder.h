/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/hir/hir.h"

#include <list>
#include <vector>

namespace gfx {
namespace hir {

// Forward declarations
class Instruction;
class Block;
class Module;

class Builder {
    Module* module;
    Block* ib;
    std::list<Instruction*>::iterator ip;

public:
    // Insertion
    void setModule(Module* module);
    void setInsertPoint(Block* block);

    // HIR instruction generation
    Instruction* appendInstr(Opcode opcode, bool hasResultId);
    Instruction* appendInstr(U16 opcode, Literal idType, Literal idResult);

    // HIR types
    Literal opTypeVoid();
    Literal opTypeBool();
    Literal opTypeInt(Literal width, bool signedness);
    Literal opTypeFloat(Literal width);
    Literal opTypeVector(Literal componentType, Literal componentCount);
    Literal opTypeMatrix(Literal columnType, Literal columnCount);

    // HIR operations
    Literal opIAdd(Literal lhs, Literal rhs);
    Literal opISub(Literal lhs, Literal rhs);
    Literal opIMul(Literal lhs, Literal rhs);
    Literal opIDiv(Literal lhs, Literal rhs);
};

}  // namespace hir
}  // namespace gfx
