/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_state.h"
#include "nucleus/emulator.h"

#include "llvm/IR/LLVMContext.h"

namespace cpu {
namespace ppu {

llvm::StructType* State::type()
{
    llvm::StructType* ppuStateType = llvm::StructType::get(llvm::getGlobalContext(), std::vector<llvm::Type*>{
        llvm::ArrayType::get( llvm::Type::getInt64Ty(llvm::getGlobalContext()),  32 ), // GPR's
        llvm::ArrayType::get( llvm::Type::getDoubleTy(llvm::getGlobalContext()), 32 ), // FPR's
        llvm::ArrayType::get( llvm::Type::getInt64Ty(llvm::getGlobalContext()),  96 ), // TODO: Other registers
    });

    return ppuStateType;
}

void State::declareGlobalState(llvm::Module* module)
{
    module->getOrInsertGlobal("ppuState", State::type());

    // Configure global PPU state
    llvm::GlobalVariable* ppuState = module->getNamedGlobal("ppuState");
    ppuState->setConstant(false);
    ppuState->setThreadLocal(true);
    ppuState->setLinkage(llvm::GlobalValue::ExternalLinkage);
    ppuState->setInitializer(llvm::ConstantAggregateZero::get(State::type()));
}

llvm::Value* State::readGPR(llvm::IRBuilder<>& builder, int index)
{
    llvm::Module* module = builder.GetInsertBlock()->getParent()->getParent();
    llvm::GlobalVariable* ppuState = module->getNamedGlobal("ppuState");

    llvm::Value* location = nullptr;
    location = builder.CreateStructGEP(ppuState, 0);
    location = builder.CreateStructGEP(location, index);
    return builder.CreateLoad(location);
}

}  // namespace ppu
}  // namespace cpu
