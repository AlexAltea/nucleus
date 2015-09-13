/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/instruction.h"
#include "nucleus/cpu/hir/opcodes.h"
#include "nucleus/cpu/hir/type.h"

namespace cpu {
namespace backend {

union InstrKey {
    using Value = U32;

    struct {
        U32 opcode : 8;
        U32 dest : 5;
        U32 src1 : 5;
        U32 src2 : 5;
        U32 src3 : 5;
        U32 reserved : 4;
    };

    Value value;

    InstrKey(const hir::Instruction* instr) : value(0) {
        const auto& opInfo = hir::opcodeInfo[instr->opcode];
        opcode = instr->opcode;
        if (opInfo.getSignatureDst() == hir::OPCODE_SIG_TYPE_V) {
            dest = instr->dest->type;
        }
        if (opInfo.getSignatureSrc1() == hir::OPCODE_SIG_TYPE_V) {
            src1 = instr->src1.value->type;
        }
        if (opInfo.getSignatureSrc2() == hir::OPCODE_SIG_TYPE_V) {
            src2 = instr->src2.value->type;
        }
        if (opInfo.getSignatureSrc3() == hir::OPCODE_SIG_TYPE_V) {
            src3 = instr->src3.value->type;
        }
    }
};

struct Op {
};

struct VoidOp : Op {
    static constexpr InstrKey::Value key = hir::TYPE_VOID;
    void load(const hir::Value* v) {
    }
};


/**
 * Generic Value operand
 * @tparam  OpType     Derived operand type (CRTP)
 * @tparam  KeyValue   Operand key
 * @tparam  RegType    Type of emitter register
 * @tparam  ConstType  Type of constant value
 */
template <typename OpType, typename RegType, typename ConstType, int KeyValue>
struct ValueOp : Op {
    static constexpr InstrKey::Value key = KeyValue;
    const hir::Value* value;

    bool isConstant;
    RegType reg;
    operator const RegType&() const {
        return reg;
    }
    bool operator==(const OpType& other) const {
        // Constant, Constant
        if (isConstant && other.isConstant) {
            return reinterpret_cast<const OpType*>(this)->constant() == other.constant();
        }
        // Register, Register
        if (!isConstant && !other.isConstant) {
            return reg == other.reg;
        }
        return false;
    }
    void load(const hir::Value* v) {
        value = v;
        isConstant = value->isConstant();
        if (!isConstant) {
            reg = RegType(v->reg);
        }
    }
};

template <typename RegType>
struct I8OpBase : ValueOp<I8OpBase<RegType>, RegType, S8, hir::TYPE_I8> {
    const S8 constant() const {
        return value->constant.i8;
    }
};
template <typename RegType>
struct I16OpBase : ValueOp<I16OpBase<RegType>, RegType, S16, hir::TYPE_I16> {
    const S16 constant() const {
        return value->constant.i16;
    }
};
template <typename RegType>
struct I32OpBase : ValueOp<I32OpBase<RegType>, RegType, S32, hir::TYPE_I32> {
    const S32 constant() const {
        return value->constant.i32;
    }
};
template <typename RegType>
struct I64OpBase : ValueOp<I64OpBase<RegType>, RegType, S64, hir::TYPE_I64> {
    const S64 constant() const {
        return value->constant.i64;
    }
};
template <typename RegType>
struct F32OpBase : ValueOp<F32OpBase<RegType>, RegType, F32, hir::TYPE_F32> {
    const F32 constant() const {
        return value->constant.f32;
    }
};
template <typename RegType>
struct F64OpBase : ValueOp<F64OpBase<RegType>, RegType, F64, hir::TYPE_F64> {
    const F64 constant() const {
        return value->constant.f64;
    }
};
template <typename RegType>
struct V128OpBase : ValueOp<V128OpBase<RegType>, RegType, V128, hir::TYPE_V128> {
    const V128 constant() const {
        return value->constant.v128;
    }
};
template <typename RegType>
struct V256OpBase : ValueOp<V256OpBase<RegType>, RegType, V256, hir::TYPE_V256> {
    const V256 constant() const {
        return value->constant.v256;
   } 
};
template <typename RegType>
struct PtrOpBase : ValueOp<PtrOpBase<RegType>, RegType, void*, hir::TYPE_PTR> {
    const void* constant() const {
        return value->constant.i64; // TODO
    }
};

/**
 * Instruction type
 * @tparam  O   Opcode
 * @tparam  D   Type of destination operand
 * @tparam  S1  Type of source 1 operand
 * @tparam  S2  Type of source 2 operand
 * @tparam  S3  Type of source 3 operand
 */
template <hir::Opcode O, typename D = VoidOp, typename S1 = VoidOp, typename S2 = VoidOp, typename S3 = VoidOp>
struct I {
    static constexpr InstrKey::Value key = (O) | (D::key << 8) | (S1::key << 13) | (S2::key << 18) | (S3::key << 23);
    const hir::Instruction* instr;
    D dest;
    S1 src1;
    S2 src2;
    S3 src3;

    I(const hir::Instruction* i) {
        instr = i;
        dest.load(i->dest);
        src1.load(i->src1.value);
        src2.load(i->src2.value);
        src3.load(i->src3.value);
    }
};

/**
 * Sequence
 * @tparam  S  Derived sequence (CRTP)
 * @tparam  I  Instruction type
 */
template <typename S, typename I>
struct SequenceBase {
    static constexpr InstrKey::Value key = I::key;
    using InstrType = I;
};

}  // namespace backend
}  // namespace cpu
