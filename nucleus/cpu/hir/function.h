/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/hir/module.h"

#include "llvm/IR/Function.h"

#include <string>

namespace cpu {
namespace hir {

class Block;
class Module;

class Function
{
public:
    llvm::Function* function;

    // Constructor
    Function(llvm::Function* f = nullptr) : function(f) {}

    static Function Create(
        llvm::FunctionType* type,
        llvm::GlobalValue::LinkageTypes linkage,
        const std::string& name,
        Module module);

    Block getEntryBlock();
    Module getParent();
    std::string getName() const;

    void setCallingConv(llvm::CallingConv::ID cc);
    void setName(const std::string& name);
};

}  // namespace hir
}  // namespace cpu
