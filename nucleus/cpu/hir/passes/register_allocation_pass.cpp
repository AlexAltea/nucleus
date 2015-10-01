/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "register_allocation_pass.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/hir/instruction.h"
#include "nucleus/assert.h"

namespace cpu {
namespace hir {
namespace passes {

RegisterAllocationPass::RegisterAllocationPass(const backend::TargetInfo& targetInfo) 
    : targetInfo(targetInfo) {
    for (const auto& regSet : targetInfo.regSets) {
        RegSetUsage regUsage;
        regUsage.regs.reset();
        regUsage.count = regSet.count;
        regUsage.types = regSet.types;
        regUsages.push_back(regUsage);
    }
}

void RegisterAllocationPass::allocArgumentReg(int index, Value* arg) {
    for (const auto& regSet : targetInfo.regSets) {
        if (regSet.types & backend::RegisterSet::TYPE_INT && arg->isTypeInteger()) {
            if (index < regSet.argIndex.size()) {
                arg->reg = regSet.argIndex[index];
                break;
            }
        }
        // TODO: Float, vector
    }
}

bool RegisterAllocationPass::tryAllocValueReg(Value* value) {
    for (auto& regUsage : regUsages) {
        if (regUsage.types & backend::RegisterSet::TYPE_INT && value->isTypeInteger()) {
            for (size_t i = 0; i < regUsage.count; i++) {
                if (!regUsage.regs[i]) {
                    regUsage.regs[i] = 1;
                    value->reg = i;
                    return true;
                }
            }
        }
    }
    return false;
}

bool RegisterAllocationPass::tryFreeValueReg(Value* value) {
    value->usage -= 1;
    if (value->usage == 0) {
        for (auto& regUsage : regUsages) {
            if (regUsage.types & backend::RegisterSet::TYPE_INT && value->isTypeInteger()) {
                regUsage.regs[value->reg] = 0;
                return true;
            }
        }
    }
    return false;
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
            arg->reg = targetInfo.regSets[0].argIndex[i];
            break;
        }
    }

    // CFG values
    for (auto& block : function->blocks) {
        for (auto& i : block->instructions) {
            // Handle call arguments
            if (i->opcode == OPCODE_ARG) {
                allocArgumentReg(i->src1.immediate, i->dest);
                continue;
            }
            // Handle call returns
            if (i->opcode == OPCODE_CALL || i->opcode == OPCODE_CALLCOND) {
                continue;
            }
            // Handle everything else
            auto opInfo = opcodeInfo[i->opcode];
            if (opInfo.getSignatureDst() == OPCODE_SIG_TYPE_V) {
                if (!tryAllocValueReg(i->dest)) {
                    assert_always("This pass does not support placing values in the stack yet");
                }
            }
            if (opInfo.getSignatureSrc1() == OPCODE_SIG_TYPE_V) {
                tryFreeValueReg(i->src1.value);
            }
            if (opInfo.getSignatureSrc2() == OPCODE_SIG_TYPE_V) {
                tryFreeValueReg(i->src2.value);
            }
            if (opInfo.getSignatureSrc3() == OPCODE_SIG_TYPE_V) {
                tryFreeValueReg(i->src3.value);
            }
        }
    }

    function->flags |= FUNCTION_IS_COMPILABLE;
    return true;
}

}  // namespace passes
}  // namespace hir
}  // namespace cpu
