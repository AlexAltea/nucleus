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
};

struct Op {
};

/**
 * Generic Value operand
 */
template <int KeyValue, typename RegType, typename ConstType>
struct ValueOp : Op {
    static constexpr InstrKey::Value key = KeyValue;
    const hir::Value* value;

    bool isConstant;
    bool isRegister;
    RegType reg;
    void load(const hir::Instruction::Operand op) {
        value = op.value;
        // TODO
    }
};

template <typename RegType>
struct I8OpBase : ValueOp<TYPE_I8, RegType, S8> {
    const S8 constant() const {
        return value->constant.i8;
    }
};
template <typename RegType>
struct I16OpBase : ValueOp<TYPE_I16, RegType, S16> {
    const S16 constant() const {
        return value->constant.i16;
    }
};
template <typename RegType>
struct I32OpBase : ValueOp<TYPE_I32, RegType, S32> {
    const S32 constant() const {
        return value->constant.i32;
    }
};
template <typename RegType>
struct I64OpBase : ValueOp<TYPE_I64, RegType, S64> {
    const S64 constant() const {
        return value->constant.i64;
    }
};
template <typename RegType>
struct F32OpBase : ValueOp<TYPE_F32, RegType, F32> {
    const F32 constant() const {
        return value->constant.f32;
    }
};
template <typename RegType>
struct F64OpBase : ValueOp<TYPE_F64, RegType, F64> {
    const F64 constant() const {
        return value->constant.f64;
    }
};
template <typename RegType>
struct V128OpBase : ValueOp<TYPE_V128, RegType, V128> {
    const V128 constant() const {
        return value->constant.v128;
    }
};
template <typename RegType>
struct V256OpBase : ValueOp<TYPE_V256, RegType, V256> {
    const V256 constant() const {
        return value->constant.v256;
   } 
};
template <typename RegType>
struct PtrOpBase : ValueOp<TYPE_PTR, RegType, void*> {
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
    S1 src1;
    S2 src2;
    S3 src3;

    void load(const hir::Instruction* i) {
        instr = i;
        src1.load(i->src1);
        src2.load(i->src2);
        src3.load(i->src3);
    }
};

/**
 * Sequence
 * @tparam  S  Derived sequence (CRTP)
 * @tparam  I  Instruction type
 */
template <typename S, typename I>
struct SequenceBase {
    using InstrType = I;
};

}  // namespace backend
}  // namespace cpu
