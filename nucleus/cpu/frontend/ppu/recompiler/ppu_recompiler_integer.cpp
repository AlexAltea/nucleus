/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"
#include "nucleus/assert.h"
#include "nucleus/cpu/frontend/ppu/ppu_utils.h"

namespace cpu {
namespace frontend {
namespace ppu {

using namespace cpu::hir;

// Utilities
Value* addDidCarry(Builder& builder, Value* v1, Value* v2) {
    /*// 32-bit
    return builder.createCmpUGT(
        builder.createTrunc(v2, TYPE_I32),
        builder.createNot(builder.createTrunc(v1, TYPE_I32)));*/

    // 64-bit
    return builder.createCmpUGT(v2, builder.createNot(v1));
}

Value* subDidCarry(Builder& builder, Value* v1, Value* v2) {
    /*// 32-bit
    return builder.createOr(
        builder.createCmpUGT(
            builder.createTrunc(v1, TYPE_I32),
            builder.createNot(builder.createNeg(builder.createTrunc(v2, TYPE_I32)))),
        builder.createCmpEQ(
            builder.createTrunc(v2, TYPE_I32),
            builder.getConstantI32(0)));*/

    // 64-bit
    return builder.createOr(
        builder.createCmpUGT(v1, builder.createNot(builder.createNeg(v2))),
        builder.createCmpEQ(v2, builder.getConstantI64(0)));
}

Value* addWithCarryDidCarry(Builder& builder, Value* v1, Value* v2, Value* v3) {
    /*// 32-bit
    v1 = builder.createTrunc(v1, TYPE_I32);
    v2 = builder.createTrunc(v2, TYPE_I32);
    v3 = builder.createZExt(v3, TYPE_I32);
    return builder.createOr(
        builder.createCmpULT(builder.createAdd(builder.createAdd(v1, v2), v3), v3),
        builder.createCmpULT(builder.createAdd(v1, v2), v1));*/

    // 64-bit
    v3 = builder.createZExt(v3, TYPE_I64);
    return builder.createOr(
        builder.createCmpULT(builder.createAdd(builder.createAdd(v1, v2), v3), v3),
        builder.createCmpULT(builder.createAdd(v1, v2), v1));
}

/**
 * PPC64 Instructions:
 *  - UISA: Integer instructions (Section: 4.2.1)
 */

void Recompiler::addx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    if (code.oe) {
        assert_always("Unimplemented: OE flag");
    } else {
        rd = builder.createAdd(ra, rb);
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::addcx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;
    Value* ca;

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    } else {
        rd = builder.createAdd(ra, rb);
        ca = addDidCarry(builder, ra, rb);
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Recompiler::addex(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* ca = getXER_CA();
    Value* rd;

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    } else {
        rd = builder.createAdd(ra, rb);
        rd = builder.createAdd(rd, builder.createZExt(ca, TYPE_I64));
        ca = addWithCarryDidCarry(builder, ra, rb, ca);
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Recompiler::addi(Instruction code)
{
    Value* simm = builder.getConstantI64(code.simm);
    Value* ra;
    Value* rd;

    if (code.ra) {
        ra = getGPR(code.ra);
        rd = builder.createAdd(ra, simm);
    } else {
        rd = simm;
    }

    setGPR(code.rd, rd);
}

void Recompiler::addic(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rd;
    Value* ca;

    Value* simm = builder.getConstantI64(code.simm);
    rd = builder.createAdd(ra, simm);
    ca = addDidCarry(builder, ra, simm);

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Recompiler::addic_(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rd;
    Value* ca;

    Value* simm = builder.getConstantI64(code.simm);
    rd = builder.createAdd(ra, simm);
    ca = addDidCarry(builder, ra, simm);

    updateCR0(rd);

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Recompiler::addis(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* simm = builder.getConstantI64(code.simm << 16);
    Value* rd;

    if (code.ra) {
        rd = builder.createAdd(ra, simm);
    } else {
        rd = simm;
    }

    setGPR(code.rd, rd);
}

void Recompiler::addmex(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* ca = getXER_CA();
    Value* rd;

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    } else {
        rd = builder.createSub(ra, builder.getConstantI64(1));
        rd = builder.createAdd(rd, builder.createZExt(ca, TYPE_I64));
        ca = addWithCarryDidCarry(builder, ra, builder.getConstantI64(-1), ca);
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Recompiler::addzex(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* ca = getXER_CA();
    Value* rd;

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    } else {
        rd = builder.createAdd(ra, builder.createZExt(ca, TYPE_I64));
        ca = addWithCarryDidCarry(builder, ra, builder.getConstantI64(0), ca);
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Recompiler::andx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb);
    Value* ra;

    ra = builder.createAnd(rs, rb);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::andcx(Instruction code)
{
    Value* rb = getGPR(code.rb);
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createAnd(rs, builder.createNot(rb));
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::andi_(Instruction code)
{
    Value* constant = builder.getConstantI64(code.uimm);
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createAnd(rs, constant);
    updateCR0(ra);

    setGPR(code.ra, ra);
}

void Recompiler::andis_(Instruction code)
{
    Value* constant = builder.getConstantI64(code.uimm << 16);
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createAnd(rs, constant);
    updateCR0(ra);

    setGPR(code.ra, ra);
}

void Recompiler::cmp(Instruction code)
{
    if (code.l10) {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I64), getGPR(code.rb, TYPE_I64), false);
    } else {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I32), getGPR(code.rb, TYPE_I32), false);
    }
}

void Recompiler::cmpi(Instruction code)
{
    if (code.l10) {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I64), builder.getConstantI64(code.simm), false);
    } else {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I32), builder.getConstantI32(code.simm), false);
    }
}

void Recompiler::cmpl(Instruction code)
{
    if (code.l10) {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I64), getGPR(code.rb, TYPE_I64), true);
    } else {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I32), getGPR(code.rb, TYPE_I32), true);
    }
}

