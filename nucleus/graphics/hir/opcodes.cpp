/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opcodes.h"

namespace gfx {
namespace hir {

OpcodeInfo opcodeInfo[__OPCODE_COUNT + 1] = {
#define OPCODE(id, name, signature)  { name, 0, signature },
#include "opcodes.inl"
#undef OPCODE
};

}  // namespace hir
}  // namespace gfx
