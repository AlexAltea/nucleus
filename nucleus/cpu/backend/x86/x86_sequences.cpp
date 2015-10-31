/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "x86_sequences.h"
#include "nucleus/assert.h"
#include "nucleus/cpu/hir/function.h"
#include "nucleus/cpu/backend/x86/x86_compiler.h"
#include "nucleus/cpu/backend/x86/x86_constants.h"
#include "nucleus/cpu/backend/x86/x86_emitter.h"
#include "nucleus/logger/logger.h"

#include <unordered_map>

namespace cpu {
namespace backend {
namespace x86 {

using namespace cpu::hir;

using I8Op = I8OpBase<Xbyak::Reg8>;
using I16Op = I16OpBase<Xbyak::Reg16>;
using I32Op = I32OpBase<Xbyak::Reg32>;
using I64Op = I64OpBase<Xbyak::Reg64>;
using F32Op = F32OpBase<Xbyak::Xmm>;
using F64Op = F64OpBase<Xbyak::Xmm>;
using V128Op = V128OpBase<Xbyak::Xmm>;
using V256Op = V256OpBase<Xbyak::Ymm>;
using PtrOp = PtrOpBase<Xbyak::Reg64>;

// Temporary register
template <typename T>
const T getTempReg(X86Emitter& e);
template <> const Xbyak::Reg8  getTempReg<Xbyak::Reg8>(X86Emitter& e)  { return e.al; }
template <> const Xbyak::Reg16 getTempReg<Xbyak::Reg16>(X86Emitter& e) { return e.ax; }
template <> const Xbyak::Reg32 getTempReg<Xbyak::Reg32>(X86Emitter& e) { return e.eax; }
template <> const Xbyak::Reg64 getTempReg<Xbyak::Reg64>(X86Emitter& e) { return e.rax; }

// Sequences
template <typename S, typename I>
struct Sequence : SequenceBase<S, I> {
    static void select(X86Emitter& emitter, const hir::Instruction* instr) {
        S::emit(emitter, I(instr));
    }

    template <typename FuncType>
    static void emitUnaryOp(X86Emitter& e, InstrType& i, FuncType func) {
        // Constant
        if (i.src1.isConstant) {
            e.mov(i.dest, i.src1.constant());
            func(e, i.dest);
        }
        // Register
        else {
            if (i.dest != i.src1) {
                e.mov(i.dest, i.src1);
            }
            func(e, i.dest);
        }
    }

    template <typename FuncType>
    static void emitCommutativeBinaryOp(X86Emitter& e, InstrType& i, FuncType func) {
        // Constant, Constant
        if (i.src1.isConstant && i.src2.isConstant) {
            if (i.src1.isConstant32b()) {
                e.mov(i.dest, i.src2.constant());
                func(e, i.dest, i.src1);
            } else if (i.src2.isConstant32b()) {
                e.mov(i.dest, i.src1.constant());
                func(e, i.dest, i.src2);
            } else {
                auto temp = getTempReg<decltype(i.src2.reg)>(e);
                e.mov(i.dest, i.src1.constant());
                e.mov(temp, i.src2.constant());
                func(e, i.dest, temp);
            }
        }
        // Constant, Register
        else if (i.src1.isConstant && !i.src2.isConstant) {
            if (i.dest == i.src2) {
                if (i.src1.isConstant32b()) {
                    func(e, i.dest, i.src1.constant());
                } else {
                    auto temp = getTempReg<decltype(i.src1.reg)>(e);
                    e.mov(temp, i.src1.constant());
                    func(e, i.dest, temp);
                }
            } else {
                e.mov(i.dest, i.src1.constant());
                func(e, i.dest, i.src2);
            }
        }
        // Register, Constant
        else if (!i.src1.isConstant && i.src2.isConstant) {
            if (i.dest == i.src1) {
                if (i.src2.isConstant32b()) {
                    func(e, i.dest, i.src2.constant());
                } else {
                    auto temp = getTempReg<decltype(i.src2.reg)>(e);
                    e.mov(temp, i.src2.constant());
                    func(e, i.dest, temp);
                }
            } else {
                e.mov(i.dest, i.src2.constant());
                func(e, i.dest, i.src1);
            }
        }
        // Register, Register
        else if (!i.src1.isConstant && !i.src2.isConstant) {
            if (i.dest == i.src1) {
                func(e, i.dest, i.src2);
            } else if (i.dest == i.src2) {
                func(e, i.dest, i.src1);
            } else {
                e.mov(i.dest, i.src1);
                func(e, i.dest, i.src2);
            }
        }
    }

    template <typename FuncRegType, typename FuncConstType>
    static void emitAssociativeBinaryOp(X86Emitter& e, InstrType& i, FuncRegType funcReg, FuncConstType funcConst) {
        // Constant, Constant
        if (i.src1.isConstant && i.src2.isConstant) {
            assert_always("Invalid associative binary instruction operands");
        }
        // Constant, Register
        else if (i.src1.isConstant && !i.src2.isConstant) {
            if (i.dest.reg == i.src2.reg) {
                auto temp = getTempReg<decltype(i.src2.reg)>(e);
                e.mov(temp, i.src2);
                e.mov(i.dest, i.src1.constant());
                funcReg(e, i.dest.reg, temp);
            } else {
                e.mov(i.dest, i.src1.constant());
                funcReg(e, i.dest.reg, i.src2.reg);
            }
        }
        // Register, Constant
        else if (!i.src1.isConstant && i.src2.isConstant) {
            if (i.dest == i.src1) {
                if (i.src2.isConstant32b()) {
                    funcConst(e, i.dest.reg, i.src2.constant());
                } else {
                    auto temp = getTempReg<decltype(i.src2.reg)>(e);
                    e.mov(temp, i.src2.constant());
                    funcReg(e, i.dest.reg, temp);
                }
            } else {
                e.mov(i.dest, i.src1);
                if (i.src2.isConstant32b()) {
                    funcConst(e, i.dest.reg, i.src2.constant());
                } else {
                    auto temp = getTempReg<decltype(i.src2.reg)>(e);
                    e.mov(temp, i.src2.constant());
                    funcReg(e, i.dest.reg, temp);
                }
            }
        }
        // Register, Register
        else if (!i.src1.isConstant && !i.src2.isConstant) {
            if (i.dest == i.src1) {
                funcReg(e, i.dest.reg, i.src2.reg);
            } else if (i.dest.reg == i.src2.reg) {
                auto temp = getTempReg<decltype(i.src2.reg)>(e);
                e.mov(temp, i.src2);
                e.mov(i.dest, i.src1);
                funcReg(e, i.dest.reg, temp);
            } else {
                e.mov(i.dest, i.src1);
                funcReg(e, i.dest.reg, i.src2.reg);
            }
        }
    }

    template <typename FuncType>
    static void emitAssociativeBinaryOp(X86Emitter& e, InstrType& i, FuncType func) {
        emitAssociativeBinaryOp(e, i, func, func);
    }

    template <typename FuncType>
    static void emitCompareOp(X86Emitter& e, InstrType& i, FuncType func) {
        // Constant, Constant
        if (i.src1.isConstant && i.src2.isConstant) {
            assert_always("Invalid comparison instruction operands");
        }
        // Constant, Register
        else if (i.src1.isConstant && !i.src2.isConstant) {
            if (i.src1.isConstant32b()) {
                func(e, i.dest, i.src2, i.src1.constant(), true);
            } else {
                auto temp = getTempReg<decltype(i.src1.reg)>(e);
                e.mov(temp, i.src1.constant());
                func(e, i.dest, i.src2, temp, true);
            }
        }
        // Register, Constant
        else if (!i.src1.isConstant && i.src2.isConstant) {
            if (i.src2.isConstant32b()) {
                func(e, i.dest, i.src1, i.src2.constant(), false);
            } else {
                auto temp = getTempReg<decltype(i.src2.reg)>(e);
                e.mov(temp, i.src2.constant());
                func(e, i.dest, i.src1, temp, false);
            }
        }
        // Register, Register
        else if (!i.src1.isConstant && !i.src2.isConstant) {
            func(e, i.dest, i.src1, i.src2, false);
        }
    }

    template <typename FuncType>
    static void emitBinaryXmmOp(X86Emitter& e, InstrType& i, FuncType func) {
        // Constant, Constant
        if (i.src1.isConstant && i.src2.isConstant) {
            assert_always("Invalid binary XMM instruction operands");
        }
        // Constant, Register
        else if (i.src1.isConstant && !i.src2.isConstant) {
            getXmmConstant(e, e.xmm0, i.src1.constant());
            func(e, i.dest, e.xmm0, i.src2);
        }
        // Register, Constant
        else if (!i.src1.isConstant && i.src2.isConstant) {
            getXmmConstant(e, e.xmm0, i.src2.constant());
            func(e, i.dest, i.src1, e.xmm0);
        }
        // Register, Register
        else {
            func(e, i.dest, i.src1, i.src2);
        }
    }
};

/**
 * Opcode: ADD
 */
struct ADD_I8 : Sequence<ADD_I8, I<OPCODE_ADD, I8Op, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.add(dest, src);
        });
    }
};
struct ADD_I16 : Sequence<ADD_I16, I<OPCODE_ADD, I16Op, I16Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.add(dest, src);
        });
    }
};
struct ADD_I32 : Sequence<ADD_I32, I<OPCODE_ADD, I32Op, I32Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.add(dest, src);
        });
    }
};
struct ADD_I64 : Sequence<ADD_I64, I<OPCODE_ADD, I64Op, I64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.add(dest, src);
        });
    }
};

