/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "module.h"

#include "llvm/IR/LLVMContext.h"

namespace cpu {
namespace hir {

void Module::dump()
{
    return module->dump();
}

Function Module::getFunction(const std::string& name)
{
    return module->getFunction(name);
}

Module Module::Create(const std::string& name)
{
    return new llvm::Module(name, llvm::getGlobalContext());
}

}  // namespace hir
}  // namespace cpu
