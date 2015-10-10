/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "builder.h"
#include "nucleus/cpu/util.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/hir/function.h"
#include "nucleus/cpu/hir/instruction.h"
#include "nucleus/assert.h"

#define ASSERT_TYPE_EQUAL(value1, value2) \
    assert_true(value1->type == value2->type)

#define ASSERT_TYPE_INTEGER(value) \
    assert_true(value->isTypeInteger())
#define ASSERT_TYPE_FLOAT(value) \
    assert_true(value->isTypeFloat())
#define ASSERT_TYPE_VECTOR(value) \
    assert_true(value->isTypeVector())

namespace cpu {
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
    value->reg = 0;
    return value;
}

Value* Builder::cloneValue(Value* source) {
    assert_true(source->isConstant(), "Builder only supports cloning constants");

    Value* value = new Value();
    value->type = source->type;
    value->flags = source->flags;
    
    if (source->isConstant()) {
        switch (source->type) {
        case TYPE_I8:  value->constant.i8  = source->constant.i8;  break;
        case TYPE_I16: value->constant.i16 = source->constant.i16; break;
        case TYPE_I32: value->constant.i32 = source->constant.i32; break;
        case TYPE_I64: value->constant.i64 = source->constant.i64; break;
        case TYPE_F32: value->constant.f32 = source->constant.f32; break;
        case TYPE_F64: value->constant.f64 = source->constant.f64; break;
        }
    }
    return value;
}

// HIR constants
Value* Builder::getConstantI8(U8 c) {
    Value* value = allocValue(TYPE_I8);
    value->setConstantI8(c);
    return value;
}
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
Value* Builder::getConstantI64(U64 c) {
    Value* value = allocValue(TYPE_I64);
    value->setConstantI64(c);
    return value;
}
Value* Builder::getConstantF32(F32 c) {
    Value* value = allocValue(TYPE_F32);
    value->setConstantF32(c);
    return value;
}
Value* Builder::getConstantF64(F64 c) {
    Value* value = allocValue(TYPE_F64);
    value->setConstantF64(c);
    return value;
}
Value* Builder::getConstantV128(V128 c) {
    Value* value = allocValue(TYPE_V128);
    value->setConstantV128(c);
    return value;
}
Value* Builder::getConstantV256(V256 c) {
    Value* value = allocValue(TYPE_V256);
    value->setConstantV256(c);
    return value;
}
Value* Builder::getConstantPointer(void* c) {
    Value* value = allocValue(TYPE_PTR);
    value->setConstantI64(reinterpret_cast<U64>(c));
    return value;
}

