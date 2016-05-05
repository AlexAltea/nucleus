/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "rsx_fp.h"
#include "nucleus/assert.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/graphics.h"
#include "nucleus/graphics/hir/block.h"
#include "nucleus/graphics/hir/function.h"
#include "nucleus/graphics/hir/module.h"

using namespace gfx::hir;

namespace gpu {
namespace rsx {

// Input registers
struct RSXFragmentProgramInput {
    int indexInput;
    std::vector<Literal> decorations;
};
const RSXFragmentProgramInput rsxInputs[] = {
    {  0, { DECORATION_BUILTIN,   BUILTIN_FRAGCOORD } },
    {  1, { DECORATION_LOCATION,  1} },
    {  2, { DECORATION_LOCATION,  2} },
    {  3, { DECORATION_LOCATION,  3} },
    {  4, { DECORATION_LOCATION,  4} },
    {  5, { DECORATION_LOCATION,  5} },
    {  6, { DECORATION_LOCATION,  6} },
    {  7, { DECORATION_LOCATION,  7} },
    {  8, { DECORATION_LOCATION,  8} },
    {  9, { DECORATION_LOCATION,  9} },
    { 10, { DECORATION_LOCATION, 10} },
    { 11, { DECORATION_LOCATION, 11} },
    { 12, { DECORATION_LOCATION, 12} },
    { 13, { DECORATION_LOCATION, 13} },
    { 14, { DECORATION_LOCATION, 14} },
};

// Output registers
struct RSXFragmentProgramOutput {
    int indexTempSingle;
    int indexTempHalf;
    std::vector<Literal> decorations;
};
const RSXFragmentProgramOutput rsxOutputs[] = {
    { 0, 0, { DECORATION_LOCATION, 0} },
    { 1, 2, { DECORATION_BUILTIN,  BUILTIN_FRAGDEPTH } },
    { 2, 4, { DECORATION_LOCATION, 1} },
    { 3, 6, { DECORATION_LOCATION, 2} },
    { 4, 8, { DECORATION_LOCATION, 3} },
};

RSXFragmentProgram::RSXFragmentProgram() {
    module = std::make_unique<Module>();
    builder.setModule(module.get());
}

Literal RSXFragmentProgram::getInputReg(int index) {
    Literal& inputReg = inputs[index];
    if (!inputReg) {
        inputReg = builder.opVariable(STORAGE_CLASS_INPUT, vecTypeId);
        const auto decoration = static_cast<Decoration>(rsxInputs[index].decorations[0]);
        const auto argument = static_cast<Literal>(rsxInputs[index].decorations[1]);
        builder.addDecoration(inputReg, decoration, argument);
        entryPointInterface.push_back(inputReg);
    }
    return inputReg;
}

Literal RSXFragmentProgram::getSamplerReg(int index) {
    Literal& samplerReg = samplers[index];
    if (!samplerReg) {
        samplerReg = builder.opVariable(STORAGE_CLASS_UNIFORM_CONSTANT, samplerTypeId);
        builder.addDecoration(samplerReg, DECORATION_BINDING, index);
    }
    return builder.opLoad(samplerReg);
}

Literal RSXFragmentProgram::getTempReg(int index, bool isHalf) {
    Literal& tempReg = tempsSingle[index];
    if (!tempReg) {
        tempReg = builder.opVariable(STORAGE_CLASS_FUNCTION, vecTypeId);
    }
    return tempReg;
}

Literal RSXFragmentProgram::getMaskedValue(Literal dest, Literal source, U08 mask) {
    // Check if mask enables all components
    if (mask == 0b1111) {
        return source;
    }
    // Check if mask disables all components
    if (mask == 0b0000) {
        return dest;
    }
    Literal x = (mask >> 3) ? 4 : 0;
    Literal y = (mask >> 2) ? 5 : 1;
    Literal z = (mask >> 1) ? 6 : 2;
    Literal w = (mask >> 0) ? 7 : 3;
    return builder.opVectorShuffle(vecTypeId, dest, source, {x,y,z,w});
}

Literal RSXFragmentProgram::getSwizzledValue(Literal vector, U08 swizzle) {
    // Check if swizzling is required. Note that: [11,10,01,00] -> [x,y,z,w].
    if (swizzle == 0b11100100) {
        return vector;
    }
    Literal x = (swizzle >> 0) & 0b11;
    Literal y = (swizzle >> 2) & 0b11;
    Literal z = (swizzle >> 4) & 0b11;
    Literal w = (swizzle >> 6) & 0b11;
    return builder.opVectorShuffle(vecTypeId, vector, vector, {x,y,z,w});
}

Literal RSXFragmentProgram::getSourceVector(int index) {
    // Get source descriptor
    const rsx_fp_instruction_source_t source = { instr.word[index+1] & 0x3FFFF };

    // Get source value
    Literal sourceId = 0;
    switch (source.type) {
    case RSX_FP_REGISTER_TYPE_TEMP:
        sourceId = builder.opLoad(getTempReg(source.index, source.half));
        break;
    case RSX_FP_REGISTER_TYPE_INPUT:
        sourceId = builder.opLoad(getInputReg(instr.input_index));
        break;
    case RSX_FP_REGISTER_TYPE_CONSTANT:
        sourceId = builder.makeConstantComposite(vecTypeId, {
            builder.makeConstantFloat(reinterpret_cast<const F32&>(instr_ptr->word[0])),
            builder.makeConstantFloat(reinterpret_cast<const F32&>(instr_ptr->word[1])),
            builder.makeConstantFloat(reinterpret_cast<const F32&>(instr_ptr->word[2])),
            builder.makeConstantFloat(reinterpret_cast<const F32&>(instr_ptr->word[3])),
        });
        instr_ptr++;
        break;
    }

    // Swizzling
    sourceId = getSwizzledValue(sourceId, source.swizzling);

    // Absolute value
    // TODO

    // Negated value
    if (source.neg) {
        sourceId = builder.opFNegate(sourceId);
    }
    return sourceId;
}

void RSXFragmentProgram::setDestVector(Literal value) {
    if (instr.dst_index >= 48) {
        logger.error(LOG_GPU, "Fragment program: Destination register out of range");
    }
    Literal pointer = getTempReg(instr.dst_index, instr.dst_half);
    builder.opStore(pointer, value);
    // TODO: getMaskedValue. //return format("%s[%d]%s", instr.dst_half ? "h" : "r", instr.dst_index, get_fp_mask(instr.dst_mask));
}

void RSXFragmentProgram::decompile(const rsx_fp_instruction_t* buffer) {
    // Basic types
    Literal floatType = builder.opTypeFloat(32);
    vecTypeId = builder.opTypeVector(floatType, 4);
    samplerTypeId = builder.opTypeSampledImage(
        builder.opTypeImage(floatType, DIMENSION_2D, 0, 0, 0, 1, IMAGE_FORMAT_UNKNOWN)
    );

    Function* function = new Function(*module.get(), builder.opTypeFunction(builder.opTypeVoid()));
    Block* block = new Block(*function);
    builder.setInsertionBlock(block);

    inputs.resize(15);
    tempsSingle.resize(48);
    tempsHalf.resize(48);
    samplers.resize(16);

    // TODO: Always place FragCoord position as input. Do the same with other inputs and outputs in all shader stages
    getInputReg(0);

    // Temporary values
    Literal src0, src1, src2;
    Literal tmp0, tmp1, tmp2;
    Literal tex;

    // Shader body
    instr_ptr = buffer;
    do {
        // Set current instruction
        instr.word[0] = get_word<U32>(instr_ptr->word[0]);
        instr.word[1] = get_word<U32>(instr_ptr->word[1]);
        instr.word[2] = get_word<U32>(instr_ptr->word[2]);
        instr.word[3] = get_word<U32>(instr_ptr->word[3]);
        instr_ptr++;

        switch (instr.opcode) {
        case RSX_FP_OPCODE_NOP:
            break;
        case RSX_FP_OPCODE_MOV:
            src0 = getSourceVector(0);
            setDestVector(src0);
            break;
        case RSX_FP_OPCODE_MUL:
            src0 = getSourceVector(0);
            src1 = getSourceVector(1);
            setDestVector(builder.opFMul(src0, src1));
            break;
        case RSX_FP_OPCODE_TEX:
            src0 = getSourceVector(0);
            tex = getSamplerReg(instr.tex);
            setDestVector(builder.opImageSampleImplicitLod(vecTypeId, tex, src0));
            break;
        case RSX_FP_OPCODE_FENCB:
            break;
        default:
            logger.error(LOG_GPU, "Fragment program: Unknown opcode (%d)", instr.opcode);
        }
    } while (!instr.end);

    // Output registers
    for (const auto& reg : rsxOutputs) {
        Literal& tempReg = tempsSingle[reg.indexTempSingle];
        if (tempReg) {
            Literal outputReg = builder.opVariable(STORAGE_CLASS_OUTPUT, vecTypeId);
            builder.opStore(outputReg, tempReg);
        }
    }

    // Header
    builder.addCapability(CAPABILITY_SHADER);
    builder.addEntryPoint(EXECUTION_MODEL_PIXEL, function, "main", entryPointInterface);
    builder.addMemoryModel(ADDRESSING_MODEL_LOGICAL, MEMORY_MODEL_GLSL450);
}

void RSXFragmentProgram::compile(gfx::IBackend* backend) {
    // TODO: Remove this
    for (const auto& i : module->hCapabilities)     { module->header.push_back(i); }
    for (const auto& i : module->hExtensions)       { module->header.push_back(i); }
    for (const auto& i : module->hImports)          { module->header.push_back(i); }
    for (const auto& i : module->hMemoryModel)      { module->header.push_back(i); }
    for (const auto& i : module->hEntryPoints)      { module->header.push_back(i); }
    for (const auto& i : module->hExecutions)       { module->header.push_back(i); }
    for (const auto& i : module->hDebugs)           { module->header.push_back(i); }
    for (const auto& i : module->hAnnotation)       { module->header.push_back(i); }
    for (const auto& i : module->hConstsTypesGlobs) { module->header.push_back(i); }

    gfx::ShaderDesc desc = {};
    desc.type = gfx::SHADER_TYPE_PIXEL;
    desc.module = module.get();
    shader = backend->createShader(desc);
}

}  // namespace rsx
}  // namespace gpu
