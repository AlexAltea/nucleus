/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"
#include "nucleus/cpu/frontend/ppu/ppu_state.h"
#include "nucleus/logger/logger.h"
#include "nucleus/assert.h"
#include "nucleus/config.h"
#include "nucleus/emulator.h"

namespace cpu {
namespace frontend {
namespace ppu {

using namespace cpu::hir;

Recompiler::Recompiler(ppu::Function* function) : IRecompiler<U32>(function)
{
}

void Recompiler::createProlog()
{
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

void Recompiler::createEpilog()
{
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
Value* Recompiler::getGPR(int index, Type type)
{
    const U32 offset = offsetof(State, r[index]);

    // TODO: Use volatility information?
    // Return+Parameter registers and nonvolatile registers are 3 to 10 and 14 onwards respectively

    Value* value = builder.createCtxLoad(offset, TYPE_I64);
    if (type != TYPE_I64) {
        return builder.createTrunc(value, type);
    }
    return value;
}

Value* Recompiler::getFPR(int index, Type type)
{
    const U32 offset = offsetof(State, f[index]);

    // TODO: Use volatility information?
    // Return+Parameter registers and nonvolatile registers are 1 to 13 and 14 onwards respectively

    Value* value = builder.createCtxLoad(offset, TYPE_F64);
    if (type != TYPE_F64) {
        return builder.createConvert(value, type);
    }
    return value;
}

Value* Recompiler::getVR(int index)
{
    return nullptr;
}

Value* Recompiler::getCR(int index)
{
    const U32 offset = offsetof(State, cr);

    // TODO: Use volatility information?

    Value* cr = builder.createCtxLoad(offset, TYPE_I32);
    Value* mask = builder.getConstantI32(0xFULL << (7 - index) * 4);
    cr = builder.createAnd(cr, mask);
    cr = builder.createShr(cr, (7 - index) * 4);
    cr = builder.createTrunc(cr, TYPE_I8);
    return cr;
}

Value* Recompiler::getCR()
{
    const U32 offset = offsetof(State, cr);
    return builder.createCtxLoad(offset, TYPE_I32);
}

Value* Recompiler::getLR()
{
    constexpr U32 offset = offsetof(State, lr);
    return builder.createCtxLoad(offset, TYPE_I64);
}

Value* Recompiler::getXER()
{
    constexpr U32 offset = offsetof(State, xer);
    return builder.createCtxLoad(offset, TYPE_I64);
}

Value* Recompiler::getCTR()
{
    constexpr U32 offset = offsetof(State, ctr);
    return builder.createCtxLoad(offset, TYPE_I64);
}

/**
 * Register write
 */
void Recompiler::setGPR(int index, Value* value)
{
    const U32 offset = offsetof(State, r[index]);

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

void Recompiler::setFPR(int index, Value* value)
{
    const U32 offset = offsetof(State, f[index]);

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

void Recompiler::setVR(int index, Value* value)
{
}

void Recompiler::setCR(int index, Value* value)
{
    const U32 offset = offsetof(State, cr);

    // TODO: Use volatility information?

    Value* cr = builder.createCtxLoad(offset, TYPE_I32);
    Value* mask = builder.getConstantI32(~(0xFULL << (7 - index) * 4));
    cr = builder.createAnd(cr, mask);
    cr = builder.createOr(cr, builder.createShl(builder.createZExt(value, TYPE_I32), (7 - index) * 4));
    builder.createCtxStore(offset, cr);
}

void Recompiler::setCR(Value* value)
{
    constexpr U32 offset = offsetof(State, cr);
    
    if (value->type != TYPE_I32) {
        logger.error(LOG_CPU, "Wrong value type for CR register");
        return;
    }
    builder.createCtxStore(offset, value);
}

void Recompiler::setLR(Value* value)
{
    constexpr U32 offset = offsetof(State, lr);
    
    if (value->type != TYPE_I64) {
        logger.error(LOG_CPU, "Wrong value type for LR register");
        return;
    }
    builder.createCtxStore(offset, value);
}

void Recompiler::setXER(Value* value)
{
    constexpr U32 offset = offsetof(State, xer);
    
    if (value->type != TYPE_I64) {
        logger.error(LOG_CPU, "Wrong value type for XER register");
        return;
    }
    builder.createCtxStore(offset, value);
}

void Recompiler::setCTR(Value* value)
{
    constexpr U32 offset = offsetof(State, ctr);
    
    if (value->type != TYPE_I64) {
        logger.error(LOG_CPU, "Wrong value type for CTR register");
        return;
    }
    builder.createCtxStore(offset, value);
}

/**
 * Memory access
 */
Value* Recompiler::readMemory(hir::Value* addr, hir::Type type) {
    // Get host address
    void* baseAddress = nucleus.memory.getBaseAddr();
    addr = builder.createAdd(addr, builder.getConstantPointer(baseAddress));

    if (type == TYPE_I8) {
        return builder.createLoad(addr, type);
    } else {
        return builder.createLoad(addr, type, ENDIAN_BIG);
    }
}

void Recompiler::writeMemory(Value* addr, Value* value) {
    // Get host address
    void* baseAddress = nucleus.memory.getBaseAddr();
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

    result = builder.createSelect(isGT, builder.getConstantI8(4), builder.getConstantI8(2));
    result = builder.createSelect(isLT, builder.getConstantI8(8), result);
    setCR(field, result);
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
