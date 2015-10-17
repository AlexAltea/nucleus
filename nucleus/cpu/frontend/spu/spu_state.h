/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace cpu {
namespace frontend {
namespace spu {

class SPUState {
public:
    V128 gpr[128];  // General-Purpose Registers
    V128 spr[128];  // Special-Purpose Registers
};

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
