/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "spu_recompiler.h"

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
}

void Recompiler::il(Instruction code)
{
}

void Recompiler::ila(Instruction code)
{
}

void Recompiler::ilh(Instruction code)
{
}

void Recompiler::ilhu(Instruction code)
{
}

void Recompiler::iohl(Instruction code)
{
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

    rt = builder.createAnd(rt, 1);
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
    Value<I16, 8> ra = getGPR<I16>(code.ra);
    Value<I16, 8> rt;

    rt = builder.createAdd(ra, code.i10); // TODO: ADD with (code.i10)^8

    setGPR(code.rt, rt);
}

void Recompiler::ai(Instruction code)
{
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

    rt = builder.createAnd(ra, (code.i10 & 0xFF)); // TODO: AND with (code.i10 & 0xFF)^16

    setGPR(code.rt, rt);
}

void Recompiler::andhi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    rt = builder.createAnd(ra, code.i10); // TODO: AND with (code.i10)^8

    setGPR(code.rt, rt);
}

void Recompiler::andi(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rt;

    rt = builder.createAnd(ra, code.i10); // TODO: AND with (code.i10)^8

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
    rt = builder.createZExt<I32>(result);

    setGPR(code.rt, rt);
}

void Recompiler::bgx(Instruction code)
{
}

void Recompiler::cg(Instruction code)
{
}

void Recompiler::cgx(Instruction code)
{
}

void Recompiler::clz(Instruction code)
{
}

void Recompiler::cntb(Instruction code)
{
}

void Recompiler::eqv(Instruction code)
{
}

void Recompiler::fsm(Instruction code)
{
}

void Recompiler::fsmb(Instruction code)
{
}

void Recompiler::fsmh(Instruction code)
{
}

void Recompiler::gb(Instruction code)
{
}

void Recompiler::gbb(Instruction code)
{
}

void Recompiler::gbh(Instruction code)
{
}

void Recompiler::mpy(Instruction code)
{
}

void Recompiler::mpya(Instruction code)
{
}

void Recompiler::mpyh(Instruction code)
{
}

void Recompiler::mpyhh(Instruction code)
{
}

void Recompiler::mpyhha(Instruction code)
{
}

void Recompiler::mpyhhau(Instruction code)
{
}

void Recompiler::mpyhhu(Instruction code)
{
}

void Recompiler::mpyi(Instruction code)
{
}

void Recompiler::mpys(Instruction code)
{
}

void Recompiler::mpyu(Instruction code)
{
}

void Recompiler::mpyui(Instruction code)
{
}

void Recompiler::nand(Instruction code)
{
}

void Recompiler::nor(Instruction code)
{
}

void Recompiler::or_(Instruction code)
{
}

void Recompiler::orbi(Instruction code)
{
}

void Recompiler::orc(Instruction code)
{
}

void Recompiler::orhi(Instruction code)
{
}

void Recompiler::ori(Instruction code)
{
}

void Recompiler::orx(Instruction code)
{
}

void Recompiler::selb(Instruction code)
{
}

void Recompiler::sf(Instruction code)
{
}

void Recompiler::sfh(Instruction code)
{
}

void Recompiler::sfhi(Instruction code)
{
}

void Recompiler::sfi(Instruction code)
{
}

void Recompiler::sfx(Instruction code)
{
}

void Recompiler::shufb(Instruction code)
{
}

void Recompiler::sumb(Instruction code)
{
}

void Recompiler::xor_(Instruction code)
{
}

void Recompiler::xorbi(Instruction code)
{
}

void Recompiler::xorhi(Instruction code)
{
}

void Recompiler::xori(Instruction code)
{
}

void Recompiler::xsbh(Instruction code)
{
}

void Recompiler::xshw(Instruction code)
{
}

void Recompiler::xswd(Instruction code)
{
}


// Shift and Rotate Instructions (Chapter 6)
void Recompiler::shl(Instruction code)
{
}

void Recompiler::shlh(Instruction code)
{
}

void Recompiler::shlhi(Instruction code)
{
}

void Recompiler::shli(Instruction code)
{
}

void Recompiler::shlqbi(Instruction code)
{
}

void Recompiler::shlqbii(Instruction code)
{
}

void Recompiler::shlqby(Instruction code)
{
}

void Recompiler::shlqbybi(Instruction code)
{
}

void Recompiler::shlqbyi(Instruction code)
{
}

void Recompiler::rot(Instruction code)
{
}

void Recompiler::roth(Instruction code)
{
}

void Recompiler::rothi(Instruction code)
{
}

void Recompiler::rothm(Instruction code)
{
}

void Recompiler::rothmi(Instruction code)
{
}

void Recompiler::roti(Instruction code)
{
}

void Recompiler::rotm(Instruction code)
{
}

void Recompiler::rotma(Instruction code)
{
}

void Recompiler::rotmah(Instruction code)
{
}

void Recompiler::rotmahi(Instruction code)
{
}

void Recompiler::rotmai(Instruction code)
{
}

void Recompiler::rotmi(Instruction code)
{
}

void Recompiler::rotqbi(Instruction code)
{
}

void Recompiler::rotqbii(Instruction code)
{
}

void Recompiler::rotqby(Instruction code)
{
}

void Recompiler::rotqbybi(Instruction code)
{
}

void Recompiler::rotqbyi(Instruction code)
{
}

void Recompiler::rotqmbi(Instruction code)
{
}

void Recompiler::rotqmbii(Instruction code)
{
}

void Recompiler::rotqmby(Instruction code)
{
}

void Recompiler::rotqmbybi(Instruction code)
{
}

void Recompiler::rotqmbyi(Instruction code)
{
}

}  // namespace spu
}  // namespace cpu
