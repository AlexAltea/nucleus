/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "spu_instruction.h"
#include "nucleus/cpu/frontend/spu/spu_tables.h"

namespace cpu {
namespace frontend {
namespace spu {

bool Instruction::is_valid() const {
    // If instruction is not defined in the SPU architecture
    if (get_entry(*this).type == ENTRY_INVALID) {
        return false;
    }
    return true;
}

bool Instruction::is_branch() const {
    return false; // TODO
}

bool Instruction::is_branch_conditional() const {
    return false; // TODO
}

bool Instruction::is_branch_unconditional() const {
    return false; // TODO
}

bool Instruction::is_call() const {
    return false; // TODO
}

U32 Instruction::get_target(U32 currentAddr) const {
    return 0; // TODO
}


}  // namespace spu
}  // namespace frontend
}  // namespace cpu
