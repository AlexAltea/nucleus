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
    if ((op9  == 0x040) || // brz
        (op9  == 0x042) || // brnz
        (op9  == 0x044) || // brhz
        (op9  == 0x046) || // brhnz
        (op9  == 0x060) || // bra
        (op9  == 0x062) || // brasl
        (op9  == 0x064) || // br
        (op9  == 0x066)) { // brsl
        return true;
    }
    if ((op11 == 0x1A8) || // bi
        (op11 == 0x1A9) || // bisl
        (op11 == 0x1AB) || // bisled
        (op11 == 0x128) || // biz
        (op11 == 0x129) || // binz
        (op11 == 0x12A) || // bihz
        (op11 == 0x12B)) { // bihnz
        return true;
    }
    return false;
}

bool Instruction::is_branch_conditional() const {
    if ((op9  == 0x040) || // brz
        (op9  == 0x042) || // brnz
        (op9  == 0x044) || // brhz
        (op9  == 0x046)) { // brhnz
        return true;
    }
    if ((op11 == 0x1AB) || // bisled
        (op11 == 0x128) || // biz
        (op11 == 0x129) || // binz
        (op11 == 0x12A) || // bihz
        (op11 == 0x12B)) { // bihnz
        return true;
    }
    return false;
}

bool Instruction::is_branch_unconditional() const {
    if ((op9  == 0x060) || // bra
        (op9  == 0x062) || // brasl
        (op9  == 0x064) || // br
        (op9  == 0x066)) { // brsl
        return true;
    }
    if ((op11 == 0x1A8) || // bi
        (op11 == 0x1A9)) { // bisl
        return true;
    }
    return false;
}

bool Instruction::is_call() const {
    if ((op9  == 0x062) || // brasl
        (op9  == 0x066)) { // brsl
        return true;
    }
    if ((op11 == 0x1A9) || // bisl
        (op11 == 0x1AB)) { // bisled
        return true;
    }
    return false;
}

U32 Instruction::get_target(U32 currentAddr) const {
    // Absolute branches
    if ((op9  == 0x060) || // bra
        (op9  == 0x062)) { // brasl
        return (currentAddr & 0xFF000000) + (i16 << 2);
    }
    // Relative branches
    if ((op9  == 0x040) || // brz
        (op9  == 0x042) || // brnz
        (op9  == 0x044) || // brhz
        (op9  == 0x046) || // brhnz
        (op9  == 0x064) || // br
        (op9  == 0x066)) { // brsl
        return currentAddr + (i16 << 2);
    }
    return 0;
}


}  // namespace spu
}  // namespace frontend
}  // namespace cpu