/**
 * Opcode: SUB
 */
struct SUB_I8 : Sequence<SUB_I8, I<OPCODE_SUB, I8Op, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.sub(dest, src);
        });
    }
};
struct SUB_I16 : Sequence<SUB_I16, I<OPCODE_SUB, I16Op, I16Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.sub(dest, src);
        });
    }
};
struct SUB_I32 : Sequence<SUB_I32, I<OPCODE_SUB, I32Op, I32Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.sub(dest, src);
        });
    }
};
struct SUB_I64 : Sequence<SUB_I64, I<OPCODE_SUB, I64Op, I64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.sub(dest, src);
        });
    }
};

/**
 * Opcode: MUL
 */
struct MUL_I8 : Sequence<MUL_I8, I<OPCODE_MUL, I8Op, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            assert_always("Unimplemented");
        } else {
            assert_always("Unimplemented");
        }
    }
};
struct MUL_I16 : Sequence<MUL_I16, I<OPCODE_MUL, I16Op, I16Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            assert_always("Unimplemented");
        } else {
            assert_always("Unimplemented");
        }
    }
};
struct MUL_I32 : Sequence<MUL_I32, I<OPCODE_MUL, I32Op, I32Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            assert_always("Unimplemented");
        } else {
            assert_always("Unimplemented");
        }
    }
};
struct MUL_I64 : Sequence<MUL_I64, I<OPCODE_MUL, I64Op, I64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            assert_always("Unimplemented");
        } else {
            if (i.src2.isConstant) {
                if (i.src2.isConstant32b()) {
                    e.imul(i.dest, i.src1, i.src2.constant());
                } else {
                    e.mov(i.dest, i.src1);
                    e.mov(e.rax, i.src2.constant());
                    e.imul(i.dest, e.rax);
                }
            } else {
                e.mov(i.dest, i.src1);
                e.mov(e.rax, i.src2);
                e.imul(i.dest, e.rax);
            }
        }
    }
};

/**
 * Opcode: MULH
 */
#define EMIT_MULH(mulFunc, regA, regD) \
    if (i.src1.isConstant) { \
        e.mov(regA, i.src1.constant()); \
    } else { \
        e.mov(regA, i.src1); \
    } \
    if (i.src2.isConstant) { \
        e.mov(regD, i.src2.constant()); \
        mulFunc(regD); \
    } else { \
        mulFunc(i.src2); \
    } \
    e.mov(i.dest, regD);

struct MULH_I8 : Sequence<MULH_I8, I<OPCODE_MULH, I8Op, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            if (e.isExtensionAvailable(X86Extension::BMI2)) {
                assert_always("Unimplemented");
            } else {
                EMIT_MULH(e.mul, e.al, e.dl);
            }
        } else {
            EMIT_MULH(e.imul, e.al, e.dl);
        }
        // TODO: Restore rdx
    }
};
struct MULH_I16 : Sequence<MULH_I16, I<OPCODE_MULH, I16Op, I16Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            if (e.isExtensionAvailable(X86Extension::BMI2)) {
                assert_always("Unimplemented");
            } else {
                EMIT_MULH(e.mul, e.ax, e.dx);
            }
        } else {
            EMIT_MULH(e.imul, e.ax, e.dx);
        }
        // TODO: Restore rdx
    }
};
struct MULH_I32 : Sequence<MULH_I32, I<OPCODE_MULH, I32Op, I32Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            if (e.isExtensionAvailable(X86Extension::BMI2)) {
                assert_always("Unimplemented");
            } else {
                EMIT_MULH(e.mul, e.eax, e.edx);
            }
        } else {
            EMIT_MULH(e.imul, e.eax, e.edx);
        }
        // TODO: Restore rdx
    }
};
struct MULH_I64 : Sequence<MULH_I64, I<OPCODE_MULH, I64Op, I64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            if (e.isExtensionAvailable(X86Extension::BMI2)) {
                assert_always("Unimplemented");
            } else {
                EMIT_MULH(e.mul, e.rax, e.rdx);
            }
        } else {
            EMIT_MULH(e.imul, e.rax, e.rdx);
        }
        // TODO: Restore rdx
    }
};

#undef EMIT_MULH

/**
 * Opcode: DIV
 */
#define EMIT_DIV(convFunc, divFunc, regA, regD) \
    if (i.src1.isConstant) { \
        e.mov(regA, i.src1.constant()); \
    } else { \
        e.mov(regA, i.src1); \
    } \
    if (i.src2.isConstant) { \
        e.mov(regD, i.src2.constant()); \
        divFunc(regD); \
    } else { \
        convFunc;\
        divFunc(i.src2); \
    } \
    e.mov(i.dest, regA);

struct DIV_I8 : Sequence<DIV_I8, I<OPCODE_DIV, I8Op, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            EMIT_DIV(e.xor_(e.dl, e.dl), e.div, e.al, e.dl);
        } else {
            EMIT_DIV(e.xor_(e.dl, e.dl), e.idiv, e.al, e.dl);
        }
    }
};
struct DIV_I16 : Sequence<DIV_I16, I<OPCODE_DIV, I16Op, I16Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            EMIT_DIV(e.xor_(e.dx, e.dx), e.div, e.ax, e.dx);
        } else {
            EMIT_DIV(e.cwd(), e.idiv, e.ax, e.dx);
        }
    }
};
struct DIV_I32 : Sequence<DIV_I32, I<OPCODE_DIV, I32Op, I32Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            EMIT_DIV(e.xor_(e.edx, e.edx), e.div, e.eax, e.edx);
        } else {
            EMIT_DIV(e.cdq(), e.idiv, e.eax, e.edx);
        }
    }
};
struct DIV_I64 : Sequence<DIV_I64, I<OPCODE_DIV, I64Op, I64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            EMIT_DIV(e.xor_(e.rdx, e.rdx), e.div, e.rax, e.rdx);
        } else {
            EMIT_DIV(e.cqo(), e.idiv, e.rax, e.rdx);
        }
        // TODO: Restore rdx
    }
};

#undef EMIT_DIV

/**
 * Opcode: SQRT
 */
struct SQRT_F32 : Sequence<SQRT_F32, I<OPCODE_SQRT, F32Op, F32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.vsqrtss(i.dest, i.src1);
    }
};
struct SQRT_F64 : Sequence<SQRT_F64, I<OPCODE_SQRT, F64Op, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.vsqrtsd(i.dest, i.src1);
    }
};

/**
 * Opcode: ABS
 */
struct ABS_F32 : Sequence<ABS_F32, I<OPCODE_ABS, F32Op, F32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        getXmmConstantCompI32(e, e.xmm0, 0x7FFFFFFFUL);
        e.vpand(i.dest, i.src1, e.xmm0);
    }
};
struct ABS_F64 : Sequence<ABS_F64, I<OPCODE_ABS, F64Op, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        getXmmConstantCompI64(e, e.xmm0, 0x7FFFFFFF'FFFFFFFFULL);
        e.vpand(i.dest, i.src1, e.xmm0);
    }
};

/**
 * Opcode: NEG
 */
struct NEG_I8 : Sequence<NEG_I8, I<OPCODE_NEG, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitUnaryOp(e, i, [](X86Emitter& e, auto dest){
            e.neg(dest);
        });
    }
};
struct NEG_I16 : Sequence<NEG_I16, I<OPCODE_NEG, I16Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitUnaryOp(e, i, [](X86Emitter& e, auto dest){
            e.neg(dest);
        });
    }
};
struct NEG_I32 : Sequence<NEG_I32, I<OPCODE_NEG, I32Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitUnaryOp(e, i, [](X86Emitter& e, auto dest){
            e.neg(dest);
        });
    }
};
struct NEG_I64 : Sequence<NEG_I64, I<OPCODE_NEG, I64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitUnaryOp(e, i, [](X86Emitter& e, auto dest){
            e.neg(dest);
        });
    }
};

/**
 * Opcode: NOT
 */
struct NOT_I8 : Sequence<NOT_I8, I<OPCODE_NOT, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitUnaryOp(e, i, [](X86Emitter& e, auto dest) {
            e.not_(dest);
        });
    }
};
struct NOT_I16 : Sequence<NOT_I16, I<OPCODE_NOT, I16Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitUnaryOp(e, i, [](X86Emitter& e, auto dest) {
            e.not_(dest);
        });
    }
};
struct NOT_I32 : Sequence<NOT_I32, I<OPCODE_NOT, I32Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitUnaryOp(e, i, [](X86Emitter& e, auto dest) {
            e.not_(dest);
        });
    }
};
struct NOT_I64 : Sequence<NOT_I64, I<OPCODE_NOT, I64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitUnaryOp(e, i, [](X86Emitter& e, auto dest) {
            e.not_(dest);
        });
    }
};
struct NOT_V128 : Sequence<NOT_V128, I<OPCODE_NOT, V128Op, V128Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        assert_false(i.src1.isConstant);
        e.vpcmpeqw(e.xmm0, e.xmm0);
        e.vpxor(i.dest, i.src1, e.xmm0);
    }
};

/**
 * Opcode: AND
 */
