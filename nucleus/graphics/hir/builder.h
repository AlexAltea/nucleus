/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/hir/instruction.h"
#include "nucleus/graphics/hir/type.h"
#include "nucleus/graphics/hir/value.h"

#include <list>
#include <vector>

namespace gfx {
namespace hir {

// Forward declarations
class Block;
class Value;

class Builder {
    Block* ib;
    std::list<Instruction*>::iterator ip;

public:
    /**
     * HIR insertion
     */
    void setInsertPoint(Block* block);
    void setInsertPoint(Block* block, std::list<Instruction*>::iterator ip);

    // HIR values
    Value* allocValue(Type type);
    Value* cloneValue(Value* source);

    // HIR constants
    Value* getConstantI16(U16 c);
    Value* getConstantI32(U32 c);
    Value* getConstantF16(F32 c); // TODO
    Value* getConstantF32(F32 c);
    Value* getConstantV128(V128 c);

    // HIR builtins
    Value* getBuiltinValue(ValueBuiltin builtin);

    /**
     * HIR instruction generation
     */
    Instruction* appendInstr(Opcode opcode, OpcodeFlags flags, Value* dest = nullptr);

    // Arithmetic operations
    Value* createAdd(Value* lhs, Value* rhs, VectorFlags flags);
    Value* createSub(Value* lhs, Value* rhs, VectorFlags flags);
    Value* createMul(Value* lhs, Value* rhs, VectorFlags flags);
    Value* createDiv(Value* lhs, Value* rhs, VectorFlags flags);

    // Variable operations
    Value* createLoad(Value* variable);
    void createStore(Value* variable, Value* value);
};

}  // namespace hir
}  // namespace gfx
