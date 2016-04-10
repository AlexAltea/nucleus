/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "rsx_vp.h"
#include "nucleus/assert.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/graphics.h"
#include "nucleus/graphics/hir/block.h"
#include "nucleus/graphics/hir/function.h"
#include "nucleus/graphics/hir/module.h"

using namespace gfx::hir;

namespace gpu {
namespace rsx {

// Output registers
struct RSXVertexProgramOutput {
    int indexOutput;
    std::vector<Literal> decorations;
};
const RSXVertexProgramOutput rsxOutputs[] = {
    {  0, { DECORATION_BUILTIN,   BUILTIN_POSITION } },
    {  1, { DECORATION_LOCATION,  1} },
    {  2, { DECORATION_LOCATION,  2} },
    {  3, { DECORATION_LOCATION,  3} },
    {  4, { DECORATION_LOCATION,  4} },
    {  5, { DECORATION_LOCATION,  5} }, // TODO: ClipDistance/Fog
    {  6, { DECORATION_LOCATION,  6} }, // TODO: ClipDistance/PointSize
    {  7, { DECORATION_LOCATION,  7} },
    {  8, { DECORATION_LOCATION,  8} },
    {  9, { DECORATION_LOCATION,  9} },
    { 10, { DECORATION_LOCATION, 10} },
    { 11, { DECORATION_LOCATION, 11} },
    { 12, { DECORATION_LOCATION, 12} },
    { 13, { DECORATION_LOCATION, 13} },
    { 14, { DECORATION_LOCATION, 14} },
    { 15, { DECORATION_LOCATION, 15} },
};

RSXVertexProgram::RSXVertexProgram() {
    module = std::make_unique<Module>();
    builder.setModule(module.get());
}

Literal RSXVertexProgram::getDataReg(int index) {
    Literal& dataReg = data[index];
    if (!dataReg) {
        dataReg = builder.opVariable(STORAGE_CLASS_PRIVATE, vecTypeId);
    }
    return dataReg;
}

Literal RSXVertexProgram::getInputReg(int index) {
    Literal& inputReg = inputs[index];
    if (!inputReg) {
        inputReg = builder.opVariable(STORAGE_CLASS_INPUT, vecTypeId);
        entryPointInterface.push_back(inputReg);
        builder.addDecoration(inputReg, DECORATION_LOCATION, index);
    }
    return inputReg;
}

Literal RSXVertexProgram::getOutputReg(int index) {
    Literal& outputReg = outputs[index];
    if (!outputReg) {
        outputReg = builder.opVariable(STORAGE_CLASS_OUTPUT, vecTypeId);
        const auto decoration = static_cast<Decoration>(rsxOutputs[index].decorations[0]);
        const auto argument = static_cast<Literal>(rsxOutputs[index].decorations[1]);
        builder.addDecoration(outputReg, decoration, argument);
        entryPointInterface.push_back(outputReg);
    }
    return outputReg;
}

Literal RSXVertexProgram::getConstantReg(int index) {
    Literal offsetId = builder.makeConstantInt(index);
    return builder.opAccessChain(constMemoryId, {offsetId});
}

Literal RSXVertexProgram::getSwizzledValue(Literal vector, U08 swizzle) {
    // Check if swizzling is required. Note that: [00,01,10,11] -> [0,1,2,3] -> [x,y,z,w].
    if (swizzle == 0b00011011) {
        return vector;
    }
    Literal x = (swizzle >> 6) & 0b11;
    Literal y = (swizzle >> 4) & 0b11;
    Literal z = (swizzle >> 2) & 0b11;
    Literal w = (swizzle >> 0) & 0b11;
    return builder.opVectorShuffle(vecTypeId, vector, vector, {x,y,z,w});
}

Literal RSXVertexProgram::getSourceVector(int index) {
    // Get source descriptor
    rsx_vp_instruction_source_t source;
    switch (index) {
    case 0: source.value = (instr.src0_hi << 9) | instr.src0_lo; break;
    case 1: source.value = (instr.src1); break;
    case 2: source.value = (instr.src2_hi << 11) | instr.src2_lo; break;
    }

    // Get source value
    Literal sourceId = 0;
    switch (source.type) {
    case RSX_VP_REGISTER_TYPE_DATA:
        sourceId = builder.opLoad(getDataReg(source.index));
        break;
    case RSX_VP_REGISTER_TYPE_INPUT:
        sourceId = builder.opLoad(getInputReg(instr.src_input));
        break;
    case RSX_VP_REGISTER_TYPE_CONSTANT:
        sourceId = builder.opLoad(getConstantReg(instr.src_const));
        break;
    }

    // Swizzling
    sourceId = getSwizzledValue(sourceId, source.swizzling);

    // Absolute value
    switch (index) {
    case 0: sourceId = instr.abs_src0 ? builder.opExtFAbs(sourceId) : sourceId; break;
    case 1: sourceId = instr.abs_src1 ? builder.opExtFAbs(sourceId) : sourceId; break;
    case 2: sourceId = instr.abs_src2 ? builder.opExtFAbs(sourceId) : sourceId; break;
    }

    // Negated value
    if (source.neg) {
        sourceId = builder.opFNegate(sourceId);
    }
    return sourceId;
}


void RSXVertexProgram::setDestVector(Literal value) {
    // Get destination register pointer
    Literal pointer;
    if (instr.dst == 0x1F) {
        pointer = getDataReg(instr.dst_data);
    } else {
        pointer = getOutputReg(instr.dst);
    }
    
    // Masked write
    U08 mask = instr.masc_vec;
    if (mask == 0b0000) {  // Check if mask disables all components
        return; 
    }
    if (mask == 0b1111) {  // Check if mask enables all components
        builder.opStore(pointer, value);
        return;
    }
    Literal x = ((mask >> 3) & 1) ? 4 : 0;
    Literal y = ((mask >> 2) & 1) ? 5 : 1;
    Literal z = ((mask >> 1) & 1) ? 6 : 2;
    Literal w = ((mask >> 0) & 1) ? 7 : 3;
    Literal dest = builder.opLoad(pointer);
    Literal maskVal = builder.opVectorShuffle(vecTypeId, dest, value, {x,y,z,w});
    builder.opStore(pointer, maskVal);
}

void RSXVertexProgram::decompile(const rsx_vp_instruction_t* buffer) {
    // Basic types
    Literal floatType = builder.opTypeFloat(32);
    vecTypeId = builder.opTypeVector(floatType, 4);
    constMemoryId = builder.opVariable(STORAGE_CLASS_UNIFORM_CONSTANT, builder.opTypeArray(vecTypeId, 468));

    Function* function = new Function(*module.get(), builder.opTypeFunction(builder.opTypeVoid()));
    Block* block = new Block(*function);
    builder.setInsertionBlock(block);

    data.resize(48);
    inputs.resize(16);
    outputs.resize(16);

    // Temporary values
    Literal src0, src1, src2;
    Literal tmp0, tmp1, tmp2;

    // Shader body
    do {
        instr = *buffer;

        // SCA Opcodes
        switch (instr.opcode_sca) {
        case RSX_VP_OPCODE_SCA_NOP:
            break;
        default:
            logger.error(LOG_GPU, "VPE: Unknown SCA opcode (%d)", instr.opcode_sca);
        }

        // VEC Opcodes
        switch (instr.opcode_vec) {
        case RSX_VP_OPCODE_VEC_NOP:
            break;
        case RSX_VP_OPCODE_VEC_MOV:
            src0 = getSourceVector(0);
            setDestVector(src0);
            break;
        case RSX_VP_OPCODE_VEC_MUL:
            src0 = getSourceVector(0);
            src1 = getSourceVector(1);
            setDestVector(builder.opFMul(src0, src1));
            break;
        case RSX_VP_OPCODE_VEC_MAD:
            src0 = getSourceVector(0);
            src1 = getSourceVector(1);
            src2 = getSourceVector(2);
            setDestVector(builder.opFAdd(builder.opFMul(src0, src1), src2));
            break;
        case RSX_VP_OPCODE_VEC_FRC:
            assert_always("Unimplemented");
            //source += format("%s = fract(%s)%s;", DST(), SRC(0), get_vp_mask(instr.masc_vec));
            break;
        case RSX_VP_OPCODE_VEC_FLR:
            assert_always("Unimplemented");
            //source += format("%s = floor(%s)%s;", DST(), SRC(0), get_vp_mask(instr.masc_vec));
            break;
        case RSX_VP_OPCODE_VEC_DP4:
            src0 = getSourceVector(0);
            src1 = getSourceVector(1);
            tmp0 = builder.opDot(src0, src1);
            setDestVector(builder.opCompositeConstruct(vecTypeId, {tmp0, tmp0, tmp0, tmp0}));
            break;
        default:
            logger.error(LOG_GPU, "VPE: Unknown VEC opcode (%d)", instr.opcode_vec);
        }
    } while (!(buffer++)->end);

    // Header
    builder.addCapability(CAPABILITY_SHADER);
    builder.addEntryPoint(EXECUTION_MODEL_VERTEX, function, "main", entryPointInterface);
    builder.addMemoryModel(ADDRESSING_MODEL_LOGICAL, MEMORY_MODEL_GLSL450);
}

void RSXVertexProgram::compile(gfx::IBackend* backend) {
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
    desc.type = gfx::SHADER_TYPE_VERTEX;
    desc.module = module.get();
    shader = backend->createShader(desc);
}

}  // namespace rsx
}  // namespace gpu
