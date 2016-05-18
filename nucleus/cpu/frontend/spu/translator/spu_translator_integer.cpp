/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "spu_recompiler.h"
#include "nucleus/assert.h"

namespace cpu {
namespace spu {

using namespace cpu::hir;

/**
 * SPU Instructions:
 *  - Constant-Formation Instructions (Chapter 4)
 *  - Integer and Logical Instructions (Chapter 5)
 *  - Shift and Rotate Instructions (Chapter 6)
 */

// Constant-Formation Instructions (Chapter 4)
void Recompiler::fsmbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::il(Instruction code)
{
    V128 value;
    value.s32[0] = code.i16;
    value.s32[1] = code.i16;
    value.s32[2] = code.i16;
    value.s32[3] = code.i16;
    Value* rt = builder.getConstantV128(value);

    setGPR(code.rt, rt);
}

void Recompiler::ila(Instruction code)
{
    V128 value;
    value.u32[0] = code.i18 & 0x3FFFF;
    value.u32[1] = code.i18 & 0x3FFFF;
    value.u32[2] = code.i18 & 0x3FFFF;
    value.u32[3] = code.i18 & 0x3FFFF;
    Value* rt = builder.getConstantV128(value);

    setGPR(code.rt, rt);
}

void Recompiler::ilh(Instruction code)
{
    V128 value;
    value.u16[0] = code.i16;
    value.u16[1] = code.i16;
    value.u16[2] = code.i16;
    value.u16[3] = code.i16;
    value.u16[4] = code.i16;
    value.u16[5] = code.i16;
    value.u16[6] = code.i16;
    value.u16[7] = code.i16;
    Value* rt = builder.getConstantV128(value);

    setGPR(code.rt, rt);
}

void Recompiler::ilhu(Instruction code)
{
    V128 value;
    value.u32[0] = code.i16 << 16;
    value.u32[1] = code.i16 << 16;
    value.u32[2] = code.i16 << 16;
    value.u32[3] = code.i16 << 16;
    Value* rt = builder.getConstantV128(value);

    setGPR(code.rt, rt);
}

void Recompiler::iohl(Instruction code)
{
    Value* constant;
    Value* rt;

    V128 value;
    value.u32[0] = code.i16 & 0xFFFF;
    value.u32[1] = code.i16 & 0xFFFF;
    value.u32[2] = code.i16 & 0xFFFF;
    value.u32[3] = code.i16 & 0xFFFF;
    constant = builder.getConstantV128(value);
    rt = builder.createOr(rt, constant);

    setGPR(code.rt, rt);
}


// Integer and Logical Instructions (Chapter 5)
void Recompiler::a(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVAdd(ra, rb, TYPE_I32);

    setGPR(code.rt, rt);
}

void Recompiler::absdb(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVSub(ra, rb, TYPE_I8);
    rt = builder.createVAbs(rt, TYPE_I8);

    setGPR(code.rt, rt);
}

void Recompiler::addx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt = getGPR(code.rt);

    //rt = builder.createAnd(rt, 1); // TODO
    rt = builder.createVAdd(rt, ra, TYPE_I32);
    rt = builder.createVAdd(rt, rb, TYPE_I32);

    setGPR(code.rt, rt);
}

void Recompiler::ah(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVAdd(ra, rb, TYPE_I16);

    setGPR(code.rt, rt);
}

void Recompiler::ahi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    V128 value;
    value.u16[0] = code.i10;
    value.u16[1] = code.i10;
    value.u16[2] = code.i10;
    value.u16[3] = code.i10;
    value.u16[4] = code.i10;
    value.u16[5] = code.i10;
    value.u16[6] = code.i10;
    value.u16[7] = code.i10;
    rt = builder.getConstantV128(value);
    rt = builder.createAdd(rt, ra);

    setGPR(code.rt, rt);
}

void Recompiler::ai(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    V128 value;
    value.u32[0] = code.i10;
    value.u32[1] = code.i10;
    value.u32[2] = code.i10;
    value.u32[3] = code.i10;
    rt = builder.getConstantV128(value);
    rt = builder.createAdd(rt, ra);

    setGPR(code.rt, rt);
}

void Recompiler::and_(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createAnd(ra, rb);

    setGPR(code.rt, rt);
}

void Recompiler::andc(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rb = builder.createNot(rb);
    rt = builder.createAnd(ra, rb);

    setGPR(code.rt, rt);
}

void Recompiler::andbi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    //rt = builder.createAnd(ra, (code.i10 & 0xFF)); // TODO: AND with (code.i10 & 0xFF)^16

    setGPR(code.rt, rt);
}

void Recompiler::andhi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    //rt = builder.createAnd(ra, code.i10); // TODO: AND with (code.i10)^8

    setGPR(code.rt, rt);
}

void Recompiler::andi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    //rt = builder.createAnd(ra, code.i10); // TODO: AND with (code.i10)^8

    setGPR(code.rt, rt);
}

void Recompiler::avgb(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVAvg(ra, rb, TYPE_I8);

    setGPR(code.rt, rt);
}

void Recompiler::bg(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    auto result = builder.createVCmpUGT(ra, rb, TYPE_I32);
    rt = builder.createZExt(result, TYPE_I32);

    setGPR(code.rt, rt);
}

void Recompiler::bgx(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::cg(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::cgx(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::clz(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::cntb(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::eqv(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::fsm(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::fsmb(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::fsmh(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::gb(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::gbb(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::gbh(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::mpy(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::mpya(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::mpyh(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::mpyhh(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::mpyhha(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::mpyhhau(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::mpyhhu(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::mpyi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::mpys(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::mpyu(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::mpyui(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::nand(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::nor(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::or_(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::orbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::orc(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::orhi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::ori(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::orx(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::selb(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::sf(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::sfh(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::sfhi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::sfi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::sfx(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::shufb(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::sumb(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::xor_(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::xorbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::xorhi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::xori(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::xsbh(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::xshw(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::xswd(Instruction code)
{
    assert_always("Unimplemented");
}


// Shift and Rotate Instructions (Chapter 6)
void Recompiler::shl(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::shlh(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::shlhi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::shli(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::shlqbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::shlqbii(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::shlqby(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::shlqbybi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::shlqbyi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rot(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::roth(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rothi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rothm(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rothmi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::roti(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotm(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotma(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotmah(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotmahi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotmai(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotmi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotqbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotqbii(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotqby(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotqbybi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotqbyi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotqmbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotqmbii(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotqmby(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotqmbybi(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rotqmbyi(Instruction code)
{
    assert_always("Unimplemented");
}

}  // namespace spu
}  // namespace cpu
