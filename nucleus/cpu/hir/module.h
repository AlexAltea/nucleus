/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/function.h"

#include "llvm/IR/Module.h"

#include <string>

namespace cpu {
namespace hir {

class Function;

class Module
{
public:
    llvm::Module* module;

    // Constructor
    Module(llvm::Module* m = nullptr) : module(m) {}

    static Module Create(const std::string& name);

    void dump();
    Function getFunction(const std::string& name);

    template <typename T>
    llvm::Constant* getOrInsertGlobal(const std::string& name) {
        return module->getOrInsertGlobal(name, T::getType().type);
    }
};

}  // namespace hir
}  // namespace cpu