struct AND_I8 : Sequence<AND_I8, I<OPCODE_AND, I8Op, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.and_(dest, src);
        });
    }
};
struct AND_I16 : Sequence<AND_I16, I<OPCODE_AND, I16Op, I16Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.and_(dest, src);
        });
    }
};
struct AND_I32 : Sequence<AND_I32, I<OPCODE_AND, I32Op, I32Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.and_(dest, src);
        });
    }
};
struct AND_I64 : Sequence<AND_I64, I<OPCODE_AND, I64Op, I64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.and_(dest, src);
        });
    }
};
struct AND_V128 : Sequence<AND_V128, I<OPCODE_AND, V128Op, V128Op, V128Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitBinaryXmmOp(e, i, [](X86Emitter& e, auto dest, auto src1, auto src2) {
            e.vpand(dest, src1, src2);
        });
    }
};

/**
 * Opcode: OR
 */
struct OR_I8 : Sequence<OR_I8, I<OPCODE_OR, I8Op, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.or_(dest, src);
        });
    }
};
struct OR_I16 : Sequence<OR_I16, I<OPCODE_OR, I16Op, I16Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.or_(dest, src);
        });
    }
};
struct OR_I32 : Sequence<OR_I32, I<OPCODE_OR, I32Op, I32Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.or_(dest, src);
        });
    }
};
struct OR_I64 : Sequence<OR_I64, I<OPCODE_OR, I64Op, I64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.or_(dest, src);
        });
    }
};
struct OR_V128 : Sequence<OR_V128, I<OPCODE_OR, V128Op, V128Op, V128Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitBinaryXmmOp(e, i, [](X86Emitter& e, auto dest, auto src1, auto src2) {
            e.vpor(dest, src1, src2);
        });
    }
};

/**
 * Opcode: XOR
 */
struct XOR_I8 : Sequence<XOR_I8, I<OPCODE_XOR, I8Op, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.xor_(dest, src);
        });
    }
};
struct XOR_I16 : Sequence<XOR_I16, I<OPCODE_XOR, I16Op, I16Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.xor_(dest, src);
        });
    }
};
struct XOR_I32 : Sequence<XOR_I32, I<OPCODE_XOR, I32Op, I32Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.xor_(dest, src);
        });
    }
};
struct XOR_I64 : Sequence<XOR_I64, I<OPCODE_XOR, I64Op, I64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitCommutativeBinaryOp(e, i, [](X86Emitter& e, auto dest, auto src) {
            e.xor_(dest, src);
        });
    }
};
struct XOR_V128 : Sequence<XOR_V128, I<OPCODE_XOR, V128Op, V128Op, V128Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitBinaryXmmOp(e, i, [](X86Emitter& e, auto dest, auto src1, auto src2) {
            e.vpxor(dest, src1, src2);
        });
    }
};

/**
 * Opcode: SHL
 */
struct SHL_I8 : Sequence<SHL_I8, I<OPCODE_SHL, I8Op, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i,
            [](X86Emitter& e, auto dest, auto srcReg) {
                if (e.isExtensionAvailable(X86Extension::BMI2)) {
                    e.shlx(dest.cvt32(), dest.cvt32(), srcReg.cvt32());
                } else {
                    e.mov(e.cl, srcReg);
                    e.shl(dest, e.cl);
                    // TODO: Restore cl
                }
            },
            [](X86Emitter& e, auto dest, auto srcConst) {
                e.shl(dest, srcConst);
            }
        );
    }
};
struct SHL_I16 : Sequence<SHL_I16, I<OPCODE_SHL, I16Op, I16Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i,
            [](X86Emitter& e, auto dest, auto srcReg) {
                if (e.isExtensionAvailable(X86Extension::BMI2)) {
                    e.shlx(dest.cvt32(), dest.cvt32(), srcReg.cvt32());
                } else {
                    e.mov(e.cl, srcReg);
                    e.shl(dest, e.cl);
                    // TODO: Restore cl
                }
            },
            [](X86Emitter& e, auto dest, auto srcConst) {
                e.shl(dest, srcConst);
            }
        );
    }
};
struct SHL_I32 : Sequence<SHL_I32, I<OPCODE_SHL, I32Op, I32Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i,
            [](X86Emitter& e, auto dest, auto srcReg) {
                if (e.isExtensionAvailable(X86Extension::BMI2)) {
                    e.shlx(dest.cvt32(), dest.cvt32(), srcReg.cvt32());
                } else {
                    e.mov(e.cl, srcReg);
                    e.shl(dest, e.cl);
                    // TODO: Restore cl
                }
            },
            [](X86Emitter& e, auto dest, auto srcConst) {
                e.shl(dest, srcConst);
            }
        );
    }
};
struct SHL_I64 : Sequence<SHL_I64, I<OPCODE_SHL, I64Op, I64Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i,
            [](X86Emitter& e, auto dest, auto srcReg) {
                if (e.isExtensionAvailable(X86Extension::BMI2)) {
                    e.shlx(dest.cvt64(), dest.cvt64(), srcReg.cvt64());
                } else {
                    e.mov(e.cl, srcReg);
                    e.shl(dest, e.cl);
                    // TODO: Restore cl
                }
            },
            [](X86Emitter& e, auto dest, auto srcConst) {
                e.shl(dest, srcConst);
            }
        );
    }
};

/**
 * Opcode: SHR
 */
struct SHR_I8 : Sequence<SHR_I8, I<OPCODE_SHR, I8Op, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i,
            [](X86Emitter& e, auto dest, auto srcReg) {
                // TODO: Use BMI2's shrx to avoid the mov
                e.mov(e.cl, srcReg);
                e.shr(dest, e.cl);
                // TODO: Restore cl
            },
            [](X86Emitter& e, auto dest, auto srcConst) {
                e.shr(dest, srcConst);
            }
        );
    }
};
struct SHR_I16 : Sequence<SHR_I16, I<OPCODE_SHR, I16Op, I16Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i,
            [](X86Emitter& e, auto dest, auto srcReg) {
                // TODO: Use BMI2's shrx to avoid the mov
                e.mov(e.cl, srcReg);
                e.shr(dest, e.cl);
                // TODO: Restore cl
            },
            [](X86Emitter& e, auto dest, auto srcConst) {
                e.shr(dest, srcConst);
            }
        );
    }
};
struct SHR_I32 : Sequence<SHR_I32, I<OPCODE_SHR, I32Op, I32Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i,
            [](X86Emitter& e, auto dest, auto srcReg) {
                // TODO: Use BMI2's shrx to avoid the mov
                e.mov(e.cl, srcReg);
                e.shr(dest, e.cl);
                // TODO: Restore cl
            },
            [](X86Emitter& e, auto dest, auto srcConst) {
                e.shr(dest, srcConst);
            }
        );
    }
};
struct SHR_I64 : Sequence<SHR_I64, I<OPCODE_SHR, I64Op, I64Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i,
            [](X86Emitter& e, auto dest, auto srcReg) {
                // TODO: Use BMI2's shrx to avoid the mov
                e.mov(e.cl, srcReg);
                e.shr(dest, e.cl);
                // TODO: Restore cl
            },
            [](X86Emitter& e, auto dest, auto srcConst) {
                e.shr(dest, srcConst);
            }
        );
    }
};

/**
 * Opcode: SHRA
 */
struct SHRA_I8 : Sequence<SHRA_I8, I<OPCODE_SHRA, I8Op, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i,
            [](X86Emitter& e, auto dest, auto srcReg) {
                // TODO: Use BMI2's sarx to avoid the mov
                e.mov(e.cl, srcReg);
                e.sar(dest, e.cl);
                // TODO: Restore cl
            },
            [](X86Emitter& e, auto dest, auto srcConst) {
                e.sar(dest, srcConst);
            }
        );
    }
};
struct SHRA_I16 : Sequence<SHRA_I16, I<OPCODE_SHRA, I16Op, I16Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i,
            [](X86Emitter& e, auto dest, auto srcReg) {
                // TODO: Use BMI2's sarx to avoid the mov
                e.mov(e.cl, srcReg);
                e.sar(dest, e.cl);
                // TODO: Restore cl
            },
            [](X86Emitter& e, auto dest, auto srcConst) {
                e.sar(dest, srcConst);
            }
        );
    }
};
struct SHRA_I32 : Sequence<SHRA_I32, I<OPCODE_SHRA, I32Op, I32Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i,
            [](X86Emitter& e, auto dest, auto srcReg) {
                // TODO: Use BMI2's sarx to avoid the mov
                e.mov(e.cl, srcReg);
                e.sar(dest, e.cl);
                // TODO: Restore cl
            },
            [](X86Emitter& e, auto dest, auto srcConst) {
                e.sar(dest, srcConst);
            }
        );
    }
};
struct SHRA_I64 : Sequence<SHRA_I64, I<OPCODE_SHRA, I64Op, I64Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitAssociativeBinaryOp(e, i,
            [](X86Emitter& e, auto dest, auto srcReg) {
                // TODO: Use BMI2's sarx to avoid the mov
                e.mov(e.cl, srcReg);
                e.sar(dest, e.cl);
                // TODO: Restore cl
            },
            [](X86Emitter& e, auto dest, auto srcConst) {
                e.sar(dest, srcConst);
            }
        );
    }
};

/**
 * Opcode: ZEXT
 */
