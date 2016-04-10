/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/backend/x86/x86_emitter.h"

namespace cpu {
namespace backend {
namespace x86 {

/**
 * x86 Constants
 * =============
 * The following functions can be used to load constant values in x86 registers
 * in a fast and memory efficient way.
 *
 * References:
 *  - Optimizing subroutines in assembly language: Chapter 13.4.
 *    Agner Fog. Technical University of Denmark.
 */

/**
 * General-Purpose Register constants:
 * Loads a constant integer value in the given register.
 * @param[in]  e         Emitter of x86 assembly
 * @param[in]  dest      Destination GPR register
 * @param[in]  constant  Integer constant
 */
void getReg8Constant(X86Emitter& e, Xbyak::Reg8 dest, const S08 constant);
void getReg16Constant(X86Emitter& e, Xbyak::Reg16 dest, const S16 constant);
void getReg32Constant(X86Emitter& e, Xbyak::Reg32 dest, const S32 constant);
void getReg64Constant(X86Emitter& e, Xbyak::Reg64 dest, const S64 constant);

/**
 * XMM constants:
 * Loads a constant value in a XMM register either by passing
 * a 128-bit vector or setting a unique value in all components.
 * @param[in]  e         Emitter of x86 assembly
 * @param[in]  dest      Destination XMM register
 * @param[in]  constant  Entire 128-bit vector or value for each component
 */
void getXmmConstant(X86Emitter& e, Xbyak::Xmm dest, const F32 constant);
void getXmmConstant(X86Emitter& e, Xbyak::Xmm dest, const F64 constant);
void getXmmConstant(X86Emitter& e, Xbyak::Xmm dest, const V128& constant);
void getXmmConstantCompI32(X86Emitter& e, Xbyak::Xmm dest, const S32 constant);
void getXmmConstantCompI64(X86Emitter& e, Xbyak::Xmm dest, const S64 constant);
void getXmmConstantCompF32(X86Emitter& e, Xbyak::Xmm dest, const F32 constant);
void getXmmConstantCompF64(X86Emitter& e, Xbyak::Xmm dest, const F64 constant);

}  // namespace x86
}  // namespace backend
}  // namespace cpu
