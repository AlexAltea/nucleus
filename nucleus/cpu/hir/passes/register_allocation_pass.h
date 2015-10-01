/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/backend/target.h"
#include "nucleus/cpu/hir/pass.h"

#include <bitset>

namespace cpu {
namespace hir {
namespace passes {

/**
 * Register Allocation Pass
 * ========================
 * This is a mandatory compiler pass that will assign, if possible, a hardware register
 * to the values of the target function. Should the pass fail to place a value in a register,
 * it will place it into the stack, wrapping any access to that variable with the
 * corresponding load/store instructions.
 *
 * Notes:
 * - This pass should be the last one to apply to a function.
 * - This pass uses Value::usage to determine when a value is no longer needed
 *   and the corresponding register can be. Value::usage will be modified.
 */
class RegisterAllocationPass : public Pass {
private:
    struct RegSetUsage {
        U32 types;
        U32 count;
        std::bitset<32> regs;
    };

    // Target information
    const backend::TargetInfo& targetInfo;

    // Register usage
    std::vector<RegSetUsage> regUsages;

    /**
     * Handle call arguments
     * @param[in]  index  Index of the argument in the function
     * @param[in]  arg    Argument whose register has to be determined
     */
    void allocArgumentReg(int index, Value* arg);

    /**
     * Try to allocate a register for a value
     * @param[in]  value  Value to allocate a register for
     * @return            True if an available register was found
     */
    bool tryAllocValueReg(Value* value);

    /**
     * Try to free a register from a value
     * @param[in]  value  Value to allocate a register for
     * @return            True if an available register was found
     */
    bool tryFreeValueReg(Value* value);

public:
    // Constructor
    RegisterAllocationPass(const backend::TargetInfo& targetInfo);

    // Get the name of this pass
    const char* name() override {
        return "Register Allocation";
    }

    // Apply this pass on a function
    bool run(Function* function) override;
};

}  // namespace passes
}  // namespace hir
}  // namespace cpu
