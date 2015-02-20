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
    "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6",  "r7",
    "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15", 
    "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", 
    "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",
};

const char* string_fpr[] = {
    "fr0",  "fr1",  "fr2",  "fr3",  "fr4",  "fr5",  "fr6",  "fr7",
    "fr8",  "fr9",  "fr10", "fr11", "fr12", "fr13", "fr14", "fr15", 
    "fr16", "fr17", "fr18", "fr19", "fr20", "fr21", "fr22", "fr23", 
    "fr24", "fr25", "fr26", "fr27", "fr28", "fr29", "fr30", "fr31",
};

const char* string_vr[] = {
    "vr0",  "vr1",  "vr2",  "vr3",  "vr4",  "vr5",  "vr6",  "vr7",
    "vr8",  "vr9",  "vr10", "vr11", "vr12", "vr13", "vr14", "vr15", 
    "vr16", "vr17", "vr18", "vr19", "vr20", "vr21", "vr22", "vr23", 
    "vr24", "vr25", "vr26", "vr27", "vr28", "vr29", "vr30", "vr31",
};

const char* string_cr = "cr";
const char* string_fpscr = "fpscr";
const char* string_xer = "xer";

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
 * Memory access
 */
llvm::Value* Recompiler::readMemory(llvm::Value* addr, int bits)
{
    llvm::Value* baseAddr = builder.getInt64((u64)nucleus.memory.getBaseAddr());
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
    llvm::Value* baseAddr = builder.getInt64((u64)nucleus.memory.getBaseAddr());
    
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

}  // namespace ppu
}  // namespace cpu