struct ZEXT_I16_I8 : Sequence<ZEXT_I16_I8, I<OPCODE_ZEXT, I16Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movzx(i.dest, i.src1);
    }
};
struct ZEXT_I32_I8 : Sequence<ZEXT_I32_I8, I<OPCODE_ZEXT, I32Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movzx(i.dest, i.src1);
    }
};
struct ZEXT_I64_I8 : Sequence<ZEXT_I64_I8, I<OPCODE_ZEXT, I64Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movzx(i.dest, i.src1);
    }
};
struct ZEXT_I32_I16 : Sequence<ZEXT_I32_I16, I<OPCODE_ZEXT, I32Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movzx(i.dest, i.src1);
    }
};
struct ZEXT_I64_I16 : Sequence<ZEXT_I64_I16, I<OPCODE_ZEXT, I64Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movzx(i.dest, i.src1);
    }
};
struct ZEXT_I64_I32 : Sequence<ZEXT_I64_I32, I<OPCODE_ZEXT, I64Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.mov(i.dest.reg.cvt32(), i.src1);
    }
};

/**
 * Opcode: SEXT
 */
struct SEXT_I16_I8 : Sequence<SEXT_I16_I8, I<OPCODE_SEXT, I16Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movsx(i.dest, i.src1);
    }
};
struct SEXT_I32_I8 : Sequence<SEXT_I32_I8, I<OPCODE_SEXT, I32Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movsx(i.dest, i.src1);
    }
};
struct SEXT_I64_I8 : Sequence<SEXT_I64_I8, I<OPCODE_SEXT, I64Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movsx(i.dest, i.src1);
    }
};
struct SEXT_I32_I16 : Sequence<SEXT_I32_I16, I<OPCODE_SEXT, I32Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movsx(i.dest, i.src1);
    }
};
struct SEXT_I64_I16 : Sequence<SEXT_I64_I16, I<OPCODE_SEXT, I64Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movsx(i.dest, i.src1);
    }
};
struct SEXT_I64_I32 : Sequence<SEXT_I64_I32, I<OPCODE_SEXT, I64Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movsxd(i.dest, i.src1);
    }
};

/**
 * Opcode: TRUNC
 */
struct TRUNC_I8_I16 : Sequence<TRUNC_I8_I16, I<OPCODE_TRUNC, I8Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movzx(i.dest.reg.cvt32(), i.src1.reg.cvt8());
    }
};
struct TRUNC_I8_I32 : Sequence<TRUNC_I8_I32, I<OPCODE_TRUNC, I8Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movzx(i.dest.reg.cvt32(), i.src1.reg.cvt8());
    }
};
struct TRUNC_I8_I64 : Sequence<TRUNC_I8_I64, I<OPCODE_TRUNC, I8Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movzx(i.dest.reg.cvt32(), i.src1.reg.cvt8());
    }
};
struct TRUNC_I16_I32 : Sequence<TRUNC_I16_I32, I<OPCODE_TRUNC, I16Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movzx(i.dest.reg.cvt32(), i.src1.reg.cvt16());
    }
};
struct TRUNC_I16_I64 : Sequence<TRUNC_I16_I64, I<OPCODE_TRUNC, I16Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movzx(i.dest.reg.cvt32(), i.src1.reg.cvt16());
    }
};
struct TRUNC_I32_I64 : Sequence<TRUNC_I32_I64, I<OPCODE_TRUNC, I32Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.mov(i.dest, i.src1.reg.cvt32());
    }
};

/**
 * Opcode: CAST
 */
struct CAST_I32_F32 : Sequence<CAST_I32_F32, I<OPCODE_CAST, I32Op, F32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.vmovd(i.dest, i.src1);
    }
};
struct CAST_F32_I32 : Sequence<CAST_F32_I32, I<OPCODE_CAST, F32Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.vmovd(i.dest, i.src1);
    }
};
struct CAST_I64_F64 : Sequence<CAST_I64_F64, I<OPCODE_CAST, I64Op, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.vmovq(i.dest, i.src1);
    }
};
struct CAST_F64_I64 : Sequence<CAST_F64_I64, I<OPCODE_CAST, F64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.vmovq(i.dest, i.src1);
    }
};

/**
 * Opcode: CONVERT
 */
struct CONVERT_I32_F32 : Sequence<CONVERT_I32_F32, I<OPCODE_CONVERT, I32Op, F32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.vcvtss2si(i.dest, i.src1);
    }
};
struct CONVERT_I32_F64 : Sequence<CONVERT_I32_F64, I<OPCODE_CONVERT, I32Op, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.vcvttsd2si(i.dest, i.src1);
    }
};
struct CONVERT_I64_F64 : Sequence<CONVERT_I64_F64, I<OPCODE_CONVERT, I64Op, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.vcvttsd2si(i.dest, i.src1);
    }
};
struct CONVERT_F32_F64 : Sequence<CONVERT_F32_F64, I<OPCODE_CONVERT, F32Op, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.vcvtsd2ss(i.dest, i.src1);
    }
};
struct CONVERT_F64_I64 : Sequence<CONVERT_F64_I64, I<OPCODE_CONVERT, F64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.vcvtsi2sd(i.dest, i.src1);
    }
};
struct CONVERT_F64_F32 : Sequence<CONVERT_F64_F32, I<OPCODE_CONVERT, F64Op, F32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.vcvtss2sd(i.dest, i.src1);
    }
};

/**
 * Opcode: CTLZ
 */
#define EMIT_CTLZ(bitSize) \
    Xbyak::Label jz, jend; \
    e.bsr(i.dest, i.src1); \
    e.jz(jz); \
    e.mov(e.rax, bitSize-1); \
    e.sub(e.rax, i.dest); \
    e.mov(i.dest, e.rax); \
    e.jmp(jend); \
    e.L(jz); \
    e.mov(i.dest, bitSize); \
    e.L(jend);

struct CTLZ_I8 : Sequence<CTLZ_I8, I<OPCODE_CTLZ, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (e.isExtensionAvailable(X86Extension::LZCNT)) {
            e.xor_(e.ax, e.ax);
            e.mov(e.al, i.src1);
            e.lzcnt(i.dest.reg.cvt16(), e.ax);
        } else {
            e.inLocalLabel();
            EMIT_CTLZ(8);
            e.outLocalLabel();
        }
    }
};
struct CTLZ_I16 : Sequence<CTLZ_I16, I<OPCODE_CTLZ, I8Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (e.isExtensionAvailable(X86Extension::LZCNT)) {
            e.lzcnt(i.dest.reg.cvt16(), i.src1);
        } else {
            e.inLocalLabel();
            EMIT_CTLZ(16);
            e.outLocalLabel();
        }
    }
};
struct CTLZ_I32 : Sequence<CTLZ_I32, I<OPCODE_CTLZ, I8Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (e.isExtensionAvailable(X86Extension::LZCNT)) {
            e.lzcnt(i.dest.reg.cvt32(), i.src1);
        } else {
            e.inLocalLabel();
            EMIT_CTLZ(32);
            e.outLocalLabel();
        }
    }
};
struct CTLZ_I64 : Sequence<CTLZ_I64, I<OPCODE_CTLZ, I8Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (e.isExtensionAvailable(X86Extension::LZCNT)) {
            e.lzcnt(i.dest.reg.cvt64(), i.src1);
        } else {
            e.inLocalLabel();
            EMIT_CTLZ(64);
            e.outLocalLabel();
        }
    }
};

/**
 * Opcode: LOAD
 */
struct LOAD_I8 : Sequence<LOAD_I8, I<OPCODE_LOAD, I8Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = i.src1.reg;
        e.mov(i.dest, e.byte[addr]);
    }
};
struct LOAD_I16 : Sequence<LOAD_I16, I<OPCODE_LOAD, I16Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = i.src1.reg;
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                e.movbe(i.dest, e.word[addr]);
            } else {
                e.mov(i.dest, e.word[addr]);
                e.ror(i.dest, 8);
            }
        } else {
            e.mov(i.dest, e.word[addr]);
        }
    }
};
struct LOAD_I32 : Sequence<LOAD_I32, I<OPCODE_LOAD, I32Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = i.src1.reg;
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                e.movbe(i.dest, e.dword[addr]);
            } else {
                e.mov(i.dest, e.dword[addr]);
                e.bswap(i.dest);
            }
        } else {
            e.mov(i.dest, e.dword[addr]);
        }
    }
};
struct LOAD_I64 : Sequence<LOAD_I64, I<OPCODE_LOAD, I64Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = i.src1.reg;
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                e.movbe(i.dest, e.qword[addr]);
            } else {
                e.mov(i.dest, e.qword[addr]);
                e.bswap(i.dest);
            }
        } else {
            e.mov(i.dest, e.qword[addr]);
        }
    }
};
struct LOAD_F32 : Sequence<LOAD_F32, I<OPCODE_LOAD, F32Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = i.src1.reg;
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                assert_always("Unimplemented");
            } else {
                e.mov(e.eax, e.dword[addr]);
                e.bswap(e.eax);
                e.vmovd(i.dest, e.eax);
            }
        } else {
            e.vmovss(i.dest, e.dword[addr]);
        }
    }
};
struct LOAD_F64 : Sequence<LOAD_F64, I<OPCODE_LOAD, F64Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = i.src1.reg;
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                assert_always("Unimplemented");
            } else {
                e.mov(e.rax, e.dword[addr]);
                e.bswap(e.rax);
                e.vmovq(i.dest, e.rax);
            }
        } else {
            e.vmovsd(i.dest, e.qword[addr]);
        }
    }
};
struct LOAD_V128 : Sequence<LOAD_V128, I<OPCODE_LOAD, V128Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = i.src1.reg;
        e.vmovups(i.dest, e.ptr[addr]);
        if (i.instr->flags & ENDIAN_BIG) {
            V128 byteSwapMask;
            byteSwapMask.u64[0] = 0x08090A0B0C0D0E0FULL;
            byteSwapMask.u64[1] = 0x0001020304050607ULL;
            getXmmConstant(e, e.xmm0, byteSwapMask);
            e.vpshufb(i.dest, i.dest, e.xmm0);
        }
    }
};

