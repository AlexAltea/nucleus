/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_state.h"
#include "nucleus/emulator.h"

#include "llvm/IR/LLVMContext.h"

namespace cpu {
namespace ppu {

void State::declareGlobalState(llvm::Module* module)
{
    llvm::Type* ppuStateType = llvm::StructType::create(std::vector<llvm::Type*>{
        llvm::ArrayType::get( llvm::Type::getInt64Ty(llvm::getGlobalContext()),  32 ), // GPR's
        llvm::ArrayType::get( llvm::Type::getDoubleTy(llvm::getGlobalContext()), 32 ), // FPR's
    });

    module->getOrInsertGlobal("ppuState", llvm::PointerType::get(ppuStateType, 0));

    // Configure global PPU state
    llvm::GlobalVariable* ppuState = module->getNamedGlobal("ppuState");
    ppuState->setConstant(false);
    ppuState->setThreadLocal(true);
    ppuState->setLinkage(llvm::GlobalValue::ExternalLinkage);
}

llvm::Value* State::readGPR(llvm::IRBuilder<>& builder, int index)
{
    llvm::GlobalVariable* ppuState = nucleus.cell.module->getNamedGlobal("ppuState");
    // TODO
    return nullptr;
}

}  // namespace ppu
}  // namespace cpu
