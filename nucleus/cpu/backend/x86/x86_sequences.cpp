/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "x86_sequences.h"
#include "nucleus/assert.h"
#include "nucleus/logger/logger.h"
#include "nucleus/cpu/backend/x86/x86_emitter.h"

#include <unordered_map>

namespace cpu {
namespace backend {
namespace x86 {

using namespace cpu::hir;

using I8Op = I8OpBase<Xbyak::Reg8>;
using I16Op = I16OpBase<Xbyak::Reg16>;
using I32Op = I32OpBase<Xbyak::Reg32>;
using I64Op = I64OpBase<Xbyak::Reg64>;
using F32Op = F32OpBase<Xbyak::Fpu>;
using F64Op = F64OpBase<Xbyak::Fpu>;
using V128Op = V128OpBase<Xbyak::Xmm>;
using V256Op = V256OpBase<Xbyak::Ymm>;
using PtrOp = PtrOpBase<Xbyak::Reg64>;

// Sequences
template <typename S, typename I>
struct Sequence : SequenceBase<S, I> {
    static void select(X86Emitter& emitter, const hir::Instruction* instr) {
        S::emit(emitter, I(instr));
    }

    template <typename FuncType>
    static void emitCommutativeBinaryOp(X86Emitter& e, InstrType& i, FuncType func) {
        // Constant, Constant
        if (i.src1.isConstant && i.src2.isConstant) {
        }
        // Constant, Register
        if (i.src1.isConstant && !i.src2.isConstant) {
        }
        // Register, Constant
        if (!i.src1.isConstant && i.src2.isConstant) {
        }
        // Register, Register
        if (!i.src1.isConstant && !i.src2.isConstant) {
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
struct LOAD_I8 : Sequence<LOAD_I8, I<OPCODE_LOAD, PtrOp, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        // TODO
    }
};
struct LOAD_I16 : Sequence<LOAD_I16, I<OPCODE_LOAD, PtrOp, I16Op>> {
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
struct LOAD_I32 : Sequence<LOAD_I32, I<OPCODE_LOAD, PtrOp, I32Op>> {
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
struct LOAD_I64 : Sequence<LOAD_I64, I<OPCODE_LOAD, PtrOp, I64Op>> {
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
struct LOAD_F32 : Sequence<LOAD_F32, I<OPCODE_LOAD, PtrOp, F32Op>> {
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
struct LOAD_F64 : Sequence<LOAD_F64, I<OPCODE_LOAD, PtrOp, F64Op>> {
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
struct LOAD_V128 : Sequence<LOAD_V128, I<OPCODE_LOAD, PtrOp, V128Op>> {
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
struct STORE_I8 : Sequence<STORE_I8, I<OPCODE_STORE, VoidOp, PtrOp, I8Op>> {
    static void emit(X86Emitter& e, InstrType& i) {
        // TODO
    }
};
struct STORE_I16 : Sequence<STORE_I16, I<OPCODE_STORE, VoidOp, PtrOp, I16Op>> {
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
struct STORE_I32 : Sequence<STORE_I32, I<OPCODE_STORE, VoidOp, PtrOp, I32Op>> {
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
struct STORE_I64 : Sequence<STORE_I64, I<OPCODE_STORE, VoidOp, PtrOp, I64Op>> {
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
struct STORE_F32 : Sequence<STORE_F32, I<OPCODE_STORE, VoidOp, PtrOp, F32Op>> {
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
struct STORE_F64 : Sequence<STORE_F64, I<OPCODE_STORE, VoidOp, PtrOp, F64Op>> {
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
struct STORE_V128 : Sequence<STORE_V128, I<OPCODE_STORE, VoidOp, PtrOp, V128Op>> {
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
 * x86 Sequences
 */
std::unordered_map<InstrKey::Value, X86Sequences::SelectFunction> X86Sequences::sequences;

void X86Sequences::init() {
    // Initialize sequences if necessary
    if (sequences.empty()) {
        registerSequence<ADD_I8, ADD_I16, ADD_I32, ADD_I64>();
        registerSequence<SUB_I8, SUB_I16, SUB_I32, SUB_I64>();
        registerSequence<MUL_I8, MUL_I16, MUL_I32, MUL_I64>();
        registerSequence<DIV_I8, DIV_I16, DIV_I32, DIV_I64>();
    }
}

bool X86Sequences::select(X86Emitter* emitter, const hir::Instruction* instr) {
    auto key = InstrKey(instr).value;
    auto it = sequences.find(key);
    if (it != sequences.end()) {
        it->second(*emitter, instr);
        return true;
    }

    logger.error(LOG_CPU, "No sequence found");
    return false;
}

}  // namespace x86
}  // namespace backend
}  // namespace cpu
