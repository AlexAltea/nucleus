/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "register_allocation_pass.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/hir/instruction.h"

namespace cpu {
namespace hir {
namespace passes {

RegisterAllocationPass::RegisterAllocationPass(const backend::TargetInfo& targetInfo) 
    : targetInfo(targetInfo) {
}

void RegisterAllocationPass::allocateArgument(int index, Value* arg) {
    for (auto& regSet : targetInfo.regSet) {
        if (regSet.types & backend::RegisterSet::TYPE_INT && arg->isTypeInteger()) {
            if (index < regSet.argIndex.size()) {
                arg->reg = regSet.argIndex[index];
                break;
            }
        }
        // TODO: Float, vector
    }
}

bool RegisterAllocationPass::run(Function* function) {
    // Arguments
    for (int i = 0; i < function->args.size(); i++) {
        auto& arg = function->args[i];
        switch (arg->type) {
        case TYPE_I8:
        case TYPE_I16:
        case TYPE_I32:
        case TYPE_I64:
            arg->reg = targetInfo.regSet[0].argIndex[i];
            break;
        }
    }

    // CFG values
    for (auto& block : function->blocks) {
        for (auto& instruction : block->instructions) {
            if (instruction->opcode == OPCODE_ARG) {
                allocateArgument(instruction->src1.immediate, instruction->dest);
            }
        }
    }

    function->flags |= FUNCTION_IS_COMPILABLE;
    return true;
}

}  // namespace passes
}  // namespace hir
}  // namespace cpu
