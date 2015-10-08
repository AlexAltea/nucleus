/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "instruction.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/hir/function.h"
#include "nucleus/assert.h"
#include "nucleus/format.h"

#include <string>

namespace cpu {
namespace hir {

std::string Instruction::Operand::dump(U8 sigType) const {
    std::string output;
    if (sigType == OPCODE_SIG_TYPE_V || (sigType == OPCODE_SIG_TYPE_M && value != nullptr)) {
        if (value->isConstant()) {
            switch (value->type) {
            case TYPE_I8:   output = format("0x%X", value->constant.i8);   break;
            case TYPE_I16:  output = format("0x%X", value->constant.i16);  break;
            case TYPE_I32:  output = format("0x%X", value->constant.i32);  break;
            case TYPE_I64:  output = format("0x%X", value->constant.i64);  break;
            case TYPE_F32:  output = format("0x%X", value->constant.f32);  break;
            case TYPE_F64:  output = format("0x%X", value->constant.f64);  break;
            default:
                assert_always("Unimplemented case");
            }
        } else {
            switch (value->type) {
            case TYPE_I8:   output = "v" + std::to_string(value->getId()) + ".i8";   break;
            case TYPE_I16:  output = "v" + std::to_string(value->getId()) + ".i16";  break;
            case TYPE_I32:  output = "v" + std::to_string(value->getId()) + ".i32";  break;
            case TYPE_I64:  output = "v" + std::to_string(value->getId()) + ".i64";  break;
            case TYPE_F32:  output = "v" + std::to_string(value->getId()) + ".f32";  break;
            case TYPE_F64:  output = "v" + std::to_string(value->getId()) + ".f64";  break;
            case TYPE_V128: output = "v" + std::to_string(value->getId()) + ".v128"; break;
            case TYPE_V256: output = "v" + std::to_string(value->getId()) + ".v256"; break;
            default:
                assert_always("Unimplemented case");
            }
        }
    }
    if (sigType == OPCODE_SIG_TYPE_I) {
        output = std::to_string(immediate);
    }
    if (sigType == OPCODE_SIG_TYPE_B) {
        output = "b" + std::to_string(block->getId());
    }
    if (sigType == OPCODE_SIG_TYPE_F) {
        output = "f" + std::to_string(function->getId());
    }
    return output;
}

std::string Instruction::dump() const {
    std::string output;
    const auto& opInfo = opcodeInfo[opcode];

    // Destination
    auto sigTypeDest = opInfo.getSignatureDest();
    if (opInfo.getSignatureDest() == OPCODE_SIG_TYPE_V || (opInfo.getSignatureDest() == OPCODE_SIG_TYPE_M && dest != nullptr)) {
        output += "    v" + std::to_string(dest->getId()) + " = ";
    } else {
        output += "    ";
    }

    // Opcode name
    output += opInfo.name;

    // Flags
    if (flags != 0) {
        output += " ";
        switch (opcode) {
        // Arithmetic flags
        case OPCODE_ADD:
        case OPCODE_SUB:
        case OPCODE_MUL:
        case OPCODE_DIV:
            if (flags == ARITHMETIC_UNSIGNED) { output += "unsigned"; }
            break;

        // Call flags
        case OPCODE_CALL:
        case OPCODE_CALLCOND:
            if (flags == CallFlags::CALL_EXTERN) { output += "ext"; }
            break;

        // Compare flags
        case OPCODE_CMP:
            switch (flags) {
            case COMPARE_EQ:  output += "eq";  break;
            case COMPARE_NE:  output += "ne";  break;
            case COMPARE_SLT: output += "slt"; break;
            case COMPARE_SLE: output += "sle"; break;
            case COMPARE_SGE: output += "sge"; break;
            case COMPARE_SGT: output += "sgt"; break;
            case COMPARE_ULT: output += "ult"; break;
            case COMPARE_ULE: output += "ule"; break;
            case COMPARE_UGE: output += "uge"; break;
            case COMPARE_UGT: output += "ugt"; break;
            }
            break;

        // Memory flags
        case OPCODE_LOAD:
        case OPCODE_STORE:
            if (flags == ENDIAN_BIG) { output += "be"; }
            if (flags == ENDIAN_LITTLE) { output += "le"; }
            break;

        // Vector flags
        case OPCODE_VADD:
        case OPCODE_VSUB:
        case OPCODE_VAVG:
        case OPCODE_VCMP:
            if (flags == COMPONENT_I8) { output += "i8"; }
            if (flags == COMPONENT_I16) { output += "i16"; }
            if (flags == COMPONENT_I32) { output += "i32"; }
            if (flags == COMPONENT_I64) { output += "i64"; }
            if (flags == COMPONENT_F32) { output += "f32"; }
            if (flags == COMPONENT_F64) { output += "f64"; }
            break;

        default:
            output += "unk";
        }
    }

    // Sources
    auto sigTypeSrc1 = opInfo.getSignatureSrc1();
    if (sigTypeSrc1 == OPCODE_SIG_TYPE_I || src1.value != nullptr) {
        output += " " + src1.dump(sigTypeSrc1);
    }
    auto sigTypeSrc2 = opInfo.getSignatureSrc2();
    if (sigTypeSrc2 == OPCODE_SIG_TYPE_I || src2.value != nullptr) {
        output += ", " + src2.dump(sigTypeSrc2);
    }
    auto sigTypeSrc3 = opInfo.getSignatureSrc3();
    if (sigTypeSrc3 == OPCODE_SIG_TYPE_I || src3.value != nullptr) {
        output += ", " + src3.dump(sigTypeSrc3);
    }

    output += ";\n";
    return output;
}

}  // namespace hir
}  // namespace cpu
