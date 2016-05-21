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
 *  - Constant-Formation Instructions (Chapter 4)
 *  - Integer and Logical Instructions (Chapter 5)
 *  - Shift and Rotate Instructions (Chapter 6)
 */

// Constant-Formation Instructions (Chapter 4)
void Translator::fsmbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::il(Instruction code)
{
    V128 value;
    value.s32[0] = code.i16;
    value.s32[1] = code.i16;
    value.s32[2] = code.i16;
    value.s32[3] = code.i16;
    Value* rt = builder.getConstantV128(value);

    setGPR(code.rt, rt);
}

void Translator::ila(Instruction code)
{
    V128 value;
    value.u32[0] = code.i18 & 0x3FFFF;
    value.u32[1] = code.i18 & 0x3FFFF;
    value.u32[2] = code.i18 & 0x3FFFF;
    value.u32[3] = code.i18 & 0x3FFFF;
    Value* rt = builder.getConstantV128(value);

    setGPR(code.rt, rt);
}

void Translator::ilh(Instruction code)
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

void Translator::ilhu(Instruction code)
{
    V128 value;
    value.u32[0] = code.i16 << 16;
    value.u32[1] = code.i16 << 16;
    value.u32[2] = code.i16 << 16;
    value.u32[3] = code.i16 << 16;
    Value* rt = builder.getConstantV128(value);

    setGPR(code.rt, rt);
}

void Translator::iohl(Instruction code)
{
    Value* constant;
    Value* rt = getGPR(code.rt);

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
void Translator::a(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVAdd(ra, rb, TYPE_I32);

    setGPR(code.rt, rt);
}

void Translator::absdb(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVSub(ra, rb, TYPE_I8);
    rt = builder.createVAbs(rt, TYPE_I8);

    setGPR(code.rt, rt);
}

void Translator::addx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt = getGPR(code.rt);

    //rt = builder.createAnd(rt, 1); // TODO
    rt = builder.createVAdd(rt, ra, TYPE_I32);
    rt = builder.createVAdd(rt, rb, TYPE_I32);

    setGPR(code.rt, rt);
}

void Translator::ah(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVAdd(ra, rb, TYPE_I16);

    setGPR(code.rt, rt);
}

void Translator::ahi(Instruction code)
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

void Translator::ai(Instruction code)
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

void Translator::and_(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createAnd(ra, rb);

    setGPR(code.rt, rt);
}

void Translator::andc(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rb = builder.createNot(rb);
    rt = builder.createAnd(ra, rb);

    setGPR(code.rt, rt);
}

void Translator::andbi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    //rt = builder.createAnd(ra, (code.i10 & 0xFF)); // TODO: AND with (code.i10 & 0xFF)^16

    //setGPR(code.rt, rt);
}

void Translator::andhi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    //rt = builder.createAnd(ra, code.i10); // TODO: AND with (code.i10)^8

    //setGPR(code.rt, rt);
}

void Translator::andi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    //rt = builder.createAnd(ra, code.i10); // TODO: AND with (code.i10)^8

    //setGPR(code.rt, rt);
}

void Translator::avgb(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVAvg(ra, rb, TYPE_I8);

    setGPR(code.rt, rt);
}

void Translator::bg(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    auto result = builder.createVCmpUGT(ra, rb, TYPE_I32);
    rt = builder.createZExt(result, TYPE_I32);

    setGPR(code.rt, rt);
}

void Translator::bgx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cg(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cgx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::clz(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cntb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::eqv(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::fsm(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::fsmb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::fsmh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::gb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::gbb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::gbh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mpy(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mpya(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mpyh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mpyhh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mpyhha(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mpyhhau(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mpyhhu(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mpyi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mpys(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mpyu(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::mpyui(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::nand(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::nor(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::or_(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::orbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::orc(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::orhi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::ori(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::orx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::selb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::sf(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::sfh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::sfhi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::sfi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::sfx(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::shufb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::sumb(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::xor_(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::xorbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::xorhi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::xori(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::xsbh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::xshw(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::xswd(Instruction code)
{
    assert_always("Unimplemented");
}


// Shift and Rotate Instructions (Chapter 6)
void Translator::shl(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::shlh(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::shlhi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::shli(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::shlqbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::shlqbii(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::shlqby(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::shlqbybi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::shlqbyi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rot(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::roth(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rothi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rothm(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rothmi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::roti(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotm(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotma(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotmah(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotmahi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotmai(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotmi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotqbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotqbii(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotqby(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotqbybi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotqbyi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotqmbi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotqmbii(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotqmby(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotqmbybi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::rotqmbyi(Instruction code)
{
    assert_always("Unimplemented");
}

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
