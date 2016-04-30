/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/backend/settings.h"
#include "nucleus/cpu/backend/target.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/hir/function.h"
#include "nucleus/cpu/hir/module.h"
#include "nucleus/cpu/hir/pass.h"
#include "nucleus/cpu/hir/value.h"

#include <memory>
#include <vector>

namespace cpu {
namespace backend {

class Compiler {
    // Compiler passes
    std::vector<std::unique_ptr<hir::Pass>> passes;

protected:
    // Optimize HIR
    virtual bool optimize(hir::Function* function);

public:
    // Generic code generation settings
    Settings settings;

    // Generic target information
    TargetInfo targetInfo;

    // Constructor
    Compiler();
    Compiler(const Settings& settings);

    // Add optimization passes
    virtual void addPass(std::unique_ptr<hir::Pass> pass);

    // Compile HIR
    virtual bool compile(hir::Block* block) = 0;
    virtual bool compile(hir::Function* function) = 0;
    virtual bool compile(hir::Module* module) = 0;

    /**
     * Runs a compiled function
     * @param[in]  function   Function to be called
     * @param[in]  state      Pointer to the guest thread state
     * @param[in]  arguments  Arguments as an array of constant values
     */
    virtual bool call(hir::Function* function, void* state, const std::vector<hir::Value*>& args = {}) = 0;

    // Manage RWX memory
    void* allocRWXMemory(Size size);
    void freeRWXMemory(void* addr);
};

}  // namespace backend
}  // namespace cpu
