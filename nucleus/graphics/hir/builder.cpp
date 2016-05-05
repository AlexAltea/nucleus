/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "builder.h"
#include "nucleus/graphics/hir/block.h"
#include "nucleus/graphics/hir/function.h"
#include "nucleus/graphics/hir/instruction.h"
#include "nucleus/graphics/hir/module.h"
#include "nucleus/assert.h"

namespace gfx {
namespace hir {

Literal Builder::getOpcode(Literal resultId) const {
    return module->idInstructions[resultId]->opcode;
}

Literal Builder::getContainedType(Literal typeId, int member) const {
    Instruction* instr = module->idInstructions[typeId];
    switch (instr->opcode) {
    case OP_TYPE_VECTOR:
    case OP_TYPE_MATRIX:
    case OP_TYPE_ARRAY:
    case OP_TYPE_RUNTIME_ARRAY:
        return instr->operands[0];
    case OP_TYPE_POINTER:
        return instr->operands[1];
    case OP_TYPE_STRUCT:
        return instr->operands[member];
    default:
        assert_always("Unexpected");
        return 0;
    }
}

Literal Builder::getType(Literal resultId) const {
    return module->idInstructions[resultId]->typeId;
}

Literal Builder::getDerefType(Literal resultId) {
    Literal ptrType = getType(resultId);
    Instruction* ptrTypeInstr = module->idInstructions[ptrType];
    assert_true(ptrTypeInstr->opcode == OP_TYPE_POINTER);
    return ptrTypeInstr->operands[1];
}

Literal Builder::getConstantScalar(Literal resultId) const {
    return module->idInstructions[resultId]->operands[0];
}

// Instruction utilities
Instruction* Builder::createInstr(Opcode opcode, bool hasResultId) {
    Instruction* instr = new Instruction();
    instr->opcode = opcode;
    instr->typeId = 0;

    if (hasResultId) {
        Literal resultId = module->idInstructions.size();
        module->idInstructions.push_back(instr);
        instr->resultId = resultId;
    } else {
        instr->resultId = 0;
    }
    return instr;
}

Instruction* Builder::appendInstr(U16 opcode, Literal typeId, Literal resultId) {
    Instruction* instr = new Instruction();
    instr->opcode = opcode;
    instr->typeId = typeId;
    instr->resultId = resultId;

    if (resultId != 0) {
        module->idInstructions[resultId] = instr;
    }
    return instr;
}

Instruction* Builder::getInstr(Literal instrId) {
    if (instrId >= module->idInstructions.size()) {
        return nullptr;
    }
    return module->idInstructions[instrId];
}

// Insertion
void Builder::setModule(Module* m) {
    module = m;
}
void Builder::setInsertionBlock(Block* block) {
    curBlock = block;
    curFunction = &block->getParent();
}

// HIR header
void Builder::addCapability(Capability capability) {
    Instruction* i = createInstr(OP_CAPABILITY, false);
    i->addOperandImmediate(capability);
    module->hCapabilities.push_back(i);
}

void Builder::addEntryPoint(ExecutionModel model, Function* function, const char* name, const std::vector<Literal>& io) {
    Instruction* i = createInstr(OP_ENTRY_POINT, false);
    i->operands.push_back(model);
    i->addOperandLiteral(function->getId());
    i->addOperandString(name);
    i->addOperandArray(io);
    module->hEntryPoints.push_back(i);
}

void Builder::addMemoryModel(AddressingModel addressingModel, MemoryModel memoryModel) {
    Instruction* i = createInstr(OP_MEMORY_MODEL, false);
    i->addOperandImmediate(addressingModel);
    i->addOperandImmediate(memoryModel);
    module->hMemoryModel.push_back(i);
}

void Builder::addDecoration(Literal target, Decoration decoration, Literal arg) {
    Instruction* i = createInstr(OP_DECORATE, false);
    i->addOperandLiteral(target);
    i->addOperandImmediate(decoration);
    i->addOperandLiteral(arg);
    module->hAnnotation.push_back(i);
}

// HIR types
Literal Builder::opTypeVoid() {
    // Search in cache
    if (!cache[CACHE_OP_TYPE_VOID].empty()) {
        return cache[CACHE_OP_TYPE_VOID][0]->resultId;
    }
    // Make type
    Instruction* i = createInstr(OP_TYPE_VOID, true);
    cache[CACHE_OP_TYPE_VOID].push_back(i);
    module->hConstsTypesGlobs.push_back(i);
    return i->resultId;
}

Literal Builder::opTypeBool() {
    // Search in cache
    if (!cache[CACHE_OP_TYPE_BOOL].empty()) {
        return cache[CACHE_OP_TYPE_BOOL][0]->resultId;
    }
    // Make type
    Instruction* i = createInstr(OP_TYPE_BOOL, true);
    cache[CACHE_OP_TYPE_BOOL].push_back(i);
    module->hConstsTypesGlobs.push_back(i);
    return i->resultId;
}

Literal Builder::opTypeInt(Literal width, bool signedness) {
    // Search in cache
    for (const auto* t : cache[CACHE_OP_TYPE_INT]) {
        if (t->operands[0] == width && t->operands[1] == signedness) {
            return t->resultId;
        }
    }
    // Make type
    Instruction* i = createInstr(OP_TYPE_INT, true);
    i->operands.push_back(width);
    i->operands.push_back(signedness);
    cache[CACHE_OP_TYPE_INT].push_back(i);
    module->hConstsTypesGlobs.push_back(i);
    return i->resultId;
}

Literal Builder::opTypeFloat(Literal width) {
    // Search in cache
    for (const auto* t : cache[CACHE_OP_TYPE_FLOAT]) {
        if (t->operands[0] == width) {
            return t->resultId;
        }
    }
    // Make type
    Instruction* i = createInstr(OP_TYPE_FLOAT, true);
    i->operands.push_back(width);
    cache[CACHE_OP_TYPE_FLOAT].push_back(i);
    module->hConstsTypesGlobs.push_back(i);
    return i->resultId;
}

Literal Builder::opTypeVector(Literal componentType, Literal componentCount) {
    // Search in cache
    for (const auto* t : cache[CACHE_OP_TYPE_VECTOR]) {
        if (t->operands[0] == componentType && t->operands[1] == componentCount) {
            return t->resultId;
        }
    }
    // Make type
    Instruction* i = createInstr(OP_TYPE_VECTOR, true);
    i->operands.push_back(componentType);
    i->operands.push_back(componentCount);
    cache[CACHE_OP_TYPE_VECTOR].push_back(i);
    module->hConstsTypesGlobs.push_back(i);
    return i->resultId;
}

Literal Builder::opTypeMatrix(Literal columnType, Literal columnCount) {
    // Search in cache
    for (const auto* t : cache[CACHE_OP_TYPE_MATRIX]) {
        if (t->operands[0] == columnType && t->operands[1] == columnCount) {
            return t->resultId;
        }
    }
    // Make type
    Instruction* i = createInstr(OP_TYPE_MATRIX, true);
    i->operands.push_back(columnType);
    i->operands.push_back(columnCount);
    cache[CACHE_OP_TYPE_MATRIX].push_back(i);
    module->hConstsTypesGlobs.push_back(i);
    return i->resultId;
}

Literal Builder::opTypeArray(Literal elementType, int length) {
    Literal lengthId = makeConstantUInt(length);
    // Search in cache
    for (const auto* t : cache[CACHE_OP_TYPE_POINTER]) {
        if (t->operands[0] == elementType && t->operands[1] == lengthId) {
            return t->resultId;
        }
    }
    // Make type
    Instruction* i = createInstr(OP_TYPE_ARRAY, true);
    i->operands.push_back(elementType);
    i->operands.push_back(lengthId);
    cache[CACHE_OP_TYPE_ARRAY].push_back(i);
    module->hConstsTypesGlobs.push_back(i);
    return i->resultId;
}

Literal Builder::opTypePointer(StorageClass storageClass, Literal type) {
    // Search in cache
    for (const auto* t : cache[CACHE_OP_TYPE_POINTER]) {
        if (t->operands[0] == storageClass && t->operands[1] == type) {
            return t->resultId;
        }
    }
    // Make type
    Instruction* i = createInstr(OP_TYPE_POINTER, true);
    i->operands.push_back(storageClass);
    i->addOperandLiteral(type);
    cache[CACHE_OP_TYPE_POINTER].push_back(i);
    module->hConstsTypesGlobs.push_back(i);
    return i->resultId;
}

Literal Builder::opTypeImage(Literal sampledType, Dimension dim, Literal depth, Literal arrayed, Literal ms, Literal sampled, ImageFormat format) {
    // TODO: Search in cache
    // Make type
    Instruction* i = createInstr(OP_TYPE_IMAGE, true);
    i->operands.push_back(sampledType);
    i->operands.push_back(dim);
    i->operands.push_back(depth);
    i->operands.push_back(arrayed);
    i->operands.push_back(ms);
    i->operands.push_back(sampled);
    i->operands.push_back(format);
    module->hConstsTypesGlobs.push_back(i);
    return i->resultId;
}

Literal Builder::opTypeSampledImage(Literal imageTypeId) {
    // TODO: Search in cache
    // Make type
    Instruction* i = createInstr(OP_TYPE_SAMPLED_IMAGE, true);
    i->operands.push_back(imageTypeId);
    module->hConstsTypesGlobs.push_back(i);
    return i->resultId;
}

Literal Builder::opTypeFunction(Literal returnType, const std::vector<Literal>& parameters) {
    // Search in cache
    for (const auto* t : cache[CACHE_OP_TYPE_FUNCTION]) {
        if (t->operands[0] != returnType || parameters.size() == (t->operands.size() - 1)) {
            continue;
        }
        bool mismatch = false;
        for (Size p = 0; p < parameters.size(); p++) {
            if (parameters[p] != t->operands[p + 1]) {
                mismatch = true;
                break;
            }
        }
        if (!mismatch) {
            return t->resultId;
        }
    }
    // Make type
    Instruction* i = createInstr(OP_TYPE_FUNCTION, true);
    i->addOperandLiteral(returnType);
    for (const auto& param : parameters) {
        i->addOperandLiteral(param);
    }
    cache[CACHE_OP_TYPE_FUNCTION].push_back(i);
    module->hConstsTypesGlobs.push_back(i);
    return i->resultId;
}

// HIR constants
Literal Builder::makeConstantInt(S32 c) {
    Literal typeId = opTypeInt(32, 1);
    for (const auto* i : cache[CACHE_CONSTANT_INT]) {
        if (reinterpret_cast<const S32&>(i->operands[0]) == c) {
            return i->resultId;
        }
    }
    Instruction* i = createInstr(OP_CONSTANT, true);
    i->typeId = typeId;
    i->operands.push_back(c);
    cache[CACHE_CONSTANT_INT].push_back(i);
    module->hConstsTypesGlobs.push_back(i);
    return i->resultId;
}

Literal Builder::makeConstantUInt(U32 c) {
    Literal typeId = opTypeInt(32, 0);
    for (const auto* i : cache[CACHE_CONSTANT_UINT]) {
        if (reinterpret_cast<const S32&>(i->operands[0]) == c) {
            return i->resultId;
        }
    }
    Instruction* i = createInstr(OP_CONSTANT, true);
    i->typeId = typeId;
    i->operands.push_back(c);
    cache[CACHE_CONSTANT_INT].push_back(i);
    module->hConstsTypesGlobs.push_back(i);
    return i->resultId;
}

Literal Builder::makeConstantFloat(F32 c) {
    Literal typeId = opTypeFloat(32);
    for (const auto* i : cache[CACHE_CONSTANT_FLOAT]) {
        if (reinterpret_cast<const float&>(i->operands[0]) == c) {
            return i->resultId;
        }
    }
    Instruction* i = createInstr(OP_CONSTANT, true);
    i->typeId = typeId;
    i->operands.push_back(c);
    cache[CACHE_CONSTANT_FLOAT].push_back(i);
    module->hConstsTypesGlobs.push_back(i);
    return i->resultId;
}

Literal Builder::makeConstantComposite(Literal typeId, const std::vector<Literal>& constants) {
    Instruction* i = createInstr(OP_CONSTANT_COMPOSITE, true);
    i->typeId = typeId;
    // TODO: Verify correctness of each member's type below
    for (const auto& c : constants) {
        i->operands.push_back(c);
    }
    module->hConstsTypesGlobs.push_back(i);
    return i->resultId;
}

// HIR variables
Literal Builder::opVariable(StorageClass storage, Literal type) {
    Instruction* i = createInstr(OP_VARIABLE, true);
    i->typeId = opTypePointer(storage, type);
    i->operands.push_back(storage);
    if (storage == STORAGE_CLASS_FUNCTION) {
        curFunction->addLocalVariable(i);
    } else {
        module->hConstsTypesGlobs.push_back(i);
    }
    return i->resultId;
}

// HIR operations
Literal Builder::emitBinaryOp(Opcode opcode, Literal lhs, Literal rhs) {
    Instruction* i = createInstr(opcode, true);
    i->typeId = getType(lhs);
    i->addOperandLiteral(lhs);
    i->addOperandLiteral(rhs);
    curBlock->instructions.push_back(i);
    return i->resultId;
}

Literal Builder::opDot(Literal lhs, Literal rhs) {
    Instruction* i = createInstr(OP_DOT, true);
    i->typeId = getContainedType(getType(lhs));
    i->addOperandLiteral(lhs);
    i->addOperandLiteral(rhs);
    curBlock->instructions.push_back(i);
    return i->resultId;
}

Literal Builder::opFNegate(Literal value) {
    Instruction* i = createInstr(OP_FNEGATE, true);
    i->typeId = getType(value);
    curBlock->instructions.push_back(i);
    return i->resultId;
}

Literal Builder::opImageSampleImplicitLod(Literal typeId, Literal image, Literal coordinate) {
    Instruction* i = createInstr(OP_IMAGE_SAMPLE_IMPLICIT_LOD, true);
    i->typeId = typeId;
    i->operands.push_back(image);
    i->operands.push_back(coordinate);
    curBlock->instructions.push_back(i);
    return i->resultId;
}

Literal Builder::opCompositeConstruct(Literal typeId, const std::vector<Literal>& constituents) {
    assert(constituents.size() > 1);
    Instruction* i = createInstr(OP_COMPOSITE_CONSTRUCT, true);
    i->typeId = typeId;
    for (const auto& c : constituents) {
        i->operands.push_back(c);
    }
    curBlock->instructions.push_back(i);
    return i->resultId;
}

Literal Builder::opAccessChain(Literal base, const std::vector<Literal>& indexes) {
    Literal baseTypeId = getType(base);
    assert_true(indexes.size() > 0);
    assert_true(isTypePointer(baseTypeId));

    // Figure out the final resulting type.
    Literal typeId = getContainedType(baseTypeId);
    Literal storageClass = module->idInstructions[baseTypeId]->operands[0];
    for (const auto& index : indexes) {
        if (isTypeStruct(typeId)) {
            typeId = getContainedType(typeId, getConstantScalar(index));
        } else {
            typeId = getContainedType(typeId, 0);
        }
    }
    typeId = opTypePointer(static_cast<StorageClass>(storageClass), typeId);

    Instruction* i = createInstr(OP_ACCESS_CHAIN, true);
    i->typeId = typeId;
    i->operands.push_back(base);
    for (const auto& index : indexes) {
        i->operands.push_back(index);
    }
    curBlock->instructions.push_back(i);
    return i->resultId;
}

Literal Builder::opLoad(Literal pointer) {
    Instruction* i = createInstr(OP_LOAD, true);
    i->typeId = getDerefType(pointer);
    i->addOperandLiteral(pointer);
    curBlock->instructions.push_back(i);
    return i->resultId;
}

void Builder::opStore(Literal pointer, Literal object) {
    Instruction* i = createInstr(OP_STORE, false);
    i->addOperandLiteral(pointer);
    i->addOperandLiteral(object);
    curBlock->instructions.push_back(i);
}

Literal Builder::opVectorShuffle(Literal resType, Literal vec1, Literal vec2, const std::vector<Literal>& components) {
    Instruction* i = createInstr(OP_VECTOR_SHUFFLE, true);
    i->typeId = resType;
    i->addOperandLiteral(vec1);
    i->addOperandLiteral(vec2);
    i->addOperandArray(components);
    curBlock->instructions.push_back(i);
    return i->resultId;
}

// GLSL extensions
Literal Builder::opExtFAbs(Literal value) {
    assert_always("Unimplemented");
    Instruction* i = createInstr(OP_NOP, true);
    return i->resultId;
}

}  // namespace hir
}  // namespace gfx
