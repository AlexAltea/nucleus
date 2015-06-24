/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "builder.h"

namespace cpu {
namespace hir {

Builder::Builder() : builder(llvm::getGlobalContext())
{
}

void Builder::SetInsertPoint(Block block)
{
    builder.SetInsertPoint(block.bb);
}

void Builder::SetInsertPoint(Block block, llvm::BasicBlock::iterator ip)
{
    builder.SetInsertPoint(block.bb, ip);
}


}  // namespace hir
}  // namespace cpu
