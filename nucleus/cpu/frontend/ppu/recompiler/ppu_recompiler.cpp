/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"
#include "nucleus/cpu/frontend/ppu/ppu_state.h"
#include "nucleus/memory/memory.h"
#include "nucleus/core/config.h"
#include "nucleus/logger/logger.h"
#include "nucleus/assert.h"

namespace cpu {
namespace frontend {
namespace ppu {

using namespace cpu::hir;

Recompiler::Recompiler(CPU* parent, ppu::Function* function) : parent(parent), IRecompiler<U32>(function) {
}

void Recompiler::createProlog() {
    /*builder.SetInsertPoint(prolog);

    // Place arguments in local variables
    auto argValue = function->function.function->arg_begin();

    auto ppuFunc = static_cast<Function*>(function);
    for (int i = 0; i < ppuFunc->type_in.size(); i++, argValue++) {
        switch (ppuFunc->type_in[i]) {
        case FUNCTION_IN_INTEGER:
            gpr[3 + i] = builder.createAlloca<I64>(format("r%d_", 3 + i));
            builder.createStore(Value*{argValue}, gpr[3 + i]);
            break;
        case FUNCTION_IN_FLOAT:
            fpr[1 + i] = builder.createAlloca<F64>(format("r%d_", 1 + i));
            builder.createStore(Value*{argValue}, fpr[1 + i]);
            break;
        case FUNCTION_IN_VECTOR:
            // TODO
            break;
        }
    }

    state = builder.createCall(static_cast<Segment*>(function->parent)->funcGetState);
    builder.createCall(static_cast<Segment*>(function->parent)->funcLogState, {builder.getConstantI64(function->address)});

    // Branch to the real entry block
    hir::Block entryBlock = blocks.at(function->address);
    builder.createBr(entryBlock);*/
}

void Recompiler::createEpilog() {
    assert_true(epilog == nullptr, "The frontend epilog block was already declared");

    epilog = new hir::Block(function->hirFunction);
    builder.setInsertPoint(epilog);

    if (config.ppuTranslator != CPU_TRANSLATOR_MODULE) {
        builder.createRet();
    } else {
        auto ppuFunc = static_cast<Function*>(function);
        switch (ppuFunc->type_out) {
        case FUNCTION_OUT_INTEGER:
            builder.createRet(getGPR(3));
            break;
        case FUNCTION_OUT_FLOAT:
            builder.createRet(getFPR(1));
            break;
        case FUNCTION_OUT_FLOAT_X2:
            builder.createRet(getFPR(1)); // TODO
            break;
        case FUNCTION_OUT_FLOAT_X3:
            builder.createRet(getFPR(1)); // TODO
            break;
        case FUNCTION_OUT_FLOAT_X4:
            builder.createRet(getFPR(1)); // TODO
            break;
        case FUNCTION_OUT_VECTOR:
            builder.createRet(getVR(2));
            break;
        case FUNCTION_OUT_VOID:
            builder.createRet();
            break;

        default:
            logger.error(LOG_CPU, "Recompiler::createReturn: Invalid returnType");
            builder.createRet();
            break;
        }
    }
}

/**
 * Register read
 */
Value* Recompiler::getGPR(int index, Type type) {
    const U32 offset = offsetof(PPUState, r[index]);

    // TODO: Use volatility information?
    // Return+Parameter registers and nonvolatile registers are 3 to 10 and 14 onwards respectively

    Value* value = builder.createCtxLoad(offset, TYPE_I64);
    if (type != TYPE_I64) {
        return builder.createTrunc(value, type);
    }
    return value;
}

Value* Recompiler::getFPR(int index, Type type) {
    const U32 offset = offsetof(PPUState, f[index]);

    // TODO: Use volatility information?
    // Return+Parameter registers and nonvolatile registers are 1 to 13 and 14 onwards respectively

    Value* value = builder.createCtxLoad(offset, TYPE_F64);
    if (type != TYPE_F64) {
        return builder.createConvert(value, type);
    }
    return value;
}

Value* Recompiler::getVR(int index) {
    const U32 offset = offsetof(PPUState, v[index]);

    // TODO: Use volatility information?

    return builder.createCtxLoad(offset, TYPE_V128);
}

Value* Recompiler::getCRField(int index) {
    // TODO: Use volatility information?

    Value* field = builder.createShl(builder.createCtxLoad(
        offsetof(PPUState, cr.field[index].bit[0]), TYPE_I8), U8(3));
    field = builder.createOr(field, builder.createShl(builder.createCtxLoad(
        offsetof(PPUState, cr.field[index].bit[1]), TYPE_I8), U8(2)));
    field = builder.createOr(field, builder.createShl(builder.createCtxLoad(
        offsetof(PPUState, cr.field[index].bit[2]), TYPE_I8), U8(1)));
    field = builder.createOr(field, builder.createShl(builder.createCtxLoad(
        offsetof(PPUState, cr.field[index].bit[3]), TYPE_I8), U8(0)));

    return field;
}

Value* Recompiler::getCRBit(int index) {
    const U32 offset = offsetof(PPUState, cr.field[index >> 2].bit[index & 0b11]);

     // TODO: Use volatility information?

    return builder.createCtxLoad(offset, TYPE_I8);
}

Value* Recompiler::getCR() {
    Value* cr = builder.getConstantI32(0);
    for (int field = 0; field < 8; field++) {
        cr = builder.createOr(cr, builder.createShl(
            builder.createZExt(getCRField(field), TYPE_I32), 4 * (7 - field)));
    }
    return cr;
}

Value* Recompiler::getLR() {
    constexpr U32 offset = offsetof(PPUState, lr);
    return builder.createCtxLoad(offset, TYPE_I64);
}

Value* Recompiler::getXER() {
    Value* xer = builder.getConstantI64(0);
    xer = builder.createOr(xer, builder.createShl(builder.createZExt(getXER_SO(), TYPE_I64), U8(31)));
    xer = builder.createOr(xer, builder.createShl(builder.createZExt(getXER_OV(), TYPE_I64), U8(30)));
    xer = builder.createOr(xer, builder.createShl(builder.createZExt(getXER_CA(), TYPE_I64), U8(29)));
    xer = builder.createOr(xer, builder.createShl(builder.createZExt(getXER_BC(), TYPE_I64), U8(0)));
    return xer;
}

Value* Recompiler::getXER_SO() {
    constexpr U32 offset = offsetof(PPUState, xer.so);
    return builder.createCtxLoad(offset, TYPE_I8);
}

Value* Recompiler::getXER_OV() {
    constexpr U32 offset = offsetof(PPUState, xer.ov);
    return builder.createCtxLoad(offset, TYPE_I8);
}

Value* Recompiler::getXER_CA() {
    constexpr U32 offset = offsetof(PPUState, xer.ca);
    return builder.createCtxLoad(offset, TYPE_I8);
}

Value* Recompiler::getXER_BC() {
    constexpr U32 offset = offsetof(PPUState, xer.bc);
    return builder.createCtxLoad(offset, TYPE_I8);
}

Value* Recompiler::getCTR() {
    constexpr U32 offset = offsetof(PPUState, ctr);
    return builder.createCtxLoad(offset, TYPE_I64);
}

Value* Recompiler::getFPSCR() {
    constexpr U32 offset = offsetof(PPUState, fpscr);
    return builder.createCtxLoad(offset, TYPE_I32);
}

/**
 * Register write
 */
void Recompiler::setGPR(int index, Value* value) {
    const U32 offset = offsetof(PPUState, r[index]);

    // TODO: Use volatility information?
    // Return+Parameter registers and nonvolatile registers are 3 to 10 and 14 onwards respectively

    Value* value_i64;
    if (value->type != TYPE_I64) {
        value_i64 = builder.createZExt(value, TYPE_I64);
    } else {
        value_i64 = value;
    }

    builder.createCtxStore(offset, value_i64);
}

void Recompiler::setFPR(int index, Value* value) {
    const U32 offset = offsetof(PPUState, f[index]);

    // TODO: Use volatility information?
    // Return+Parameter registers and nonvolatile registers are 1 to 13 and 14 onwards respectively

    Value* value_f64;
    if (value->type != TYPE_F64) {
        value_f64 = builder.createConvert(value, TYPE_F64);
    } else {
        value_f64 = value;
    }

    builder.createCtxStore(offset, value_f64);
}

void Recompiler::setVR(int index, Value* value) {
    const U32 offset = offsetof(PPUState, v[index]);

    // TODO: Use volatility information?

    builder.createCtxStore(offset, value);
}

void Recompiler::setCRField(int index, Value* value) {
    // TODO: Use volatility information?

    switch (value->type) {
    // Unpack and store the value bits
    case TYPE_I8:
        builder.createCtxStore(offsetof(PPUState, cr.field[index].bit[0]),
            builder.createAnd(builder.createShr(value, U8(3)), builder.getConstantI8(1)));
        builder.createCtxStore(offsetof(PPUState, cr.field[index].bit[1]),
            builder.createAnd(builder.createShr(value, U8(2)), builder.getConstantI8(1)));
        builder.createCtxStore(offsetof(PPUState, cr.field[index].bit[2]),
            builder.createAnd(builder.createShr(value, U8(1)), builder.getConstantI8(1)));
        builder.createCtxStore(offsetof(PPUState, cr.field[index].bit[3]),
            builder.createAnd(builder.createShr(value, U8(0)), builder.getConstantI8(1)));
        break;

    // Store the unpacked value directly
    case TYPE_I32:
        builder.createCtxStore(offsetof(PPUState, cr.field[index]), value);
        break;

    default:
        logger.error(LOG_CPU, "Wrong value type for CR register");
    }
}

void Recompiler::setCRBit(int index, Value* value) {
    const U32 offset = offsetof(PPUState, cr.field[index >> 2].bit[index & 0b11]);

     // TODO: Use volatility information?

    builder.createCtxStore(offset, value);
}

void Recompiler::setCR(Value* value) {
    if (value->type != TYPE_I32) {
        logger.error(LOG_CPU, "Wrong value type for CR register");
        return;
    }

    for (int field = 0; field < 8; field++) {
        setCRField(field, builder.createTrunc(builder.createShr(value, (4 * (7 - field))), TYPE_I8));
    }
}

void Recompiler::setLR(Value* value) {
    constexpr U32 offset = offsetof(PPUState, lr);

    if (value->type != TYPE_I64) {
        logger.error(LOG_CPU, "Wrong value type for LR register");
        return;
    }
    builder.createCtxStore(offset, value);
}

void Recompiler::setXER(Value* value) {
    assert_true(value->type == TYPE_I64, "Wrong value type for XER register");
    constexpr U32 offset_so = offsetof(PPUState, xer.so);
    constexpr U32 offset_ov = offsetof(PPUState, xer.ov);
    constexpr U32 offset_ca = offsetof(PPUState, xer.ca);
    constexpr U32 offset_bc = offsetof(PPUState, xer.bc);

    Value* bc_i64 = builder.createAnd(value, builder.getConstantI64(0x7F));
    builder.createCtxStore(offset_bc, builder.createTrunc(bc_i64, TYPE_I8));

    Value* ca_i64 = builder.createAnd(builder.createShr(value, 29), builder.getConstantI64(1));
    builder.createCtxStore(offset_ca, builder.createTrunc(ca_i64, TYPE_I8));

    Value* ov_i64 = builder.createAnd(builder.createShr(value, 30), builder.getConstantI64(1));
    builder.createCtxStore(offset_ov, builder.createTrunc(ov_i64, TYPE_I8));

    Value* so_i64 = builder.createAnd(builder.createShr(value, 31), builder.getConstantI64(1));
    builder.createCtxStore(offset_so, builder.createTrunc(so_i64, TYPE_I8));
}

void Recompiler::setXER_SO(Value* value) {
    assert_true(value->type == TYPE_I8, "Wrong value type for XER::SO field");
    constexpr U32 offset = offsetof(PPUState, xer.so);
    builder.createCtxStore(offset, value);
}

void Recompiler::setXER_OV(Value* value) {
    assert_true(value->type == TYPE_I8, "Wrong value type for XER::OV field");
    constexpr U32 offset = offsetof(PPUState, xer.ov);
    builder.createCtxStore(offset, value);
}

void Recompiler::setXER_CA(Value* value) {
    assert_true(value->type == TYPE_I8, "Wrong value type for XER::CA field");
    constexpr U32 offset = offsetof(PPUState, xer.ca);
    builder.createCtxStore(offset, value);
}

void Recompiler::setXER_BC(Value* value) {
    assert_true(value->type == TYPE_I8, "Wrong value type for XER::CA field");
    constexpr U32 offset = offsetof(PPUState, xer.bc);
    builder.createCtxStore(offset, value);
}

void Recompiler::setCTR(Value* value) {
    constexpr U32 offset = offsetof(PPUState, ctr);

    if (value->type != TYPE_I64) {
        logger.error(LOG_CPU, "Wrong value type for CTR register");
        return;
    }
    builder.createCtxStore(offset, value);
}

void Recompiler::setFPSCR(Value* value) {
    constexpr U32 offset = offsetof(PPUState, fpscr);

    if (value->type != TYPE_I32) {
        logger.error(LOG_CPU, "Wrong value type for FPSCR register");
        return;
    }
    builder.createCtxStore(offset, value);
}

/**
 * Memory access
 */
Value* Recompiler::readMemory(hir::Value* addr, hir::Type type) {
    // Get host address
    void* baseAddress = parent->memory->getBaseAddr();
    addr = builder.createAdd(addr, builder.getConstantPointer(baseAddress));

    if (type == TYPE_I8) {
        return builder.createLoad(addr, type);
    } else {
        return builder.createLoad(addr, type, ENDIAN_BIG);
    }
}

void Recompiler::writeMemory(Value* addr, Value* value) {
    // Get host address
    void* baseAddress = parent->memory->getBaseAddr();
    addr = builder.createAdd(addr, builder.getConstantPointer(baseAddress));

    if (value->type == TYPE_I8) {
        builder.createStore(addr, value);
    } else {
        builder.createStore(addr, value, ENDIAN_BIG);
    }
}

/**
 * Operation flags
 */
void Recompiler::updateCR(int field, Value* lhs, Value* rhs, bool logicalComparison) {
    Value* isLT;
    Value* isGT;
    Value* result;

    if (logicalComparison) {
        isLT = builder.createCmpULT(lhs, rhs);
        isGT = builder.createCmpUGT(lhs, rhs);
    } else {
        isLT = builder.createCmpSLT(lhs, rhs);
        isGT = builder.createCmpSGT(lhs, rhs);
    }

    result = builder.createSelect(isGT, builder.getConstantI32(0x00000100), builder.getConstantI32(0x00010000));
    result = builder.createSelect(isLT, builder.getConstantI32(0x00000001), result);
    setCRField(field, result);
}

void Recompiler::updateCR0(Value* value) {
    updateCR(0, value, builder.getConstantI64(0), false);
}

void Recompiler::updateCR1(Value* value) {
}

void Recompiler::updateCR6(Value* value) {
}

/**
 * Branching
 */
void Recompiler::createFunctionCall(U32 nia, Value* condition) {
    auto* module = function->parent;
    auto& targetFunc = static_cast<Function&>(*module->functions.at(nia));

    // Generate array of arguments
    int index = 0;
    std::vector<Value*> arguments;
    for (const auto& param : targetFunc.type_in) {
        switch (param) {
        case FUNCTION_IN_INTEGER:
            arguments.push_back(getGPR(3 + index));
            break;
        case FUNCTION_IN_FLOAT:
            arguments.push_back(getFPR(1 + index));
            break;
        case FUNCTION_IN_VECTOR:
            arguments.push_back(getVR(2 + index));
            break;
        }
        index += 1;
    }

    Value* result;
    if (condition) {
        result = builder.createCallCond(condition, targetFunc.hirFunction, arguments);
    } else {
        result = builder.createCall(targetFunc.hirFunction, arguments);
    }

    // Save return value
    switch (targetFunc.type_out) {
    case FUNCTION_OUT_INTEGER:
        setGPR(3, result);
        break;
    case FUNCTION_OUT_FLOAT:
        setFPR(1, result);
        break;
    case FUNCTION_OUT_FLOAT_X2:
        setFPR(1, result); // TODO
        break;
    case FUNCTION_OUT_FLOAT_X3:
        setFPR(1, result); // TODO
        break;
    case FUNCTION_OUT_FLOAT_X4:
        setFPR(1, result); // TODO
        break;
    case FUNCTION_OUT_VECTOR:
        setVR(2, result);
        break;
    }
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
