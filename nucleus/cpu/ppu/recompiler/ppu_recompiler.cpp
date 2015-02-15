/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"

const char* string_gpr[] = {
     "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6" , "r7",
     "r8",  "r9", "r10", "r11", "r12", "r13", "r14", "r15", 
    "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", 
    "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",
};

namespace cpu {
namespace ppu {

Recompiler::Recompiler() : builder(llvm::getGlobalContext())
{
}

void Recompiler::setInsertPoint(llvm::BasicBlock* block)
{
    builder.SetInsertPoint(block);
}

llvm::Value* Recompiler::getGPR(int reg)
{
    if (!gpr[reg]) {
        gpr[reg] = builder.CreateAlloca(builder.getInt64Ty(), 0, string_gpr[reg]);
    }
    return builder.CreateLoad(gpr[reg], string_gpr[reg]);
}

void Recompiler::setGPR(int reg, llvm::Value* value)
{
    if (!gpr[reg]) {
        gpr[reg] = builder.CreateAlloca(builder.getInt64Ty(), 0, string_gpr[reg]);
    }
    builder.CreateStore(value, gpr[reg]);
}

llvm::Value* Recompiler::getFPR(int reg)
{
    // TODO: ?
    return nullptr;
}

void Recompiler::setFPR(int reg, llvm::Value* value)
{
    // TODO: ?
    return;
}

llvm::Value* Recompiler::getVR_u8(int reg)
{
    // TODO: ?
    return nullptr;
}

llvm::Value* Recompiler::getVR_u16(int reg)
{
    // TODO: ?
    return nullptr;
}

llvm::Value* Recompiler::getVR_u32(int reg)
{
    // TODO: ?
    return nullptr;
}

llvm::Value* Recompiler::getVR_f32(int reg)
{
    // TODO: ?
    return nullptr;
}

void Recompiler::setVR(int reg, llvm::Value* value)
{
    // TODO: ?
    return;
}

}  // namespace ppu
}  // namespace cpu
