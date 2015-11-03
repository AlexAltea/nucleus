/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "builder.h"
#include "nucleus/graphics/hir/block.h"
#include "nucleus/graphics/hir/function.h"
#include "nucleus/graphics/hir/instruction.h"
#include "nucleus/assert.h"

#define ASSERT_TYPE_EQUAL(value1, value2) \
    assert_true(value1->type == value2->type)

#define ASSERT_TYPE_INTEGER(value) \
    assert_true(value->isTypeInteger())
#define ASSERT_TYPE_FLOAT(value) \
    assert_true(value->isTypeFloat())
#define ASSERT_TYPE_VECTOR(value) \
    assert_true(value->isTypeVector())

namespace gfx {
namespace hir {

void Builder::setInsertPoint(Block* block) {
    ib = block;
    ip = block->instructions.end();
}

void Builder::setInsertPoint(Block* block, std::list<Instruction*>::iterator insertPoint) {
    ib = block;
    ip = insertPoint;
}

// HIR values
Value* Builder::allocValue(Type type) {
    Value* value = new Value();
    value->type = type;
    value->flags = 0;
    value->usage = 0;
    return value;
}

Value* Builder::cloneValue(Value* source) {
    assert_true(source->isConstant(), "Builder only supports cloning constants");

    Value* value = new Value();
    value->type = source->type;
    value->flags = source->flags;

    if (source->isConstant()) {
        switch (source->type) {
        case TYPE_I16: value->constant.i16 = source->constant.i16; break;
        case TYPE_I32: value->constant.i32 = source->constant.i32; break;
        case TYPE_F16: value->constant.f16 = source->constant.f16; break;
        case TYPE_F32: value->constant.f32 = source->constant.f32; break;
        }
    }
    return value;
}

// HIR constants
Value* Builder::getConstantI16(U16 c) {
    Value* value = allocValue(TYPE_I16);
    value->setConstantI16(c);
    return value;
}
Value* Builder::getConstantI32(U32 c) {
    Value* value = allocValue(TYPE_I32);
    value->setConstantI32(c);
    return value;
}
Value* Builder::getConstantF16(F32 c) {
    Value* value = allocValue(TYPE_I16);
    value->setConstantI16(c);
    return value;
}
Value* Builder::getConstantF32(F32 c) {
    Value* value = allocValue(TYPE_F32);
    value->setConstantF32(c);
    return value;
}
Value* Builder::getConstantV128(V128 c) {
    Value* value = allocValue(TYPE_V128);
    value->setConstantV128(c);
    return value;
}

// Instruction generation
Instruction* Builder::appendInstr(Opcode opcode, OpcodeFlags flags, Value* dest) {
    Instruction* instr = new Instruction();
    instr->parent = ib;
    instr->opcode = opcode;
    instr->flags = flags;
    instr->dest = dest;
    instr->src1.value = nullptr;
    instr->src2.value = nullptr;
    instr->src3.value = nullptr;

    if (dest && dest->parent.instruction == nullptr) {
        dest->parent.instruction = instr;
    }

    ib->instructions.insert(ip, instr);
    return instr;
}

// Arithmetic operations
Value* Builder::createAdd(Value* lhs, Value* rhs, VectorFlags flags) {
    ASSERT_TYPE_INTEGER(lhs);
    ASSERT_TYPE_EQUAL(lhs, rhs);

    if (lhs->isConstantZero()) {
        return rhs;
    } else if (rhs->isConstantZero()) {
        return lhs;
    } else if (lhs->isConstant() && rhs->isConstant()) {
        Value* dest = cloneValue(lhs);
        dest->doAdd(rhs);
        return dest;
    }

    Instruction* i = appendInstr(OPCODE_ADD, flags, allocValue(lhs->type));
    i->src1.setValue(lhs);
    i->src2.setValue(rhs);
    return i->dest;
}

}  // namespace hir
}  // namespace gfx
