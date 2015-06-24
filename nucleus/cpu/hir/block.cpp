/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "block.h"

#include "llvm/IR/LLVMContext.h"

namespace cpu {
namespace hir {

Block Block::Create(const std::string& name, Function function)
{
    return llvm::BasicBlock::Create(llvm::getGlobalContext(), name, function.function);
}

}  // namespace hir
}  // namespace cpu
