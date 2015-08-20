/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/thread.h"
#include "nucleus/cpu/frontend/ppu/interpreter/ppu_interpreter.h"

namespace cpu {
namespace frontend {
namespace ppu {

// PowerPC rotation-masks
struct RotateMask {
    U64 rotateMask[64][64];

    RotateMask() {
        for (U32 mb = 0; mb < 64; mb++) {
            for (U32 me = 0; me < 64; me++) {
                const U64 mask = (~0ULL >> mb) ^ ((me >= 63) ? 0 : ~0ULL >> (me + 1));
                rotateMask[mb][me] = mb > me ? ~mask : mask;
            }
        }
    }

    U64* operator[](size_t mb) {
        return rotateMask[mb];
    }
};

extern RotateMask rotateMask;

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
