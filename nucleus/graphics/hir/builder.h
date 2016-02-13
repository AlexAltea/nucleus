/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/hir/hir.h"

#include <list>
#include <vector>

namespace gfx {
namespace hir {

// Forward declarations
class Instruction;
class Block;
class Function;
class Module;

class Builder {
    Module* module;
    Function* curFunction;
    Block* curBlock;

    // Return the opcode of an instruction given its result ID
    Literal getOpcode(Literal resultId) const;

    // Return the type of a member in a composite
    Literal getContainedType(Literal typeId, int member = 0) const;

    Literal getType(Literal resultId) const;
    Literal getDerefType(Literal resultId);

    // Constants
    Literal getConstantScalar(Literal resultId) const;

    // Type checks given a type ID
    bool isTypeBool(Literal typeId)         const { return getOpcode(typeId) == OP_TYPE_BOOL; }
    bool isTypePointer(Literal typeId)      const { return getOpcode(typeId) == OP_TYPE_POINTER; }
    bool isTypeVector(Literal typeId)       const { return getOpcode(typeId) == OP_TYPE_VECTOR; }
    bool isTypeMatrix(Literal typeId)       const { return getOpcode(typeId) == OP_TYPE_MATRIX; }
    bool isTypeStruct(Literal typeId)       const { return getOpcode(typeId) == OP_TYPE_STRUCT; }
    bool isTypeArray(Literal typeId)        const { return getOpcode(typeId) == OP_TYPE_ARRAY; }
    bool isTypeImage(Literal typeId)        const { return getOpcode(typeId) == OP_TYPE_IMAGE; }
    bool isTypeSampler(Literal typeId)      const { return getOpcode(typeId) == OP_TYPE_SAMPLER; }
    bool isTypeSampledImage(Literal typeId) const { return getOpcode(typeId) == OP_TYPE_SAMPLED_IMAGE; }

    // Type checks given a value ID
    bool isValuePointer(Literal resultId)   const { return isTypePointer(getType(resultId)); }
    bool isValueVector(Literal resultId)    const { return isTypeVector(getType(resultId)); }
    bool isValueMatrix(Literal resultId)    const { return isTypeMatrix(getType(resultId)); }

private:
    // Instruction to be cached
    enum Cache {
        CACHE_OP_TYPE_VOID = 0,
        CACHE_OP_TYPE_BOOL,
        CACHE_OP_TYPE_INT,
        CACHE_OP_TYPE_FLOAT,
        CACHE_OP_TYPE_VECTOR,
        CACHE_OP_TYPE_MATRIX,
        CACHE_OP_TYPE_ARRAY,
        CACHE_OP_TYPE_STRUCT,
        CACHE_OP_TYPE_POINTER,
        CACHE_OP_TYPE_FUNCTION,

        CACHE_CONSTANT_BOOL,
        CACHE_CONSTANT_INT,
        CACHE_CONSTANT_FLOAT,

        _CACHE_COUNT,
    };
    // Database of created types to ensure unique creation
    std::vector<Instruction*> cache[_CACHE_COUNT];

public:
    // Insertion
    void setModule(Module* module);
    void setInsertionBlock(Block* block);

    // Instruction utilities
    Instruction* createInstr(Opcode opcode, bool hasResultId);
    Instruction* appendInstr(U16 opcode, Literal idType, Literal idResult);
    Instruction* getInstr(Literal instrId);

    // HIR header
    void addCapability(Capability capability);
    void addEntryPoint(ExecutionModel model, Function* function, const char* name, const std::vector<Literal>& io);
    void addMemoryModel(AddressingModel addressingModel, MemoryModel memoryModel);
    void addDecoration(Literal target, Decoration decoration);
    void addDecoration(Literal target, Decoration decoration, Literal arg);
    void addDecoration(Literal target, Decoration decoration, const std::vector<Literal>& args);

    // HIR types
    Literal opTypeVoid();
    Literal opTypeBool();
    Literal opTypeInt(Literal width, bool signedness);
    Literal opTypeFloat(Literal width);
    Literal opTypeVector(Literal componentType, Literal componentCount);
    Literal opTypeMatrix(Literal columnType, Literal columnCount);
    Literal opTypeArray(Literal elementType, int length);
    Literal opTypePointer(StorageClass storageClass, Literal type);
    Literal opTypeFunction(Literal returnType, const std::vector<Literal>& parameters = {});

    // HIR constants
    Literal findScalarConstant(Opcode opcode);
    Literal makeConstantBool(bool c);
    Literal makeConstantInt(S32 c);
    Literal makeConstantUInt(U32 c);
    Literal makeConstantFloat(F32 c);
    Literal makeConstantDouble(F64 c);

    // HIR variables
    Literal opVariable(StorageClass storage, Literal type);

    // HIR operations
    Literal emitUnaryOp(Opcode opcode, Literal value);
    Literal emitBinaryOp(Opcode opcode, Literal lhs, Literal rhs);
    Literal opIAdd(Literal lhs, Literal rhs) { return emitBinaryOp(OP_IADD, lhs, rhs); }
    Literal opISub(Literal lhs, Literal rhs) { return emitBinaryOp(OP_ISUB, lhs, rhs); }
    Literal opIMul(Literal lhs, Literal rhs) { return emitBinaryOp(OP_IMUL, lhs, rhs); }
    Literal opUDiv(Literal lhs, Literal rhs) { return emitBinaryOp(OP_UDIV, lhs, rhs); }
    Literal opSDiv(Literal lhs, Literal rhs) { return emitBinaryOp(OP_SDIV, lhs, rhs); }
    Literal opFAdd(Literal lhs, Literal rhs) { return emitBinaryOp(OP_FADD, lhs, rhs); }
    Literal opFSub(Literal lhs, Literal rhs) { return emitBinaryOp(OP_FSUB, lhs, rhs); }
    Literal opFMul(Literal lhs, Literal rhs) { return emitBinaryOp(OP_FMUL, lhs, rhs); }
    Literal opFDiv(Literal lhs, Literal rhs) { return emitBinaryOp(OP_FDIV, lhs, rhs); }
    Literal opDot(Literal lhs, Literal rhs);
    Literal opFNegate(Literal value);

    Literal opCompositeConstruct(Literal typeId, const std::vector<Literal>& constituents);
    Literal opAccessChain(Literal base, const std::vector<Literal>& indexes);
    Literal opLoad(Literal pointer);
    void opStore(Literal pointer, Literal object);
    Literal opVectorShuffle(Literal resType, Literal vec1, Literal vec2, const std::vector<Literal>& components);

    // GLSL extensions
    Literal opExtFAbs(Literal value);
};

}  // namespace hir
}  // namespace gfx
