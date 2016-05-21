/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace cpu {
namespace frontend {
namespace spu {

class alignas(16) SPUState {
public:
    V128 r[128];  // General-Purpose Registers
    V128 s[128];  // Special-Purpose Registers

    U32 pc;       // Program Counter
};

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
