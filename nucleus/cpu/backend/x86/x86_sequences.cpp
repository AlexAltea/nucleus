/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "x86_sequences.h"
#include "nucleus/cpu/backend/x86/x86_emitter.h"
#include "nucleus/assert.h"

#include <unordered_map>

namespace cpu {
namespace backend {
namespace x86 {

using namespace cpu::hir;

struct I8Op : ValueOp<TYPE_I8, Xbyak::Reg8, S8> {
};

struct I16Op : ValueOp<TYPE_I16, Xbyak::Reg16, S16> {
};

struct I32Op : ValueOp<TYPE_I32, Xbyak::Reg32, S32> {
};

struct I64Op : ValueOp<TYPE_I64, Xbyak::Reg64, S64> {
};

struct F32Op : ValueOp<TYPE_F32, Xbyak::Fpu, F32> {
};

struct F64Op : ValueOp<TYPE_F64, Xbyak::Fpu, F64> {
};

struct V128Op : ValueOp<TYPE_V128, Xbyak::Xmm, V128> {
};

struct V256Op : ValueOp<TYPE_V256, Xbyak::Ymm, V256> {
};

struct PtrOp : ValueOp<TYPE_PTR, Xbyak::Reg64, void*> {
};


template <typename S, typename I>
struct Sequence : ISequence<S, I> {
    template <typename FuncType>
    static void emitCommutativeBinaryOp(X86Emitter& e, InstrType& i, FuncType func) {
        // Constant, Constant
        if (i.src1.isConstant && i.src2.isConstant) {
            func(e, i.dest.reg, i.src2.constant());
        }
        // Constant, Register
        if (i.src1.isConstant && i.src2.isRegister) {
        }
        // Register, Constant
        if (i.src1.isRegister && i.src2.isConstant) {
        }
        // Register, Register
        if (i.src1.isRegister && i.src2.isRegister) {
        }
    }
    template <typename FuncType>
    static void emitAssociativeBinaryOp(X86Emitter& e, InstrType& i, FuncType func) {
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
            // TODO
        } else {
            // TODO
        }
    }
};
struct MUL_I16 : Sequence<MUL_I16, I<OPCODE_MUL, I16Op, I16Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            // TODO
        } else {
            // TODO
        }
    }
};
struct MUL_I32 : Sequence<MUL_I32, I<OPCODE_MUL, I32Op, I32Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            // TODO
        } else {
            // TODO
        }
    }
};
struct MUL_I64 : Sequence<MUL_I64, I<OPCODE_MUL, I64Op, I64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            // TODO
        } else {
            // TODO
        }
    }
};


/**
 * Opcode: DIV
 */
struct DIV_I8 : Sequence<DIV_I8, I<OPCODE_DIV, I8Op, I8Op, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            // TODO
        } else {
            // TODO
        }
    }
};
struct DIV_I16 : Sequence<DIV_I16, I<OPCODE_DIV, I16Op, I16Op, I16Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            // TODO
        } else {
            // TODO
        }
    }
};
struct DIV_I32 : Sequence<DIV_I32, I<OPCODE_DIV, I32Op, I32Op, I32Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            // TODO
        } else {
            // TODO
        }
    }
};
struct DIV_I64 : Sequence<DIV_I64, I<OPCODE_DIV, I64Op, I64Op, I64Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ARITHMETIC_UNSIGNED) {
            // TODO
        } else {
            // TODO
        }
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

/**
 * Opcode: LOAD
 */
struct LOAD_I8 : Sequence<LOAD_I8, I<OPCODE_LOAD, I8Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        // TODO
    }
};
struct LOAD_I16 : Sequence<LOAD_I16, I<OPCODE_LOAD, I16Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                // TODO
            } else {
                // TODO
            }
        } else {
            // TODO
        }
    }
};
struct LOAD_I32 : Sequence<LOAD_I32, I<OPCODE_LOAD, I32Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                // TODO
            } else {
                // TODO
            }
        } else {
            // TODO
        }
    }
};
struct LOAD_I64 : Sequence<LOAD_I64, I<OPCODE_LOAD, I64Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                // TODO
            } else {
                // TODO
            }
        } else {
            // TODO
        }
    }
};
struct LOAD_F32 : Sequence<LOAD_F32, I<OPCODE_LOAD, F32Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                // TODO
            } else {
                // TODO
            }
        } else {
            // TODO
        }
    }
};
struct LOAD_F64 : Sequence<LOAD_F64, I<OPCODE_LOAD, F64Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                // TODO
            } else {
                // TODO
            }
        } else {
            // TODO
        }
    }
};
struct LOAD_V128 : Sequence<LOAD_V128, I<OPCODE_LOAD, V128Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                // TODO
            } else {
                // TODO
            }
        } else {
            // TODO
        }
    }
};

/**
 * Opcode: STORE
 */
struct STORE_I8 : Sequence<STORE_I8, I<OPCODE_STORE, I8Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        // TODO
    }
};
struct STORE_I16 : Sequence<STORE_I16, I<OPCODE_STORE, I16Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                // TODO
            } else {
                // TODO
            }
        } else {
            // TODO
        }
    }
};
struct STORE_I32 : Sequence<STORE_I32, I<OPCODE_STORE, I32Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                // TODO
            } else {
                // TODO
            }
        } else {
            // TODO
        }
    }
};
struct STORE_I64 : Sequence<STORE_I64, I<OPCODE_STORE, I64Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                // TODO
            } else {
                // TODO
            }
        } else {
            // TODO
        }
    }
};
struct STORE_F32 : Sequence<STORE_F32, I<OPCODE_STORE, F32Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                // TODO
            } else {
                // TODO
            }
        } else {
            // TODO
        }
    }
};
struct STORE_F64 : Sequence<STORE_F64, I<OPCODE_STORE, F64Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                // TODO
            } else {
                // TODO
            }
        } else {
            // TODO
        }
    }
};
struct STORE_V128 : Sequence<STORE_V128, I<OPCODE_STORE, V128Op, PtrOp>> {
    static void emit(X86Emitter& e, InstrType& i) {
        if (i.instr->flags & ENDIAN_BIG) {
            if (e.isExtensionAvailable(X86Extension::MOVBE)) {
                // TODO
            } else {
                // TODO
            }
        } else {
            // TODO
        }
    }
};

X86Sequences::X86Sequences()
{
    // Initialize sequences if necessary
    if (sequences.empty()) {

    }
}

}  // namespace x86
}  // namespace backend
}  // namespace cpu