/**
 * Opcode: STORE
 */
struct STORE_I8 : Sequence<STORE_I8, I<OPCODE_STORE, VoidOp, PtrOp, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = i.src1.reg;
        if (i.src2.isConstant) {
            e.mov(e.byte[addr], i.src2.constant());
        } else {
            e.mov(e.byte[addr], i.src2);
        }
    }
};
struct STORE_I16 : Sequence<STORE_I16, I<OPCODE_STORE, VoidOp, PtrOp, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = i.src1.reg;
        if (i.instr->flags & ENDIAN_BIG) {
            assert_false(i.src2.isConstant);
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                e.movbe(e.word[addr], i.src2);
            } else {
                e.mov(e.eax, i.src2);
                e.xchg(e.ah, e.al);
                e.mov(e.word[addr], e.eax);
            }
        } else {
            if (i.src2.isConstant) {
                e.mov(e.word[addr], i.src2.constant());
            } else {
                e.mov(e.word[addr], i.src2);
            }
        }
    }
};
struct STORE_I32 : Sequence<STORE_I32, I<OPCODE_STORE, VoidOp, PtrOp, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = i.src1.reg;
        if (i.instr->flags & ENDIAN_BIG) {
            assert_false(i.src2.isConstant);
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                e.movbe(e.dword[addr], i.src2);
            } else {
                e.mov(e.eax, i.src2);
                e.bswap(e.eax);
                e.mov(e.dword[addr], e.eax);
            }
        } else {
            if (i.src2.isConstant) {
                e.mov(e.dword[addr], i.src2.constant());
            } else {
                e.mov(e.dword[addr], i.src2);
            }
        }
    }
};
struct STORE_I64 : Sequence<STORE_I64, I<OPCODE_STORE, VoidOp, PtrOp, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = i.src1.reg;
        if (i.instr->flags & ENDIAN_BIG) {
            assert_false(i.src2.isConstant);
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                e.movbe(e.qword[addr], i.src2);
            } else {
                e.mov(e.rax, i.src2);
                e.bswap(e.rax);
                e.mov(e.qword[addr], e.rax);
            }
        } else {
            if (i.src2.isConstant) {
                e.mov(e.qword[addr], i.src2.constant());
            } else {
                e.mov(e.qword[addr], i.src2);
            }
        }
    }
};
struct STORE_F32 : Sequence<STORE_F32, I<OPCODE_STORE, VoidOp, PtrOp, F32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = i.src1.reg;
        if (i.instr->flags & ENDIAN_BIG) {
            assert_false(i.src2.isConstant);
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                assert_always("Unimplemented");
            } else {
                e.vmovd(e.eax, i.src2);
                e.bswap(e.eax);
                e.mov(e.dword[addr], e.eax);
            }
        } else {
            if (i.src2.isConstant) {
                e.mov(e.dword[addr], i.src2.value->constant.i32);
            } else {
                e.vmovss(e.dword[addr], i.src2);
            }
        }
    }
};
struct STORE_F64 : Sequence<STORE_F64, I<OPCODE_STORE, VoidOp, PtrOp, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = i.src1.reg;
        if (i.instr->flags & ENDIAN_BIG) {
            assert_false(i.src2.isConstant);
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                assert_always("Unimplemented");
            } else {
                e.vmovq(e.rax, i.src2);
                e.bswap(e.rax);
                e.mov(e.qword[addr], e.rax);
            }
        } else {
            if (i.src2.isConstant) {
                e.mov(e.qword[addr], i.src2.value->constant.i64);
            } else {
                e.vmovsd(e.qword[addr], i.src2);
            }
        }
    }
};
struct STORE_V128 : Sequence<STORE_V128, I<OPCODE_STORE, VoidOp, PtrOp, V128Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = i.src1.reg;
        if (i.instr->flags & ENDIAN_BIG) {
            assert_false(i.src2.isConstant);
            V128 byteSwapMask;
            byteSwapMask.u64[0] = 0x08090A0B0C0D0E0FULL;
            byteSwapMask.u64[1] = 0x0001020304050607ULL;
            getXmmConstant(e, e.xmm0, byteSwapMask);
            e.vpshufb(e.xmm0, i.src2, e.xmm0);
            e.vmovaps(e.ptr[addr], e.xmm0);
        } else {
            if (i.src2.isConstant) {
                e.mov(e.rax, i.src2.constant().u64[0]);
                e.mov(e.qword[addr + 0], e.rax);
                e.mov(e.rax, i.src2.constant().u64[1]);
                e.mov(e.qword[addr + 8], e.rax);
            } else {
                e.vmovaps(e.ptr[addr], i.src2);
            }
        }
    }
};

/**
 * Opcode: CTXLOAD
 */
struct CTXLOAD_I8 : Sequence<CTXLOAD_I8, I<OPCODE_CTXLOAD, I8Op, ImmediateOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = e.rbx + i.src1.immediate;
        e.mov(i.dest, e.byte[addr]);
    }
};
struct CTXLOAD_I16 : Sequence<CTXLOAD_I16, I<OPCODE_CTXLOAD, I16Op, ImmediateOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = e.rbx + i.src1.immediate;
        e.mov(i.dest, e.word[addr]);
    }
};
struct CTXLOAD_I32 : Sequence<CTXLOAD_I32, I<OPCODE_CTXLOAD, I32Op, ImmediateOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = e.rbx + i.src1.immediate;
        e.mov(i.dest, e.dword[addr]);
    }
};
struct CTXLOAD_I64 : Sequence<CTXLOAD_I64, I<OPCODE_CTXLOAD, I64Op, ImmediateOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = e.rbx + i.src1.immediate;
        e.mov(i.dest, e.qword[addr]);
    }
};
struct CTXLOAD_F32 : Sequence<CTXLOAD_F32, I<OPCODE_CTXLOAD, F32Op, ImmediateOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = e.rbx + i.src1.immediate;
        e.vmovss(i.dest, e.dword[addr]);
    }
};
struct CTXLOAD_F64 : Sequence<CTXLOAD_F64, I<OPCODE_CTXLOAD, F64Op, ImmediateOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = e.rbx + i.src1.immediate;
        e.vmovsd(i.dest, e.qword[addr]);
    }
};
struct CTXLOAD_V128 : Sequence<CTXLOAD_V128, I<OPCODE_CTXLOAD, V128Op, ImmediateOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = e.rbx + i.src1.immediate;
        e.vmovaps(i.dest, e.ptr[addr]);
    }
};

/**
 * Opcode: CTXSTORE
 */
struct CTXSTORE_I8 : Sequence<CTXSTORE_I8, I<OPCODE_CTXSTORE, VoidOp, ImmediateOp, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = e.rbx + i.src1.immediate;
        if (i.src2.isConstant) {
            e.mov(e.byte[addr], i.src2.constant());
        } else {
            e.mov(e.byte[addr], i.src2);
        }
    }
};
struct CTXSTORE_I16 : Sequence<CTXSTORE_I16, I<OPCODE_CTXSTORE, VoidOp, ImmediateOp, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = e.rbx + i.src1.immediate;
        if (i.src2.isConstant) {
            e.mov(e.word[addr], i.src2.constant());
        } else {
            e.mov(e.word[addr], i.src2);
        }
    }
};
struct CTXSTORE_I32 : Sequence<CTXSTORE_I32, I<OPCODE_CTXSTORE, VoidOp, ImmediateOp, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = e.rbx + i.src1.immediate;
        if (i.src2.isConstant) {
            e.mov(e.dword[addr], i.src2.constant());
        } else {
            e.mov(e.dword[addr], i.src2);
        }
    }
};
struct CTXSTORE_I64 : Sequence<CTXSTORE_I64, I<OPCODE_CTXSTORE, VoidOp, ImmediateOp, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = e.rbx + i.src1.immediate;
        if (i.src2.isConstant) {
            e.mov(e.qword[addr], i.src2.constant());
        } else {
            e.mov(e.qword[addr], i.src2);
        }
    }
};
struct CTXSTORE_F32 : Sequence<CTXSTORE_F32, I<OPCODE_CTXSTORE, VoidOp, ImmediateOp, F32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = e.rbx + i.src1.immediate;
        if (i.src2.isConstant) {
            e.mov(e.dword[addr], i.src2.value->constant.i32);
        } else {
            e.vmovss(e.dword[addr], i.src2);
        }
    }
};
struct CTXSTORE_F64 : Sequence<CTXSTORE_F64, I<OPCODE_CTXSTORE, VoidOp, ImmediateOp, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = e.rbx + i.src1.immediate;
        if (i.src2.isConstant) {
            e.mov(e.qword[addr], i.src2.value->constant.i64);
        } else {
            e.vmovsd(e.qword[addr], i.src2);
        }
    }
};
struct CTXSTORE_V128 : Sequence<CTXSTORE_V128, I<OPCODE_CTXSTORE, VoidOp, ImmediateOp, V128Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        auto addr = e.rbx + i.src1.immediate;
        if (i.src2.isConstant) {
            e.mov(e.qword[addr + 0], i.src2.constant().u64[0]);
            e.mov(e.qword[addr + 8], i.src2.constant().u64[1]);
        } else {
            e.vmovaps(e.ptr[addr], i.src2);
        }
    }
};

