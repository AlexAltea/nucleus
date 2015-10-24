/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_state.h"

namespace cpu {
namespace frontend {
namespace ppu {

U32 PPUState::getCR() {
    U32 value = 0;

    // Going from MSb to LSb
    int shift = 31;
    for (const auto& field : cr.field) {
        for (const auto& bit : field.bit) {
            value |= ((bit & 0x1) << shift--);
        }
    }
    return value;
}

void PPUState::setCR(U32 value) {
    // Going from MSb to LSb
    int shift = 31;
    for (auto& field : cr.field) {
        for (auto& bit : field.bit) {
            bit = ((value >> shift--) & 0x1);
        }
    }
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
