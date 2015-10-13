/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppc_assembler.h"

namespace cpu {
namespace backend {
namespace ppc {

void PPCAssembler::emit(U32 instruction) {
    auto* addr = static_cast<BE<U32>*>(curAddr);
    *addr++ = instruction;
}

// Emit instruction form
void PPCAssembler::emitFormXO(U32 instruction, Operand d, Operand a, Operand b, bool oe, bool rc) {
    const U32 rd = (d & 0x1F) << 21;
    const U32 ra = (a & 0x1F) << 16;
    const U32 rb = (b & 0x1F) << 11;
    emit(instruction | rd | ra | rb);
}

// PPC instructions
void PPCAssembler::add(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000214, rd, ra, rb, false, false); }

}  // namespace ppc
}  // namespace backend
}  // namespace cpu