/**
 * Opcode: MEMFENCE
 */
struct MEMFENCE : Sequence<MEMFENCE, I<OPCODE_MEMFENCE>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.mfence();
    }
};

/**
 * Opcode: SELECT
 */
struct SELECT_I8 : Sequence<SELECT_I8, I<OPCODE_SELECT, I8Op, I8Op, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.test(i.src1, i.src1);

        if (i.src2.isConstant) {
            auto temp = getTempReg<decltype(i.src2.reg)>(e);
            e.mov(temp, i.src2.constant());
            e.cmovnz(i.dest.reg.cvt32(), temp.cvt32());
        } else {
            e.cmovnz(i.dest.reg.cvt32(), i.src2.reg.cvt32());
        }

        if (i.src3.isConstant) {
            auto temp = getTempReg<decltype(i.src3.reg)>(e);
            e.mov(temp, i.src3.constant());
            e.cmovz(i.dest.reg.cvt32(), temp.cvt32());
        } else {
            e.cmovz(i.dest.reg.cvt32(), i.src3.reg.cvt32());
        }
    }
};
struct SELECT_I16 : Sequence<SELECT_I16, I<OPCODE_SELECT, I16Op, I8Op, I16Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.test(i.src1, i.src1);

        if (i.src2.isConstant) {
            auto temp = getTempReg<decltype(i.src2.reg)>(e);
            e.mov(temp, i.src2.constant());
            e.cmovnz(i.dest.reg.cvt32(), temp.cvt32());
        } else {
            e.cmovnz(i.dest.reg.cvt32(), i.src2.reg.cvt32());
        }

        if (i.src3.isConstant) {
            auto temp = getTempReg<decltype(i.src3.reg)>(e);
            e.mov(temp, i.src3.constant());
            e.cmovz(i.dest.reg.cvt32(), temp.cvt32());
        } else {
            e.cmovz(i.dest.reg.cvt32(), i.src3.reg.cvt32());
        }
    }
};
struct SELECT_I32 : Sequence<SELECT_I32, I<OPCODE_SELECT, I32Op, I8Op, I32Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.test(i.src1, i.src1);

        if (i.src2.isConstant) {
            auto temp = getTempReg<decltype(i.src2.reg)>(e);
            e.mov(temp, i.src2.constant());
            e.cmovnz(i.dest, temp);
        } else {
            e.cmovnz(i.dest, i.src2);
        }

        if (i.src3.isConstant) {
            auto temp = getTempReg<decltype(i.src3.reg)>(e);
            e.mov(temp, i.src3.constant());
            e.cmovz(i.dest, temp);
        } else {
            e.cmovz(i.dest, i.src3);
        }
    }
};
struct SELECT_I64 : Sequence<SELECT_I64, I<OPCODE_SELECT, I64Op, I8Op, I64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.test(i.src1, i.src1);

        if (i.src2.isConstant) {
            auto temp = getTempReg<decltype(i.src2.reg)>(e);
            e.mov(temp, i.src2.constant());
            e.cmovnz(i.dest, temp);
        } else {
            e.cmovnz(i.dest, i.src2);
        }

        if (i.src3.isConstant) {
            auto temp = getTempReg<decltype(i.src3.reg)>(e);
            e.mov(temp, i.src3.constant());
            e.cmovz(i.dest, temp);
        } else {
            e.cmovz(i.dest, i.src3);
        }
    }
};
struct SELECT_F32 : Sequence<SELECT_F32, I<OPCODE_SELECT, F32Op, I8Op, F32Op, F32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movzx(e.eax, i.src1);
        e.vmovd(e.xmm1, e.eax);
        e.vxorps(e.xmm0, e.xmm0);
        e.vcmpneqss(e.xmm0, e.xmm1);
        e.vpand(e.xmm1, e.xmm0, i.src2);
        e.vpandn(i.dest, e.xmm0, i.src3);
        e.vpor(i.dest, e.xmm1);
    }
};
struct SELECT_F64 : Sequence<SELECT_F64, I<OPCODE_SELECT, F64Op, I8Op, F64Op, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.movzx(e.eax, i.src1);
        e.vmovd(e.xmm1, e.eax);
        e.vxorpd(e.xmm0, e.xmm0);
        e.vcmpneqsd(e.xmm0, e.xmm1);
        e.vpand(e.xmm1, e.xmm0, i.src2);
        e.vpandn(i.dest, e.xmm0, i.src3);
        e.vpor(i.dest, e.xmm1);
    }
};

/**
 * Opcode: CMP
 */
#define EMIT_COMMUTATIVE_INTEGER_COMPARE(set) \
    emitCompareOp(e, i, [](X86Emitter& e, auto dest, auto lhs, auto rhs, bool inverse) { \
        e.cmp(lhs, rhs); \
        e.##set##(dest); \
    });

#define EMIT_ASSOCIATIVE_INTEGER_COMPARE(set, setInv) \
    emitCompareOp(e, i, [](X86Emitter& e, auto dest, auto lhs, auto rhs, bool inverse) { \
        e.cmp(lhs, rhs); \
        if (!inverse) { \
            e.##set##(dest); \
        } else { \
            e.##setInv##(dest); \
        } \
    });

#define EMIT_FLOAT_COMPARE(cmp, set) \
    if (i.src1.isConstant) { \
        getXmmConstant(e, e.xmm0, i.src1.constant()); \
        e.##cmp##(e.xmm0, i.src2); \
    } else if (i.src2.isConstant) {  \
        getXmmConstant(e, e.xmm0, i.src2.constant()); \
        e.##cmp##(i.src1, e.xmm0); \
    } else { \
        e.##cmp##(i.src1, i.src2); \
    } \
    e.##set##(i.dest); 


struct CMP_I8 : Sequence<CMP_I8, I<OPCODE_CMP, I8Op, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        switch (i.instr->flags) {
        case COMPARE_EQ:  EMIT_COMMUTATIVE_INTEGER_COMPARE(sete);          break;
        case COMPARE_NE:  EMIT_COMMUTATIVE_INTEGER_COMPARE(setne);         break;
        case COMPARE_SLT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setl,  setg);   break;
        case COMPARE_SLE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setle, setge);  break;
        case COMPARE_SGE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setge, setle);  break;
        case COMPARE_SGT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setg,  setl);   break;
        case COMPARE_ULT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setb,  seta);   break;
        case COMPARE_ULE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setbe, setae);  break;
        case COMPARE_UGE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setae, setbe);  break;
        case COMPARE_UGT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(seta,  setb);   break;
        default:
            assert_always("Unimplemented case");
        }
    }
};
struct CMP_I16 : Sequence<CMP_I16, I<OPCODE_CMP, I8Op, I16Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        switch (i.instr->flags) {
        case COMPARE_EQ:  EMIT_COMMUTATIVE_INTEGER_COMPARE(sete);          break;
        case COMPARE_NE:  EMIT_COMMUTATIVE_INTEGER_COMPARE(setne);         break;
        case COMPARE_SLT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setl,  setg);   break;
        case COMPARE_SLE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setle, setge);  break;
        case COMPARE_SGE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setge, setle);  break;
        case COMPARE_SGT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setg,  setl);   break;
        case COMPARE_ULT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setb,  seta);   break;
        case COMPARE_ULE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setbe, setae);  break;
        case COMPARE_UGE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setae, setbe);  break;
        case COMPARE_UGT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(seta,  setb);   break;
        default:
            assert_always("Unimplemented case");
        }
    }
};

struct CMP_I32 : Sequence<CMP_I32, I<OPCODE_CMP, I8Op, I32Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        switch (i.instr->flags) {
        case COMPARE_EQ:  EMIT_COMMUTATIVE_INTEGER_COMPARE(sete);          break;
        case COMPARE_NE:  EMIT_COMMUTATIVE_INTEGER_COMPARE(setne);         break;
        case COMPARE_SLT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setl,  setg);   break;
        case COMPARE_SLE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setle, setge);  break;
        case COMPARE_SGE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setge, setle);  break;
        case COMPARE_SGT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setg,  setl);   break;
        case COMPARE_ULT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setb,  seta);   break;
        case COMPARE_ULE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setbe, setae);  break;
        case COMPARE_UGE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setae, setbe);  break;
        case COMPARE_UGT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(seta,  setb);   break;
        default:
            assert_always("Unimplemented case");
        }
    }
};

struct CMP_I64 : Sequence<CMP_I64, I<OPCODE_CMP, I8Op, I64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        switch (i.instr->flags) {
        case COMPARE_EQ:  EMIT_COMMUTATIVE_INTEGER_COMPARE(sete);          break;
        case COMPARE_NE:  EMIT_COMMUTATIVE_INTEGER_COMPARE(setne);         break;
        case COMPARE_SLT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setl,  setg);   break;
        case COMPARE_SLE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setle, setge);  break;
        case COMPARE_SGE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setge, setle);  break;
        case COMPARE_SGT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setg,  setl);   break;
        case COMPARE_ULT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setb,  seta);   break;
        case COMPARE_ULE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setbe, setae);  break;
        case COMPARE_UGE: EMIT_ASSOCIATIVE_INTEGER_COMPARE(setae, setbe);  break;
        case COMPARE_UGT: EMIT_ASSOCIATIVE_INTEGER_COMPARE(seta,  setb);   break;
        default:
            assert_always("Unimplemented case");
        }
    }
};

