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

}  // namespace ppu
}  // namespace cpu
