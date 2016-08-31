/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/frontend/x86/x86_state.h"

namespace cpu {
namespace native {
namespace x86 {

void setStateAndEnter64(cpu::frontend::x86::X86State state, void* target);

}  // namespace x86
}  // namespace native
}  // namespace cpu
