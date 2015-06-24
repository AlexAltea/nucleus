/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "function.h"

namespace cpu {
namespace hir {

Block Function::getEntryBlock()
{
    return Block{ &(function->getEntryBlock()) };
}

Module Function::getParent()
{
    return function->getParent();
}

std::string Function::getName() const
{
    return function->getName();
}

void Function::setCallingConv(llvm::CallingConv::ID cc)
{
    function->setCallingConv(cc);
}

void Function::setName(const std::string& name)
{
    function->setName(name);
}

Function Function::Create(llvm::FunctionType* type, llvm::GlobalValue::LinkageTypes linkage, const std::string& name, Module module)
{
    return llvm::Function::Create(type, linkage, name, module.module);
}

}  // namespace hir
}  // namespace cpu
