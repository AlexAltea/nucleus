/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"
#include "nucleus/emulator.h"

namespace cpu {
namespace ppu {

// Register names
const char* string_gpr[] = {
    "r0_",  "r1_",  "r2_",  "r3_",  "r4_",  "r5_",  "r6_",  "r7_",
    "r8_",  "r9_",  "r10_", "r11_", "r12_", "r13_", "r14_", "r15_",
    "r16_", "r17_", "r18_", "r19_", "r20_", "r21_", "r22_", "r23_",
    "r24_", "r25_", "r26_", "r27_", "r28_", "r29_", "r30_", "r31_",
};

const char* string_fpr[] = {
    "fr0_",  "fr1_",  "fr2_",  "fr3_",  "fr4_",  "fr5_",  "fr6_",  "fr7_",
    "fr8_",  "fr9_",  "fr10_", "fr11_", "fr12_", "fr13_", "fr14_", "fr15_",
    "fr16_", "fr17_", "fr18_", "fr19_", "fr20_", "fr21_", "fr22_", "fr23_",
    "fr24_", "fr25_", "fr26_", "fr27_", "fr28_", "fr29_", "fr30_", "fr31_",
};

const char* string_vr[] = {
    "vr0_",  "vr1_",  "vr2_",  "vr3_",  "vr4_",  "vr5_",  "vr6_",  "vr7_",
    "vr8_",  "vr9_",  "vr10_", "vr11_", "vr12_", "vr13_", "vr14_", "vr15_",
    "vr16_", "vr17_", "vr18_", "vr19_", "vr20_", "vr21_", "vr22_", "vr23_",
    "vr24_", "vr25_", "vr26_", "vr27_", "vr28_", "vr29_", "vr30_", "vr31_",
};

const char* string_cr = "cr_";
const char* string_fpscr = "fpscr_";
const char* string_xer = "xer_";

Recompiler::Recompiler(Segment* segment, Function* function) :
    builder(llvm::getGlobalContext()),
    segment(segment),
    function(function)
{
}

void Recompiler::setInsertPoint(llvm::BasicBlock* block)
{
    builder.SetInsertPoint(block);
}

void Recompiler::createBranch(Block& block)
{
    builder.CreateBr(block.bb);
}

void Recompiler::createReturn()
{
    llvm::Value* ret = nullptr;

    switch (returnType) {
    case FUNCTION_OUT_INTEGER:
        ret = getGPR(3);
        break;
    case FUNCTION_OUT_FLOAT:
        ret = getFPR(1);
        break;
    case FUNCTION_OUT_FLOAT_X2:
        ret = getFPR(1); // TODO
        break;
    case FUNCTION_OUT_FLOAT_X3:
        ret = getFPR(1); // TODO
        break;
    case FUNCTION_OUT_FLOAT_X4:
        ret = getFPR(1); // TODO
        break;
    case FUNCTION_OUT_VECTOR:
        ret = getVR_u32(2);
        break;
    }

    if (returnType == FUNCTION_OUT_VOID) {
        builder.CreateRetVoid();
    } else {
        builder.CreateRet(ret);
    }
}

void Recompiler::createProlog()
{
    // Write LLVM IR at prolog block
    llvm::BasicBlock* prologBlock = function->prolog;
    builder.SetInsertPoint(prologBlock);

    // Place arguments in local variables
    auto argValue = function->function->arg_begin();
    for (int i = 0; i < function->type_in.size(); i++, argValue++) {
        switch (function->type_in[i]) {
        case FUNCTION_IN_INTEGER:
            gpr[3 + i] = allocaVariable(builder.getInt64Ty(), string_gpr[3 + i]);
            builder.CreateStore(argValue, gpr[3 + i]);
            break;
        case FUNCTION_IN_FLOAT:
            fpr[1 + i] = allocaVariable(builder.getDoubleTy(), string_fpr[1 + i]);
            builder.CreateStore(argValue, fpr[1 + i]);
            break;
        case FUNCTION_IN_VECTOR:
            // TODO
            break;
        }
    }

    // Print name of the function and registers r3 to r10
    std::string functionInfo = function->name + "\nr3  = %016llX\nr4  = %016llX\nr5  = %016llX\nr6  = %016llX\nr7  = %016llX\nr8  = %016llX\nr9  = %016llX\nr10 = %016llX\n\n";
    emit_printf(functionInfo.c_str(), std::vector<llvm::Value*>{
        getGPR(3), getGPR(4), getGPR(5), getGPR(6), getGPR(7), getGPR(8), getGPR(9), getGPR(10)
    });

    // Branch to the real entry block
    llvm::BasicBlock* entryBlock = function->blocks[function->address].bb;
    builder.CreateBr(entryBlock);
}

llvm::AllocaInst* Recompiler::allocaVariable(llvm::Type* type, const llvm::Twine& name)
{
    llvm::BasicBlock& entryBlock = function->function->getEntryBlock();
    llvm::IRBuilder<> allocaBuilder(&entryBlock, entryBlock.begin());
    return allocaBuilder.CreateAlloca(type, 0, name);
}

llvm::Value* Recompiler::getGPR(int index, int bits)
{
    if (!gpr[index]) {
        gpr[index] = allocaVariable(builder.getInt64Ty(), string_gpr[index]);
    }

    llvm::Value* reg = builder.CreateLoad(gpr[index], string_gpr[index]);
    if (bits < 64) {
        reg = builder.CreateTrunc(reg, builder.getIntNTy(bits));
    }

    return reg;
}

void Recompiler::setGPR(int index, llvm::Value* value)
{
    if (!gpr[index]) {
        gpr[index] = allocaVariable(builder.getInt64Ty(), string_gpr[index]);
    }
    builder.CreateStore(value, gpr[index]);
}

llvm::Value* Recompiler::getFPR(int index)
{
    if (!fpr[index]) {
        fpr[index] = allocaVariable(builder.getDoubleTy(), string_fpr[index]);
    }
    return builder.CreateLoad(fpr[index], string_fpr[index]);
}

void Recompiler::setFPR(int index, llvm::Value* value)
{
    if (!fpr[index]) {
        fpr[index] = allocaVariable(builder.getDoubleTy(), string_fpr[index]);
    }

    if (value->getType()->isIntegerTy(64)) {
        value = builder.CreateBitCast(value, builder.getDoubleTy());
    }
    if (value->getType()->isIntegerTy(32)) {
        value = builder.CreateBitCast(value, builder.getFloatTy());
    }
    if (value->getType()->isFloatTy()) {
        value = builder.CreateFPExt(value, builder.getDoubleTy());
    }

    builder.CreateStore(value, fpr[index]);
}

llvm::Value* Recompiler::getVR_u8(int index)
{
    // TODO: ?
    return nullptr;
}

llvm::Value* Recompiler::getVR_u16(int index)
{
    // TODO: ?
    return nullptr;
}

llvm::Value* Recompiler::getVR_u32(int index)
{
    // TODO: ?
    return nullptr;
}

llvm::Value* Recompiler::getVR_f32(int index)
{
    // TODO: ?
    return nullptr;
}

void Recompiler::setVR(int index, llvm::Value* value)
{
    // TODO: ?
    return;
}

/**
 * Operation flags
 */
void Recompiler::updateCR0(llvm::Value* value)
{
    llvm::Value* isLT = builder.CreateICmpSLT(value, builder.getInt64(0));
    llvm::Value* isGT = builder.CreateICmpSGT(value, builder.getInt64(0));
    llvm::Value* cr;

    cr = builder.CreateSelect(isGT, builder.getInt8(2), builder.getInt8(4));
    cr = builder.CreateSelect(isLT, builder.getInt8(1), cr);
}

/**
 * Memory access
 */
llvm::Value* Recompiler::readMemory(llvm::Value* addr, int bits)
{
    llvm::Value* baseAddr = builder.CreateLoad(segment->memoryBase, false);
    llvm::Value* value;

    addr = builder.CreateAdd(addr, baseAddr);
    addr = builder.CreateIntToPtr(addr, builder.getIntNTy(bits)->getPointerTo());
    value = builder.CreateLoad(addr);

    // Reverse endianness if necessary
    if (bits > 8) {
        llvm::Function* bswap = getIntrinsicIntN(llvm::Intrinsic::bswap, bits);
        value = builder.CreateCall(bswap, value);
    }

    return value;
}

void Recompiler::writeMemory(llvm::Value* addr, llvm::Value* value)
{
    llvm::Value* baseAddr = builder.CreateLoad(segment->memoryBase, false);

    // Reverse endianness if necessary
    int bits = value->getType()->getIntegerBitWidth();
    if (bits > 8) {
        llvm::Function* bswap = getIntrinsicIntN(llvm::Intrinsic::bswap, bits);
        value = builder.CreateCall(bswap, value);
    }

    addr = builder.CreateAdd(addr, baseAddr);
    addr = builder.CreateIntToPtr(addr, builder.getIntNTy(bits)->getPointerTo());
    builder.CreateStore(value, addr);
}

void Recompiler::emit_printf(const char* format, std::vector<llvm::Value*> args)
{
    llvm::FunctionType* printfType = nullptr;
    llvm::Function* printfFunc = segment->module->getFunction("printf");

    if (!printfFunc) {
        printfType = llvm::FunctionType::get(builder.getInt32Ty(), std::vector<llvm::Type*>{}, true);
        printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", segment->module);
        printfFunc->setCallingConv(llvm::CallingConv::C);
    }

    args.insert(args.begin(), builder.CreateGlobalString(format));
    llvm::CallInst* printfCall = builder.CreateCall(printfFunc, args);
    printfCall->setTailCall(false);
}

}  // namespace ppu
}  // namespace cpu
