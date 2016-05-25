/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "spu_translator.h"
#include "nucleus/assert.h"

namespace cpu {
namespace frontend {
namespace spu {

using namespace cpu::hir;

/**
 * SPU Instructions:
 *  - Memory-Load/Store Instructions (Chapter 3)
 */

// Memory-Load/Store Instructions (Chapter 3)
void Translator::cbd(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    Value* idx = builder.createExtract(ra, builder.getConstantI8(3), TYPE_I32);
    idx = builder.createAdd(idx, builder.getConstantI32(code.i7));
    idx = builder.createNot(idx);
    idx = builder.createAnd(idx, builder.getConstantI32(0xF));
    rt = builder.getConstantV128(V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F));
    rt = builder.createInsert(rt, idx, builder.getConstantI32(0x03));

    setGPR(code.rt, rt);
}

void Translator::cbx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    Value* idx = builder.createAdd(
        builder.createExtract(ra, builder.getConstantI8(3), TYPE_I32),
        builder.createExtract(rb, builder.getConstantI8(3), TYPE_I32));
    idx = builder.createNot(idx);
    idx = builder.createAnd(idx, builder.getConstantI32(0xF));
    rt = builder.getConstantV128(V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F));
    rt = builder.createInsert(rt, idx, builder.getConstantI64(0x0001020304050607ULL));

    setGPR(code.rt, rt);
}

void Translator::cdd(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    Value* idx = builder.createExtract(ra, builder.getConstantI8(3), TYPE_I32);
    idx = builder.createAdd(idx, builder.getConstantI32(code.i7));
    idx = builder.createNot(idx);
    idx = builder.createAnd(idx, builder.getConstantI32(0x8));
    idx = builder.createShr(idx, builder.getConstantI32(0x3));
    rt = builder.getConstantV128(V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F));
    rt = builder.createInsert(rt, idx, builder.getConstantI64(0x0001020304050607ULL));

    setGPR(code.rt, rt);
}

void Translator::cdx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    Value* idx = builder.createAdd(
        builder.createExtract(ra, builder.getConstantI8(3), TYPE_I32),
        builder.createExtract(rb, builder.getConstantI8(3), TYPE_I32));
    idx = builder.createNot(idx);
    idx = builder.createAnd(idx, builder.getConstantI32(0x8));
    idx = builder.createShr(idx, builder.getConstantI32(0x3));
    rt = builder.getConstantV128(V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F));
    rt = builder.createInsert(rt, idx, builder.getConstantI64(0x0001020304050607ULL));

    setGPR(code.rt, rt);
}

void Translator::chd(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    Value* idx = builder.createExtract(ra, builder.getConstantI8(3), TYPE_I32);
    idx = builder.createAdd(idx, builder.getConstantI32(code.i7));
    idx = builder.createNot(idx);
    idx = builder.createAnd(idx, builder.getConstantI32(0xE));
    idx = builder.createShr(idx, builder.getConstantI32(0x1));
    rt = builder.getConstantV128(V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F));
    rt = builder.createInsert(rt, idx, builder.getConstantI32(0x0203));

    setGPR(code.rt, rt);
}

void Translator::chx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    Value* idx = builder.createAdd(
        builder.createExtract(ra, builder.getConstantI8(3), TYPE_I32),
        builder.createExtract(rb, builder.getConstantI8(3), TYPE_I32));
    idx = builder.createNot(idx);
    idx = builder.createAnd(idx, builder.getConstantI32(0xE));
    idx = builder.createShr(idx, builder.getConstantI32(0x1));
    rt = builder.getConstantV128(V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F));
    rt = builder.createInsert(rt, idx, builder.getConstantI64(0x0001020304050607ULL));

    setGPR(code.rt, rt);
}

void Translator::cwd(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    Value* idx = builder.createExtract(ra, builder.getConstantI8(3), TYPE_I32);
    idx = builder.createAdd(idx, builder.getConstantI32(code.i7));
    idx = builder.createNot(idx);
    idx = builder.createAnd(idx, builder.getConstantI32(0xC));
    idx = builder.createShr(idx, builder.getConstantI32(0x2));
    rt = builder.getConstantV128(V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F));
    rt = builder.createInsert(rt, idx, builder.getConstantI32(0x00010203));

    setGPR(code.rt, rt);
}

void Translator::cwx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    Value* idx = builder.createAdd(
        builder.createExtract(ra, builder.getConstantI8(3), TYPE_I32),
        builder.createExtract(rb, builder.getConstantI8(3), TYPE_I32));
    idx = builder.createNot(idx);
    idx = builder.createAnd(idx, builder.getConstantI32(0xC));
    idx = builder.createShr(idx, builder.getConstantI32(0x2));
    rt = builder.getConstantV128(V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F));
    rt = builder.createInsert(rt, idx, builder.getConstantI64(0x0001020304050607ULL));

    setGPR(code.rt, rt);
}

void Translator::lqa(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lqd(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::lqr(Instruction code)
{
    Value* addr = builder.getConstantI32(((code.i16 << 2) + currentAddress) & ~0xF);
    Value* rt = readMemory(addr, TYPE_V128);
    setGPR(code.rt, rt);
}

void Translator::lqx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stqa(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stqd(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::stqr(Instruction code)
{
    Value* rt = getGPR(code.rt);
    Value* addr = builder.getConstantI32(((code.i16 << 2) + currentAddress) & ~0xF);
    writeMemory(addr, rt);
}

void Translator::stqx(Instruction code)
{
    assert_always("Unimplemented");
}

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