void Recompiler::cmpli(Instruction code)
{
    if (code.l10) {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I64), builder.getConstantI64(code.uimm), true);
    } else {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I32), builder.getConstantI32(code.uimm), true);
    }
}

void Recompiler::cntlzdx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createCtlz(rs);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::cntlzwx(Instruction code)
{
    Value* rs = getGPR(code.rs);

    Value* ra_i32 = builder.createCtlz(rs);
    Value* ra_i64 = builder.createZExt(ra_i32, TYPE_I64);
    if (code.rc) {
        updateCR0(ra_i64);
    }

    setGPR(code.ra, ra_i64);
}

void Recompiler::divdx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    rd = builder.createDiv(ra, rb, ARITHMETIC_SIGNED);

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::divdux(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    rd = builder.createDiv(ra, rb, ARITHMETIC_UNSIGNED);

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::divwx(Instruction code)
{
    Value* ra = getGPR(code.ra, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I32);
    Value* rd;

    auto result = builder.createDiv(ra, rb, ARITHMETIC_SIGNED);
    rd = builder.createZExt(result, TYPE_I64);

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::divwux(Instruction code)
{
    Value* ra = getGPR(code.ra, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I32);
    Value* rd;

    auto result = builder.createDiv(ra, rb, ARITHMETIC_UNSIGNED);
    rd = builder.createZExt(result, TYPE_I64);

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::eqvx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb);
    Value* ra;

    ra = builder.createXor(rs, rb);
    ra = builder.createNeg(ra);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::extsbx(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I8);
    Value* ra;

    ra = builder.createSExt(rs, TYPE_I64);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::extshx(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I16);
    Value* ra;

    ra = builder.createSExt(rs, TYPE_I64);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::extswx(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I32);
    Value* ra;

    ra = builder.createSExt(rs, TYPE_I64);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::mulhdx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    rd = builder.createMulH(ra, rb, ARITHMETIC_SIGNED);
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::mulhdux(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    rd = builder.createMulH(ra, rb, ARITHMETIC_UNSIGNED);
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::mulhwx(Instruction code)
{
    Value* ra = getGPR(code.ra, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I32);
    Value* rd;

    rd = builder.createMulH(ra, rb, ARITHMETIC_SIGNED);
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::mulhwux(Instruction code)
{
    Value* ra = getGPR(code.ra, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I32);
    Value* rd;

    rd = builder.createMulH(ra, rb, ARITHMETIC_UNSIGNED);
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::mulldx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    rd = builder.createMul(ra, rb, ARITHMETIC_SIGNED);
    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::mulli(Instruction code)
{
    Value* constant = builder.getConstantI64(code.simm);
    Value* ra = getGPR(code.ra);
    Value* rd;

    rd = builder.createMul(ra, constant, ARITHMETIC_SIGNED);

    setGPR(code.rd, rd);
}

void Recompiler::mullwx(Instruction code)
{
    Value* ra = getGPR(code.ra, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I32);
    Value* rd;

    auto ra_i64 = builder.createSExt(ra, TYPE_I64);
    auto rb_i64 = builder.createSExt(rb, TYPE_I64);
    rd = builder.createMul(ra_i64, rb_i64, ARITHMETIC_SIGNED);

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::nandx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb);
    Value* ra;

    ra = builder.createAnd(rs, rb);
    ra = builder.createNot(ra);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::negx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rd;

    rd = builder.createNeg(ra);
    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::norx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb);
    Value* ra;

    ra = builder.createOr(rs, rb);
    ra = builder.createNot(ra);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::orx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb);
    Value* ra;

    ra = builder.createOr(rs, rb);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::orcx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb);
    Value* ra;

    rb = builder.createNot(rb);
    ra = builder.createOr(rs, rb);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::ori(Instruction code)
{
    Value* constant = builder.getConstantI64(code.uimm);
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createOr(rs, constant);

    setGPR(code.ra, ra);
}

void Recompiler::oris(Instruction code)
{
    Value* constant = builder.getConstantI64(code.uimm << 16);
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createOr(rs, constant);

    setGPR(code.ra, ra);
}

void Recompiler::rldc_lr(Instruction code)
{
    assert_always("Unimplemented");
}

void Recompiler::rldicx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* ra = rs;

    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 mb = code.mb | (code.mb_ << 5);
    if (sh) {
        auto resl = builder.createShr(rs, 64 - sh);
        auto resh = builder.createShl(rs, sh);
        ra = builder.createOr(resh, resl);
    }

    ra = builder.createAnd(ra, builder.getConstantI64(rotateMask[mb][63 - sh]));
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::rldiclx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* ra = rs;

    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 mb = code.mb | (code.mb_ << 5);
    if (sh) {
        auto resl = builder.createShr(rs, 64 - sh);
        auto resh = builder.createShl(rs, sh);
        ra = builder.createOr(resh, resl);
    }

    ra = builder.createAnd(ra, builder.getConstantI64(rotateMask[mb][63]));
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::rldicrx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* ra = rs;

    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 me = code.me_ | (code.me__ << 5);
    if (sh) {
        auto resl = builder.createShr(rs, 64 - sh);
        auto resh = builder.createShl(rs, sh);
        ra = builder.createOr(resh, resl);
    }

    ra = builder.createAnd(ra, builder.getConstantI64(rotateMask[0][me]));
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::rldimix(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* ra = getGPR(code.ra);
    Value* temp = rs;

    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 mb = code.mb | (code.mb_ << 5);
    if (sh) {
        auto resl = builder.createShr(rs, 64 - sh);
        auto resh = builder.createShl(rs, sh);
        temp = builder.createOr(resh, resl);
    }
    
    const U64 mask = rotateMask[mb][63 - sh];
    temp = builder.createAnd(temp, builder.getConstantI64(mask));
    ra = builder.createAnd(ra, builder.getConstantI64(~mask));
    ra = builder.createOr(ra, temp);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::rlwimix(Instruction code)
{
    Value* rs_trunc = builder.createZExt(getGPR(code.rs, TYPE_I32), TYPE_I64);
    Value* rs_shift = builder.createShl(rs_trunc, 32);

    Value* rs = builder.createOr(rs_trunc, rs_shift);
    Value* ra = getGPR(code.ra);
    Value* temp = rs;

    if (code.sh) {
        auto resl = builder.createShr(rs, 64 - code.sh);
        auto resh = builder.createShl(rs, code.sh);
        temp = builder.createOr(resh, resl);
    }

    const U64 mask = rotateMask[32 + code.mb][32 + code.me];
    temp = builder.createAnd(temp, builder.getConstantI64(mask));
    ra = builder.createAnd(ra, builder.getConstantI64(~mask));
    ra = builder.createOr(ra, temp);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::rlwinmx(Instruction code)
{
    Value* rs_trunc = builder.createZExt(getGPR(code.rs, TYPE_I32), TYPE_I64);
    Value* rs_shift = builder.createShl(rs_trunc, 32);

    Value* rs = builder.createOr(rs_trunc, rs_shift);
    Value* ra = rs;

    if (code.sh) {
        auto resl = builder.createShr(rs, 64 - code.sh);
        auto resh = builder.createShl(rs, code.sh);
        ra = builder.createOr(resh, resl);
    }

    ra = builder.createAnd(ra, builder.getConstantI64(rotateMask[32 + code.mb][32 + code.me]));
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::rlwnmx(Instruction code)
{
    Value* rs_trunc = builder.createZExt(getGPR(code.rs, TYPE_I32), TYPE_I64);
    Value* rs_shift = builder.createShl(rs_trunc, 32);

    Value* rs = builder.createOr(rs_trunc, rs_shift);
    Value* rb = getGPR(code.rb);
    Value* ra;

    auto shl = builder.createAnd(rb, builder.getConstantI64(0x1F));
    auto shr = builder.createSub(builder.getConstantI64(32), shl);
    auto resl = builder.createShr(rs, shr);
    auto resh = builder.createShl(rs, shl);
    ra = builder.createOr(resh, resl);
    ra = builder.createAnd(ra, builder.getConstantI64(rotateMask[32 + code.mb][32 + code.me]));
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::sldx(Instruction code)
{
    assert_always("Unimplemented");
    /*Value* rs = builder.createZExt<I128>(getGPR(code.rs));
    Value* rb = builder.createZExt<I128>(builder.createAnd(getGPR(code.rb, TYPE_I8), 0x7F));
    Value* ra;

    ra = builder.createTrunc(builder.createShl(rs, rb), TYPE_I64);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);*/
}

void Recompiler::slwx(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I8);
    Value* ra;

    rs = builder.createZExt(rs, TYPE_I64);
    rb = builder.createZExt(builder.createAnd(rb, builder.getConstantI8(0x3F)), TYPE_I64);
    ra = builder.createZExt(builder.createTrunc(builder.createShl(rs, rb), TYPE_I32), TYPE_I64);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::sradx(Instruction code)
{
    assert_always("Unimplemented");
    /*Value* rs = builder.createZExt<I128>(getGPR(code.rs));
    Value* rb = builder.createZExt<I128>(builder.createAnd(getGPR(code.rb, TYPE_I8), 0x7F));
    Value* temp;
    Value* ra;

    rs = builder.createShl(rs, 64);
    temp = builder.createAShr(rs, rb);
    temp = builder.createAShr(temp, 64);
    ra = builder.createTrunc<I64>(temp);
    if (code.rc) {
        updateCR0(ra);
    }

    // TODO: Update XER CA

    setGPR(code.ra, ra);*/
}

void Recompiler::sradix(Instruction code)
{
    assert_always("Unimplemented");
    /*Value* rs = builder.createZExt<I128>(getGPR(code.rs));
    Value* temp;
    Value* ra;

    const U32 sh = code.sh | (code.sh_ << 5);
    rs = builder.createShl(rs, 64);
    temp = builder.createAShr(rs, sh);
    temp = builder.createAShr(temp, 64);
    ra = builder.createTrunc<I64>(temp);
    if (code.rc) {
        updateCR0(ra);
    }

    // TODO: Update XER CA

    setGPR(code.ra, ra);*/
}

void Recompiler::srawx(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I8);
    Value* ra;
    Value* ca;

    rs = builder.createShl(builder.createZExt(rs, TYPE_I64), 32);
    ra = builder.createShrA(rs, builder.createAnd(rb, builder.getConstantI8(0x3F)));
    ra = builder.createShrA(ra, 32);
    if (code.rc) {
        updateCR0(ra);
    }

    // TODO: Update XER CA

    setXER_CA(ca);
    setGPR(code.ra, ra);
}

void Recompiler::srawix(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I32);
    Value* ra;
    Value* ca;

    rs = builder.createShl(builder.createZExt(rs, TYPE_I64), 32);
    ra = builder.createShrA(rs, code.sh);
    ra = builder.createShrA(ra, 32);
    if (code.rc) {
        updateCR0(ra);
    }

    setXER_CA(ca);
    setGPR(code.ra, ra);
}

void Recompiler::srdx(Instruction code)
{
    assert_always("Unimplemented");
    /*Value* rs = builder.createZExt<I128>(getGPR(code.rs));
    Value* rb = builder.createZExt<I128>(builder.createAnd(getGPR(code.rb, TYPE_I8), 0x7F));
    Value* temp;
    Value* ra;

    temp = builder.createLShr(rs, rb);
    ra = builder.createTrunc<I64>(temp);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);*/
}

void Recompiler::srwx(Instruction code)
{
    Value* rs = getGPR(code.rs, TYPE_I32);
    Value* rb = getGPR(code.rb, TYPE_I8);
    Value* ra;

    rs = builder.createZExt(rs, TYPE_I64);
    rb = builder.createZExt(builder.createAnd(rb, builder.getConstantI8(0x3F)), TYPE_I64);
    ra = builder.createShr(rs, rb);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::subfx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER SO, OV update
    } else {
        rd = builder.createSub(rb, ra);
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::subfcx(Instruction code)
{
    Value* ra = builder.createNeg(getGPR(code.ra));
    Value* rb = getGPR(code.rb);
    Value* rd;
    Value* ca;

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    } else {
        rd = builder.createAdd(ra, rb);
        ca = subDidCarry(builder, rb, ra);
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Recompiler::subfex(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* ca = getXER_CA();
    Value* rd;

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    } else {
        rd = builder.createAdd(builder.createNot(ra), rb);
        rd = builder.createAdd(rd, builder.createZExt(ca, TYPE_I64));
        ca = addWithCarryDidCarry(builder, builder.createNot(ra), rb, ca);
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Recompiler::subfic(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rd;
    Value* ca;

    Value* simm = builder.getConstantI64(code.simm);
    rd = builder.createSub(simm, ra);
    ca = subDidCarry(builder, simm, ra);

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Recompiler::subfmex(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rd;
    Value* ca;

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    } else {
        rd = builder.createSub(builder.createNot(ra), builder.getConstantI64(1));
        rd = builder.createAdd(rd, builder.createZExt(getXER_CA(), TYPE_I64));
        ca = addWithCarryDidCarry(builder, builder.createNot(ra), builder.getConstantI64(-1), getXER_CA());
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Recompiler::subfzex(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rd;
    Value* ca;

    if (code.oe) {
        assert_always("Unimplemented");
        // TODO: XER OV update
    } else {
        rd = builder.createNot(ra);
        rd = builder.createAdd(rd, builder.createZExt(getXER_CA(), TYPE_I64));
        ca = addWithCarryDidCarry(builder, builder.createNot(ra), builder.getConstantI64(0), getXER_CA());
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setXER_CA(ca);
    setGPR(code.rd, rd);
}

void Recompiler::xorx(Instruction code)
{
    Value* rs = getGPR(code.rs);
    Value* rb = getGPR(code.rb);
    Value* ra;

    ra = builder.createXor(rs, rb);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::xori(Instruction code)
{
    Value* constant = builder.getConstantI64(code.uimm);
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createXor(rs, constant);

    setGPR(code.ra, ra);
}

void Recompiler::xoris(Instruction code)
{
    Value* constant = builder.getConstantI64(code.uimm << 16);
    Value* rs = getGPR(code.rs);
    Value* ra;

    ra = builder.createXor(rs, constant);

    setGPR(code.ra, ra);
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
