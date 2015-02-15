/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"
#include "nucleus/emulator.h"

const char* string_gpr[] = {
    "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6",  "r7",
    "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15", 
    "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", 
    "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",
};

namespace cpu {
namespace ppu {

Recompiler::Recompiler(Segment* segment) : builder(llvm::getGlobalContext()), segment(segment)
{
}

void Recompiler::setInsertPoint(llvm::BasicBlock* block)
{
    builder.SetInsertPoint(block);
}

llvm::Value* Recompiler::getGPR(int index, int bits)
{
    if (!gpr[index]) {
        gpr[index] = builder.CreateAlloca(builder.getInt64Ty(), 0, string_gpr[index]);
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
        gpr[index] = builder.CreateAlloca(builder.getInt64Ty(), 0, string_gpr[index]);
    }
    builder.CreateStore(value, gpr[index]);
}

llvm::Value* Recompiler::getFPR(int index)
{
    // TODO: ?
    return nullptr;
}

void Recompiler::setFPR(int index, llvm::Value* value)
{
    // TODO: ?
    return;
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