struct CMP_F32 : Sequence<CMP_F32, I<OPCODE_CMP, I8Op, F32Op, F32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        switch (i.instr->flags) {
        case COMPARE_EQ:  EMIT_FLOAT_COMPARE(vcomiss, sete);   break;
        case COMPARE_NE:  EMIT_FLOAT_COMPARE(vcomiss, setne);  break;
        case COMPARE_SLT: EMIT_FLOAT_COMPARE(vcomiss, setb);   break;
        case COMPARE_SLE: EMIT_FLOAT_COMPARE(vcomiss, setbe);  break;
        case COMPARE_SGE: EMIT_FLOAT_COMPARE(vcomiss, setae);  break;
        case COMPARE_SGT: EMIT_FLOAT_COMPARE(vcomiss, seta);   break;
        case COMPARE_ULT: EMIT_FLOAT_COMPARE(vcomiss, seta);   break;
        case COMPARE_ULE: EMIT_FLOAT_COMPARE(vcomiss, setbe);  break;
        case COMPARE_UGE: EMIT_FLOAT_COMPARE(vcomiss, setae);  break;
        case COMPARE_UGT: EMIT_FLOAT_COMPARE(vcomiss, seta);   break;
        default:
            assert_always("Unimplemented case");
        }
    }
};

struct CMP_F64 : Sequence<CMP_F64, I<OPCODE_CMP, I8Op, F64Op, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        switch (i.instr->flags) {
        case COMPARE_EQ:  EMIT_FLOAT_COMPARE(vcomisd, sete);   break;
        case COMPARE_NE:  EMIT_FLOAT_COMPARE(vcomisd, setne);  break;
        case COMPARE_SLT: EMIT_FLOAT_COMPARE(vcomisd, setb);   break;
        case COMPARE_SLE: EMIT_FLOAT_COMPARE(vcomisd, setbe);  break;
        case COMPARE_SGE: EMIT_FLOAT_COMPARE(vcomisd, setae);  break;
        case COMPARE_SGT: EMIT_FLOAT_COMPARE(vcomisd, seta);   break;
        case COMPARE_ULT: EMIT_FLOAT_COMPARE(vcomisd, seta);   break;
        case COMPARE_ULE: EMIT_FLOAT_COMPARE(vcomisd, setbe);  break;
        case COMPARE_UGE: EMIT_FLOAT_COMPARE(vcomisd, setae);  break;
        case COMPARE_UGT: EMIT_FLOAT_COMPARE(vcomisd, seta);   break;
        default:
            assert_always("Unimplemented case");
        }
    }
};

#undef EMIT_COMMUTATIVE_INTEGER_COMPARE
#undef EMIT_ASSOCIATIVE_INTEGER_COMPARE
#undef EMIT_FLOAT_COMPARE

/**
 * Opcode: BR
 */
struct BR : Sequence<BR, I<OPCODE_BR, VoidOp, BlockOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        const Xbyak::Label& label = e.labels[i.src1.block];
        e.jmp(label, e.T_NEAR);
    }
};

/**
 * Opcode: ARG
 */
struct ARG_I8 : Sequence<ARG_I8, I<OPCODE_ARG, I8Op, ImmediateOp, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.src2.isConstant) {
            e.mov(i.dest, i.src2.constant());
        } else {
            e.mov(i.dest, i.src2);
        }
    }
};
struct ARG_I16 : Sequence<ARG_I16, I<OPCODE_ARG, I16Op, ImmediateOp, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.src2.isConstant) {
            e.mov(i.dest, i.src2.constant());
        } else {
            e.mov(i.dest, i.src2);
        }
    }
};
struct ARG_I32 : Sequence<ARG_I32, I<OPCODE_ARG, I32Op, ImmediateOp, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.src2.isConstant) {
            e.mov(i.dest, i.src2.constant());
        } else {
            e.mov(i.dest, i.src2);
        }
    }
};
struct ARG_I64 : Sequence<ARG_I64, I<OPCODE_ARG, I64Op, ImmediateOp, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.src2.isConstant) {
            e.mov(i.dest, i.src2.constant());
        } else {
            e.mov(i.dest, i.src2);
        }
    }
};

/**
 * Opcode: CALL
 */
struct CALL_VOID : Sequence<CALL_VOID, I<OPCODE_CALL, VoidOp, FunctionOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        const Function* target = i.src1.function;
        if (i.instr->flags & CALL_EXTERN) {
            e.mov(e.rax, reinterpret_cast<size_t>(target->nativeAddress));
            e.call(e.rax);
        } else {
            if (e.settings().isJIT) {
                e.mov(e.rax, reinterpret_cast<size_t>(target));
                e.mov(e.rax, e.qword[e.rax + offsetof(hir::Function, nativeAddress)]);
                e.call(e.rax);
            } else {
                e.mov(e.rax, reinterpret_cast<size_t>(target->nativeAddress));
                e.call(e.rax);
            }
        }
    }
};
struct CALL_I8 : Sequence<CALL_I8, I<OPCODE_CALL, I8Op, FunctionOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        const Function* target = i.src1.function;
        if (i.instr->flags & CALL_EXTERN) {
            e.mov(e.rax, reinterpret_cast<size_t>(target->nativeAddress));
            e.call(e.rax);
        } else {
            if (e.settings().isJIT) {
                e.mov(e.rax, reinterpret_cast<size_t>(target));
                e.mov(e.rax, e.qword[e.rax + offsetof(hir::Function, nativeAddress)]);
                e.call(e.rax);
            } else {
                e.mov(e.rax, reinterpret_cast<size_t>(target->nativeAddress));
                e.call(e.rax);
            }
        }
        // Save return value
        e.mov(i.dest, e.al);
    }
};
struct CALL_I16 : Sequence<CALL_I16, I<OPCODE_CALL, I16Op, FunctionOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        const Function* target = i.src1.function;
        if (i.instr->flags & CALL_EXTERN) {
            e.mov(e.rax, reinterpret_cast<size_t>(target->nativeAddress));
            e.call(e.rax);
        } else {
            if (e.settings().isJIT) {
                e.mov(e.rax, reinterpret_cast<size_t>(target));
                e.mov(e.rax, e.qword[e.rax + offsetof(hir::Function, nativeAddress)]);
                e.call(e.rax);
            } else {
                e.mov(e.rax, reinterpret_cast<size_t>(target->nativeAddress));
                e.call(e.rax);
            }
        }
        // Save return value
        e.mov(i.dest, e.ax);
    }
};
struct CALL_I32 : Sequence<CALL_I32, I<OPCODE_CALL, I32Op, FunctionOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        const Function* target = i.src1.function;
        if (i.instr->flags & CALL_EXTERN) {
            e.mov(e.rax, reinterpret_cast<size_t>(target->nativeAddress));
            e.call(e.rax);
        } else {
            if (e.settings().isJIT) {
                e.mov(e.rax, reinterpret_cast<size_t>(target));
                e.mov(e.rax, e.qword[e.rax + offsetof(hir::Function, nativeAddress)]);
                e.call(e.rax);
            } else {
                e.mov(e.rax, reinterpret_cast<size_t>(target->nativeAddress));
                e.call(e.rax);
            }
        }
        // Save return value
        e.mov(i.dest, e.eax);
    }
};
struct CALL_I64 : Sequence<CALL_I64, I<OPCODE_CALL, I64Op, FunctionOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        const Function* target = i.src1.function;
        if (i.instr->flags & CALL_EXTERN) {
            e.mov(e.rax, reinterpret_cast<size_t>(target->nativeAddress));
            e.call(e.rax);
        } else {
            if (e.settings().isJIT) {
                e.mov(e.rax, reinterpret_cast<size_t>(target));
                e.mov(e.rax, e.qword[e.rax + offsetof(hir::Function, nativeAddress)]);
                e.call(e.rax);
            } else {
                e.mov(e.rax, reinterpret_cast<size_t>(target->nativeAddress));
                e.call(e.rax);
            }
        }
        // Save return value
        e.mov(i.dest, e.rax);
    }
};

/**
 * Opcode: BRCOND
 */
struct BRCOND_I8 : Sequence<BRCOND_I8, I<OPCODE_BRCOND, VoidOp, I8Op, BlockOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        const Xbyak::Label& label = e.labels[i.src2.block];
        e.test(i.src1, i.src1);
        e.jnz(label, e.T_NEAR);
    }
};
struct BRCOND_I16 : Sequence<BRCOND_I16, I<OPCODE_BRCOND, VoidOp, I16Op, BlockOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        const Xbyak::Label& label = e.labels[i.src2.block];
        e.test(i.src1, i.src1);
        e.jnz(label, e.T_NEAR);
    }
};
struct BRCOND_I32 : Sequence<BRCOND_I32, I<OPCODE_BRCOND, VoidOp, I32Op, BlockOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        const Xbyak::Label& label = e.labels[i.src2.block];
        e.test(i.src1, i.src1);
        e.jnz(label, e.T_NEAR);
    }
};
struct BRCOND_I64 : Sequence<BRCOND_I64, I<OPCODE_BRCOND, VoidOp, I64Op, BlockOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        const Xbyak::Label& label = e.labels[i.src2.block];
        e.test(i.src1, i.src1);
        e.jnz(label, e.T_NEAR);
    }
};

/**
 * Opcode: CALLCOND
 */

/**
 * Opcode: RET
 */