// HIR functions
Function* Builder::getExternFunction(void* hostAddr) {
    hir::Function* parFunction = ib->parent;
    hir::Module* parModule = parFunction->parent;

    // Check builtin functions
    Function* externFunc;
    if (hostAddr == nucleusTranslate) {
        externFunc = new Function(parModule, TYPE_VOID, {TYPE_PTR, TYPE_I64});
    } else if (hostAddr == nucleusCall) {
        externFunc = new Function(parModule, TYPE_VOID, {TYPE_I64});
    } else if (hostAddr == nucleusSysCall) {
        externFunc = new Function(parModule, TYPE_VOID, {});
    } else if (hostAddr == nucleusLog) {
        externFunc = new Function(parModule, TYPE_VOID, {TYPE_I64});
    } else if (hostAddr == nucleusTime) {
        externFunc = new Function(parModule, TYPE_I64, {});
    }

    externFunc->flags |= FUNCTION_IS_EXTERN;
    externFunc->nativeAddress = hostAddr;
    return externFunc;
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
Value* Builder::createAdd(Value* lhs, Value* rhs) {
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

    Instruction* i = appendInstr(OPCODE_ADD, 0, allocValue(lhs->type));
    i->src1.setValue(lhs);
    i->src2.setValue(rhs);
    return i->dest;
}

Value* Builder::createSub(Value* lhs, Value* rhs) {
    ASSERT_TYPE_INTEGER(lhs);
    ASSERT_TYPE_EQUAL(lhs, rhs);

    if (rhs->isConstantZero()) {
        return lhs;
    } else if (lhs->isConstant() && rhs->isConstant()) {
        Value* dest = cloneValue(lhs);
        dest->doSub(rhs);
        return dest;
    }

    Instruction* i = appendInstr(OPCODE_SUB, 0, allocValue(lhs->type));
    i->src1.setValue(lhs);
    i->src2.setValue(rhs);
    return i->dest;
}

Value* Builder::createMul(Value* lhs, Value* rhs, ArithmeticFlags flags) {
    ASSERT_TYPE_INTEGER(lhs);
    ASSERT_TYPE_EQUAL(lhs, rhs);

    if (lhs->isConstant() && rhs->isConstant()) {
        Value* dest = cloneValue(lhs);
        dest->doMul(rhs, flags);
        return dest;
    }

    Instruction* i = appendInstr(OPCODE_MUL, flags, allocValue(lhs->type));
    i->src1.setValue(lhs);
    i->src2.setValue(rhs);
    return i->dest;
}

Value* Builder::createMulH(Value* lhs, Value* rhs, ArithmeticFlags flags) {
    ASSERT_TYPE_INTEGER(lhs);
    ASSERT_TYPE_EQUAL(lhs, rhs);

    if (lhs->isConstant() && rhs->isConstant()) {
        Value* dest = cloneValue(lhs);
        dest->doMulH(rhs, flags);
        return dest;
    }

    Instruction* i = appendInstr(OPCODE_MULH, flags, allocValue(lhs->type));
    i->src1.setValue(lhs);
    i->src2.setValue(rhs);
    return i->dest;
}

Value* Builder::createDiv(Value* lhs, Value* rhs, ArithmeticFlags flags) {
    ASSERT_TYPE_INTEGER(lhs);
    ASSERT_TYPE_EQUAL(lhs, rhs);

    if (lhs->isConstant() && rhs->isConstant()) {
        Value* dest = cloneValue(lhs);
        dest->doDiv(rhs, flags);
        return dest;
    }

    Instruction* i = appendInstr(OPCODE_DIV, flags, allocValue(lhs->type));
    i->src1.setValue(lhs);
    i->src2.setValue(rhs);
    return i->dest;
}

Value* Builder::createNeg(Value* value) {
    ASSERT_TYPE_INTEGER(value);

    if (value->isConstant()) {
        Value* dest = cloneValue(value);
        dest->doNeg();
        return dest;
    }

    Instruction* i = appendInstr(OPCODE_NEG, 0, allocValue(value->type));
    i->src1.setValue(value);
    return i->dest;
}

Value* Builder::createCtlz(Value* value) {
    ASSERT_TYPE_INTEGER(value);

    Instruction* i = appendInstr(OPCODE_CTLZ, 0, allocValue(TYPE_I8));
    i->src1.setValue(value);
    return i->dest;
}

Value* Builder::createZExt(Value* value, Type type) {
    if (value->type == type) {
        return value;
    } else if (value->isConstant()) {
        Value* dest = cloneValue(value);
        dest->doZExt(type);
        return dest;
    }

    Instruction* i = appendInstr(OPCODE_ZEXT, 0, allocValue(type));
    i->src1.setValue(value);
    return i->dest;
}

Value* Builder::createSExt(Value* value, Type type) {
    if (value->type == type) {
        return value;
    } else if (value->isConstant()) {
        Value* dest = cloneValue(value);
        dest->doSExt(type);
        return dest;
    }

    Instruction* i = appendInstr(OPCODE_SEXT, 0, allocValue(type));
    i->src1.setValue(value);
    return i->dest;
}

Value* Builder::createTrunc(Value* value, Type type) {
    if (value->type == type) {
        return value;
    } else if (value->isConstant()) {
        Value* dest = cloneValue(value);
        dest->doTrunc(type);
        return dest;
    }

    Instruction* i = appendInstr(OPCODE_TRUNC, 0, allocValue(type));
    i->src1.setValue(value);
    return i->dest;
}

Value* Builder::createSqrt(Value* value) {
    return nullptr;
}

Value* Builder::createAbs(Value* value) {
    return nullptr;
}

// Logical operations
Value* Builder::createAnd(Value* lhs, Value* rhs) {
    ASSERT_TYPE_EQUAL(lhs, rhs);

    if (lhs == rhs) {
        return lhs;
    } else if (lhs->isConstantZero()) {
        return lhs;
    } else if (rhs->isConstantZero()) {
        return rhs;
    } else if (lhs->isConstant() && rhs->isConstant()) {
        Value* dest = cloneValue(lhs);
        dest->doAnd(rhs);
        return dest;
    }

    Instruction* i = appendInstr(OPCODE_AND, 0, allocValue(lhs->type));
    i->src1.setValue(lhs);
    i->src2.setValue(rhs);
    return i->dest;
}

Value* Builder::createOr(Value* lhs, Value* rhs) {
    ASSERT_TYPE_EQUAL(lhs, rhs);

    if (lhs == rhs) {
        return lhs;
    } else if (lhs->isConstantZero()) {
        return rhs;
    } else if (rhs->isConstantZero()) {
        return lhs;
    } else if (lhs->isConstant() && rhs->isConstant()) {
        Value* dest = cloneValue(lhs);
        dest->doOr(rhs);
        return dest;
    }

    Instruction* i = appendInstr(OPCODE_OR, 0, allocValue(lhs->type));
    i->src1.setValue(lhs);
    i->src2.setValue(rhs);
    return i->dest;
}

Value* Builder::createXor(Value* lhs, Value* rhs) {
    ASSERT_TYPE_EQUAL(lhs, rhs);

    if (lhs == rhs) {
        // TODO
    } else if (lhs->isConstantZero()) {
        return lhs;
    } else if (rhs->isConstantZero()) {
        return rhs;
    } else if (lhs->isConstant() && rhs->isConstant()) {
        Value* dest = cloneValue(lhs);
        dest->doXor(rhs);
        return dest;
    }

    Instruction* i = appendInstr(OPCODE_XOR, 0, allocValue(lhs->type));
    i->src1.setValue(lhs);
    i->src2.setValue(rhs);
    return i->dest;
}

Value* Builder::createNot(Value* value) {
    if (value->isConstant()) {
        Value* dest = cloneValue(value);
        dest->doNot();
        return dest;
    }
    
    Instruction* i = appendInstr(OPCODE_NOT, 0, allocValue(value->type));
    i->src1.setValue(value);
    return i->dest;
}

// Shifting operations
Value* Builder::createShl(Value* value, Value* amount) {
    ASSERT_TYPE_INTEGER(value);
    ASSERT_TYPE_INTEGER(amount);

    if (amount->isConstantZero()) {
        return value;
    }
    if (value->isConstant() && amount->isConstant()) {
        Value* dest = cloneValue(value);
        dest->doShl(amount);
        return dest;
    }
    if (amount->type != TYPE_I8) {
        amount = createTrunc(amount, TYPE_I8);
    }

    Instruction* i = appendInstr(OPCODE_SHL, 0, allocValue(value->type));
    i->src1.setValue(value);
    i->src2.setValue(amount);
    return i->dest;
}

Value* Builder::createShl(Value* value, U64 rhs) {
    return createShl(value, getConstantI8(rhs));
}

Value* Builder::createShr(Value* value, Value* amount) {
    ASSERT_TYPE_INTEGER(value);
    ASSERT_TYPE_INTEGER(amount);

    if (amount->isConstantZero()) {
        return value;
    }
    if (value->isConstant() && amount->isConstant()) {
        Value* dest = cloneValue(value);
        dest->doShr(amount);
        return dest;
    }
    if (amount->type != TYPE_I8) {
        amount = createTrunc(amount, TYPE_I8);
    }

    Instruction* i = appendInstr(OPCODE_SHR, 0, allocValue(value->type));
    i->src1.setValue(value);
    i->src2.setValue(amount);
    return i->dest;
}

Value* Builder::createShr(Value* value, U64 rhs) {
    return createShr(value, getConstantI8(rhs));
}

Value* Builder::createShrA(Value* value, Value* amount) {
    ASSERT_TYPE_INTEGER(value);
    ASSERT_TYPE_INTEGER(amount);

    if (amount->isConstantZero()) {
        return value;
    }
    if (value->isConstant() && amount->isConstant()) {
        Value* dest = cloneValue(value);
        dest->doShrA(amount);
        return dest;
    }
    if (amount->type != TYPE_I8) {
        amount = createTrunc(amount, TYPE_I8);
    }

    Instruction* i = appendInstr(OPCODE_SHRA, 0, allocValue(value->type));
    i->src1.setValue(value);
    i->src2.setValue(amount);
    return i->dest;
}

Value* Builder::createShrA(Value* value, U64 rhs) {
    return createShrA(value, getConstantI8(rhs));
}

// Memory access operations
Value* Builder::createLoad(Value* address, Type type, MemoryFlags flags) {
    Instruction* i = appendInstr(OPCODE_LOAD, flags, allocValue(type));
    i->src1.setValue(address);
    return i->dest;
}

void Builder::createStore(Value* address, Value* value, MemoryFlags flags) {
    Instruction* i = appendInstr(OPCODE_STORE, flags);
    i->src1.setValue(address);
    i->src2.setValue(value);
}

Value* Builder::createCtxLoad(U32 offset, Type type) {
    Instruction* i = appendInstr(OPCODE_CTXLOAD, 0, allocValue(type));
    i->src1.immediate = offset;
    return i->dest;
}

void Builder::createCtxStore(U32 offset, Value* value) {
    Instruction* i = appendInstr(OPCODE_CTXSTORE, 0);
    i->src1.immediate = offset;
    i->src2.setValue(value);
}

void Builder::createMemFence() {
    Instruction* i = appendInstr(OPCODE_MEMFENCE, 0);
}

// Comparison operations
Value* Builder::createCmp(Value* lhs, Value* rhs, CompareFlags flags) {
    ASSERT_TYPE_EQUAL(lhs, rhs);

    if (lhs->isConstant() && lhs->isConstant()) {
        Value* dest = cloneValue(lhs);
        dest->doCompare(rhs, flags);
        return dest;
    }

    Instruction* i = appendInstr(OPCODE_CMP, flags, allocValue(TYPE_I8));
    i->src1.setValue(lhs);
    i->src2.setValue(rhs);
    return i->dest;
}

Value* Builder::createCmpEQ(Value* lhs, Value* rhs) {
    return createCmp(lhs, rhs, COMPARE_EQ);
}

Value* Builder::createCmpNE(Value* lhs, Value* rhs) {
    return createCmp(lhs, rhs, COMPARE_NE);
}

Value* Builder::createCmpSLT(Value* lhs, Value* rhs) {
    return createCmp(lhs, rhs, COMPARE_SLT);
}

Value* Builder::createCmpSLE(Value* lhs, Value* rhs) {
    return createCmp(lhs, rhs, COMPARE_SLE);
}

Value* Builder::createCmpSGE(Value* lhs, Value* rhs) {
    return createCmp(lhs, rhs, COMPARE_SGE);
}

Value* Builder::createCmpSGT(Value* lhs, Value* rhs) {
    return createCmp(lhs, rhs, COMPARE_SGT);
}

Value* Builder::createCmpULT(Value* lhs, Value* rhs) {
    return createCmp(lhs, rhs, COMPARE_ULT);
}

Value* Builder::createCmpULE(Value* lhs, Value* rhs) {
    return createCmp(lhs, rhs, COMPARE_ULE);
}

Value* Builder::createCmpUGE(Value* lhs, Value* rhs) {
    return createCmp(lhs, rhs, COMPARE_UGE);
}

Value* Builder::createCmpUGT(Value* lhs, Value* rhs) {
    return createCmp(lhs, rhs, COMPARE_UGT);
}

void Builder::createBr(Block* block) {
    Instruction* i = appendInstr(OPCODE_BR, 0);
    i->src1.block = block;
}

void Builder::createBrCond(Value* cond, Block* blockTrue, Block* blockFalse) {
    Instruction* i = appendInstr(OPCODE_BRCOND, 0);
    i->src1.setValue(cond);
    i->src2.block = blockTrue;
}

Value* Builder::createCall(Function* function, const std::vector<Value*>& args, CallFlags flags) {
    // Checks
    assert_true(args.size() == function->typeIn.size());

    // Place arguments
    for (int index = 0; index < args.size(); index++) {
        assert_true(args[index]->type == function->typeIn[index]);
        Instruction* i = appendInstr(OPCODE_ARG, 0, allocValue(function->typeIn[index]));
        i->src1.immediate = index;
        i->src2.setValue(args[index]);
    }
    // Call function
    Instruction* i;
    if (function->typeOut == TYPE_VOID) {
        i = appendInstr(OPCODE_CALL, flags);
    } else {
        i = appendInstr(OPCODE_CALL, flags, allocValue(function->typeOut));
    }
    i->src1.function = function;
    return i->dest;
}

Value* Builder::createCallCond(Value* cond, Function* function, const std::vector<Value*>& args, CallFlags flags) {
    // Checks
    assert_true(args.size() == function->typeIn.size());

    // Place arguments
    for (int index = 0; index < args.size(); index++) {
        assert_true(args[index]->type == function->typeIn[index]);
        Instruction* i = appendInstr(OPCODE_ARG, 0, allocValue(function->typeIn[index]));
        i->src1.immediate = index;
        i->src2.setValue(args[index]);
    }
    // Call function
    Instruction* i;
    if (function->typeOut == TYPE_VOID) {
        i = appendInstr(OPCODE_CALL, flags);
    } else {
        i = appendInstr(OPCODE_CALL, flags, allocValue(function->typeOut));
    }
    i->src1.setValue(cond);
    i->src2.function = function;
    return i->dest;
}

Value* Builder::createSelect(Value* cond, Value* valueTrue, Value* valueFalse) {
    ASSERT_TYPE_EQUAL(valueTrue, valueFalse);

    if (cond->isConstant()) {
        return cond->isConstantTrue() ? valueTrue : valueFalse;
    }

    Instruction* i = appendInstr(OPCODE_SELECT, 0, allocValue(valueTrue->type));
    i->src1.setValue(cond);
    i->src2.setValue(valueTrue);
    i->src3.setValue(valueFalse);
    return i->dest;
}

void Builder::createRet(Value* value) {
    Instruction* i = appendInstr(OPCODE_RET, 0);
    i->src1.setValue(value);
}

void Builder::createRet() {
    Instruction* i = appendInstr(OPCODE_RET, 0);
}

// Floating-point operations
Value* Builder::createFAdd(Value* lhs, Value* rhs) {
    return nullptr;
}

Value* Builder::createFSub(Value* lhs, Value* rhs) {
    return nullptr;
}

Value* Builder::createFMul(Value* lhs, Value* rhs) {
    return nullptr;
}

Value* Builder::createFDiv(Value* lhs, Value* rhs) {
    return nullptr;
}

Value* Builder::createFNeg(Value* value) {
    return nullptr;
}

Value* Builder::createFExt(Value* value, Type type) {
    return nullptr;
}

Value* Builder::createFTrunc(Value* value, Type type) {
    return nullptr;
}

// Vector operations
Value* Builder::createVAdd(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

Value* Builder::createVSub(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

Value* Builder::createVMul(Value* lhs, Value* rhs, Type compType, ArithmeticFlags flags) {
    return nullptr;
}

Value* Builder::createVAvg(Value* lhs, Value* rhs, Type compType, ArithmeticFlags flags) {
    return nullptr;
}

Value* Builder::createVAbs(Value* value, Type compType) {
    return nullptr;
}

Value* Builder::createVCmpEQ(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

Value* Builder::createVCmpNE(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

Value* Builder::createVCmpFLT(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

Value* Builder::createVCmpFLE(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

Value* Builder::createVCmpFGT(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

Value* Builder::createVCmpFGE(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

Value* Builder::createVCmpSLT(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

Value* Builder::createVCmpSLE(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

Value* Builder::createVCmpSGT(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

Value* Builder::createVCmpSGE(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

Value* Builder::createVCmpULT(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

Value* Builder::createVCmpULE(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

Value* Builder::createVCmpUGT(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

Value* Builder::createVCmpUGE(Value* lhs, Value* rhs, Type compType) {
    return nullptr;
}

}  // namespace hir
}  // namespace cpu
