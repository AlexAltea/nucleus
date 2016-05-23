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
 *  - Floating-Point Instructions (Chapter 9)
 */

// Floating-Point Instructions (Chapter 9)
void Translator::cflts(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cfltu(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::csflt(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::cuflt(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::dfa(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVAdd(ra, rb, COMPONENT_F64);

    setGPR(code.rt, rt);
}

void Translator::dfceq(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::dfcgt(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::dfcmeq(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::dfcmgt(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::dfm(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVMul(ra, rb, COMPONENT_F64);

    setGPR(code.rt, rt);
}

void Translator::dfma(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rc = getGPR(code.rc);
    Value* rt;

    rt = builder.createVMul(ra, rb, COMPONENT_F64);
    rt = builder.createVAdd(rt, rc, COMPONENT_F64);

    setGPR(code.rt, rt);
}

void Translator::dfms(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rc = getGPR(code.rc);
    Value* rt;

    rt = builder.createVMul(ra, rb, COMPONENT_F64);
    rt = builder.createVSub(rt, rc, COMPONENT_F64);

    setGPR(code.rt, rt);
}

void Translator::dfnma(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rc = getGPR(code.rc);
    Value* rt;

    rt = builder.createVMul(ra, rb, COMPONENT_F64);
    rt = builder.createVAdd(rt, rc, COMPONENT_F64);
    rt = builder.createFNeg(rt);

    setGPR(code.rt, rt);
}

void Translator::dfnms(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rc = getGPR(code.rc);
    Value* rt;

    rt = builder.createVMul(ra, rb, COMPONENT_F64);
    rt = builder.createVSub(rc, rt, COMPONENT_F64);

    setGPR(code.rt, rt);
}

void Translator::dfs(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVSub(ra, rb, COMPONENT_F64);

    setGPR(code.rt, rt);
}

void Translator::dftsv(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::fa(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVAdd(ra, rb, COMPONENT_F32);

    setGPR(code.rt, rt);
}

void Translator::fceq(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::fcgt(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::fcmeq(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::fcmgt(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::fesd(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::fi(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::fm(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVMul(ra, rb, COMPONENT_F32);

    setGPR(code.rt, rt);
}

void Translator::fma(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rc = getGPR(code.rc);
    Value* rt;

    rt = builder.createVMul(ra, rb, COMPONENT_F32);
    rt = builder.createVAdd(rt, rc, COMPONENT_F32);

    setGPR(code.rt, rt);
}

void Translator::fms(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rc = getGPR(code.rc);
    Value* rt;

    rt = builder.createVMul(ra, rb, COMPONENT_F32);
    rt = builder.createVSub(rt, rc, COMPONENT_F32);

    setGPR(code.rt, rt);
}

void Translator::fnms(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rc = getGPR(code.rc);
    Value* rt;

    rt = builder.createVMul(ra, rb, COMPONENT_F32);
    rt = builder.createVSub(rc, rt, COMPONENT_F32);

    setGPR(code.rt, rt);
}

void Translator::frds(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::frest(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::frsqest(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::fs(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rt;

    rt = builder.createVSub(ra, rb, COMPONENT_F32);

    setGPR(code.rt, rt);
}

void Translator::fscrrd(Instruction code)
{
    assert_always("Unimplemented");
}

void Translator::fscrwr(Instruction code)
{
    assert_always("Unimplemented");
}

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