struct RET_VOID : Sequence<RET_VOID, I<OPCODE_RET, VoidOp, VoidOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.jmp(e.labelEpilog, e.T_NEAR);
    }
};
struct RET_I8 : Sequence<RET_I8, I<OPCODE_RET, VoidOp, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.mov(e.al, i.src1);
        e.jmp(e.labelEpilog, e.T_NEAR);
    }
};
struct RET_I16 : Sequence<RET_I16, I<OPCODE_RET, VoidOp, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.mov(e.ax, i.src1);
        e.jmp(e.labelEpilog, e.T_NEAR);
    }
};
struct RET_I32 : Sequence<RET_I32, I<OPCODE_RET, VoidOp, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.mov(e.eax, i.src1);
        e.jmp(e.labelEpilog, e.T_NEAR);
    }
};
struct RET_I64 : Sequence<RET_I64, I<OPCODE_RET, VoidOp, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.mov(e.rax, i.src1);
        e.jmp(e.labelEpilog, e.T_NEAR);
    }
};
struct RET_F32 : Sequence<RET_F32, I<OPCODE_RET, VoidOp, F32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.mov(e.xmm0, i.src1);
        e.jmp(e.labelEpilog, e.T_NEAR);
    }
};
struct RET_F64 : Sequence<RET_F64, I<OPCODE_RET, VoidOp, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        e.mov(e.xmm0, i.src1);
        e.jmp(e.labelEpilog, e.T_NEAR);
    }
};

/**
 * Opcode: FADD
 */
struct FADD_F32 : Sequence<FADD_F32, I<OPCODE_FADD, F32Op, F32Op, F32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitBinaryXmmOp(e, i, [](X86Emitter& e, auto dest, auto src1, auto src2) {
            e.vaddss(dest, src1, src2);
        });
    }
};
struct FADD_F64 : Sequence<FADD_F64, I<OPCODE_FADD, F64Op, F64Op, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitBinaryXmmOp(e, i, [](X86Emitter& e, auto dest, auto src1, auto src2) {
            e.vaddsd(dest, src1, src2);
        });
    }
};

/**
 * Opcode: FSUB
 */
struct FSUB_F32 : Sequence<FSUB_F32, I<OPCODE_FSUB, F32Op, F32Op, F32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitBinaryXmmOp(e, i, [](X86Emitter& e, auto dest, auto src1, auto src2) {
            e.vsubss(dest, src1, src2);
        });
    }
};
struct FSUB_F64 : Sequence<FSUB_F64, I<OPCODE_FSUB, F64Op, F64Op, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitBinaryXmmOp(e, i, [](X86Emitter& e, auto dest, auto src1, auto src2) {
            e.vsubsd(dest, src1, src2);
        });
    }
};

/**
 * Opcode: FMUL
 */
struct FMUL_F32 : Sequence<FMUL_F32, I<OPCODE_FMUL, F32Op, F32Op, F32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitBinaryXmmOp(e, i, [](X86Emitter& e, auto dest, auto src1, auto src2) {
            e.vmulss(dest, src1, src2);
        });
    }
};
struct FMUL_F64 : Sequence<FMUL_F64, I<OPCODE_FMUL, F64Op, F64Op, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitBinaryXmmOp(e, i, [](X86Emitter& e, auto dest, auto src1, auto src2) {
            e.vmulsd(dest, src1, src2);
        });
    }
};

/**
 * Opcode: FDIV
 */
struct FDIV_F32 : Sequence<FDIV_F32, I<OPCODE_FDIV, F32Op, F32Op, F32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitBinaryXmmOp(e, i, [](X86Emitter& e, auto dest, auto src1, auto src2) {
            e.vdivss(dest, src1, src2);
        });
    }
};
struct FDIV_F64 : Sequence<FDIV_F64, I<OPCODE_FDIV, F64Op, F64Op, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        emitBinaryXmmOp(e, i, [](X86Emitter& e, auto dest, auto src1, auto src2) {
            e.vdivsd(dest, src1, src2);
        });
    }
};

/**
 * Opcode: FNEG
 */
struct FNEG_F32 : Sequence<FNEG_F32, I<OPCODE_FNEG, F32Op, F32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        getXmmConstantCompI32(e, e.xmm0, 0x80000000UL);
        e.vxorps(i.dest, i.src1, e.xmm0);
    }
};
struct FNEG_F64 : Sequence<FNEG_F64, I<OPCODE_FNEG, F64Op, F64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        getXmmConstantCompI64(e, e.xmm0, 0x80000000'00000000ULL);
        e.vxorps(i.dest, i.src1, e.xmm0);
    }
};

/**
 * x86 Sequences
 */
std::unordered_map<InstrKey::Value, X86Sequences::SelectFunction> X86Sequences::sequences;

void X86Sequences::init() {
    // Initialize sequences if necessary
    if (sequences.empty()) {
        registerSequence<ADD_I8, ADD_I16, ADD_I32, ADD_I64>();
        registerSequence<SUB_I8, SUB_I16, SUB_I32, SUB_I64>();
        registerSequence<MUL_I8, MUL_I16, MUL_I32, MUL_I64>();
        registerSequence<MULH_I8, MULH_I16, MULH_I32, MULH_I64>();
        registerSequence<DIV_I8, DIV_I16, DIV_I32, DIV_I64>();
        registerSequence<SQRT_F32, SQRT_F64>();
        registerSequence<ABS_F32, ABS_F64>();
        registerSequence<NEG_I8, NEG_I16, NEG_I32, NEG_I64>();
        registerSequence<NOT_I8, NOT_I16, NOT_I32, NOT_I64, NOT_V128>();
        registerSequence<AND_I8, AND_I16, AND_I32, AND_I64, AND_V128>();
        registerSequence<OR_I8, OR_I16, OR_I32, OR_I64, OR_V128>();
        registerSequence<XOR_I8, XOR_I16, XOR_I32, XOR_I64, XOR_V128>();
        registerSequence<SHL_I8, SHL_I16, SHL_I32, SHL_I64>();
        registerSequence<SHR_I8, SHR_I16, SHR_I32, SHR_I64>();
        registerSequence<SHRA_I8, SHRA_I16, SHRA_I32, SHRA_I64>();
        registerSequence<ZEXT_I16_I8, ZEXT_I32_I8, ZEXT_I64_I8, ZEXT_I32_I16, ZEXT_I64_I16, ZEXT_I64_I32>();
        registerSequence<SEXT_I16_I8, SEXT_I32_I8, SEXT_I64_I8, SEXT_I32_I16, SEXT_I64_I16, SEXT_I64_I32>();
        registerSequence<TRUNC_I8_I16, TRUNC_I8_I32, TRUNC_I8_I64, TRUNC_I16_I32, TRUNC_I16_I64, TRUNC_I32_I64>();
        registerSequence<CAST_I32_F32, CAST_F32_I32, CAST_I64_F64, CAST_F64_I64>();
        registerSequence<CONVERT_I32_F32, CONVERT_I32_F64, CONVERT_I64_F64, CONVERT_F32_F64, CONVERT_F64_I64, CONVERT_F64_F32>();
        registerSequence<CTLZ_I8, CTLZ_I16, CTLZ_I32, CTLZ_I64>();
        registerSequence<LOAD_I8, LOAD_I16, LOAD_I32, LOAD_I64, LOAD_F32, LOAD_F64, LOAD_V128>();
        registerSequence<STORE_I8, STORE_I16, STORE_I32, STORE_I64, STORE_F32, STORE_F64, STORE_V128>();
        registerSequence<CTXLOAD_I8, CTXLOAD_I16, CTXLOAD_I32, CTXLOAD_I64, CTXLOAD_F32, CTXLOAD_F64, CTXLOAD_V128>();
        registerSequence<CTXSTORE_I8, CTXSTORE_I16, CTXSTORE_I32, CTXSTORE_I64, CTXSTORE_F32, CTXSTORE_F64, CTXSTORE_V128>();
        registerSequence<MEMFENCE>();
        registerSequence<SELECT_I8, SELECT_I16, SELECT_I32, SELECT_I64, SELECT_F32, SELECT_F64>();
        registerSequence<CMP_I8, CMP_I16, CMP_I32, CMP_I64, CMP_F32, CMP_F64>();
        registerSequence<ARG_I8, ARG_I16, ARG_I32, ARG_I64>();
        registerSequence<BR>();
        registerSequence<CALL_VOID, CALL_I8, CALL_I16, CALL_I32, CALL_I64>();
        registerSequence<BRCOND_I8, BRCOND_I16, BRCOND_I32, BRCOND_I64>();
        registerSequence<RET_VOID, RET_I8, RET_I16, RET_I32, RET_I64, RET_F32, RET_F64>();
        registerSequence<FADD_F32, FADD_F64>();
        registerSequence<FSUB_F32, FSUB_F64>();
        registerSequence<FMUL_F32, FMUL_F64>();
        registerSequence<FDIV_F32, FDIV_F64>();
        registerSequence<FNEG_F32, FNEG_F64>();
    }
}

bool X86Sequences::select(X86Emitter& emitter, const hir::Instruction* instr) {
    auto key = InstrKey(instr).value;
    auto it = sequences.find(key);
    if (it != sequences.end()) {
        it->second(emitter, instr);
        return true;
    }

    logger.error(LOG_CPU, "No sequence found");
    return false;
}

}  // namespace x86
}  // namespace backend
}  // namespace cpu
