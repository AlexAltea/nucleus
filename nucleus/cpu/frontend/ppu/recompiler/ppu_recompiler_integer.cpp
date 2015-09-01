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
        // TODO: XER OV update
    } else {
        rd = builder.createAdd(ra, rb);
        // TODO: XER CA update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::addex(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    // TODO: Add XER[CA]
    rd = builder.createAdd(ra, rb);

    if (code.oe) {
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

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
    Value* simm = builder.getConstantI64(code.simm);
    Value* rd;

    rd = builder.createAdd(ra, simm);
    // TODO: XER CA update

    setGPR(code.rd, rd);
}

void Recompiler::addic_(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* simm = builder.getConstantI64(code.simm);
    Value* rd;

    rd = builder.createAdd(ra, simm);
    // TODO: XER CA update
    updateCR0(rd);

    setGPR(code.rd, rd);
}

void Recompiler::addis(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* simm = builder.getConstantI64(code.simm);
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
}

void Recompiler::addzex(Instruction code)
{
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
        updateCR(code.crfd, getGPR(code.ra, TYPE_I64), getGPR(code.rb, TYPE_I64), true);
    } else {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I32), getGPR(code.rb, TYPE_I32), true);
    }
}

void Recompiler::cmpi(Instruction code)
{
    if (code.l10) {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I64), builder.getConstantI64(code.simm), true);
    } else {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I32), builder.getConstantI32(code.simm), true);
    }
}

void Recompiler::cmpl(Instruction code)
{
    if (code.l10) {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I64), getGPR(code.rb, TYPE_I64), false);
    } else {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I32), getGPR(code.rb, TYPE_I32), false);
    }
}

void Recompiler::cmpli(Instruction code)
{
    if (code.l10) {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I64), builder.getConstantI64(code.uimm), false);
    } else {
        updateCR(code.crfd, getGPR(code.ra, TYPE_I32), builder.getConstantI32(code.uimm), false);
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
    ra = builder.createNeg(ra);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::negx(Instruction code)
{
    Value* ra = getGPR(code.ra);
    Value* rd;

    rd = builder.createSub(builder.getConstantI64(0), ra);
    if (code.oe) {
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
    ra = builder.createNeg(ra);
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

    ra = builder.createAnd(ra, rotateMask[mb][63 - sh]);
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

    ra = builder.createAnd(ra, rotateMask[mb][63]);
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

    ra = builder.createAnd(ra, rotateMask[0][me]);
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
    temp = builder.createAnd(temp, mask);
    ra = builder.createAnd(ra, ~mask);
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
    temp = builder.createAnd(temp, mask);
    ra = builder.createAnd(ra, ~mask);
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

    ra = builder.createAnd(ra, rotateMask[32 + code.mb][32 + code.me]);
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

    auto shl = builder.createAnd(rb, 0x1F);
    auto shr = builder.createSub(builder.getConstantI64(32), shl);
    auto resl = builder.createShr(rs, shr);
    auto resh = builder.createShl(rs, shl);
    ra = builder.createOr(resh, resl);
    ra = builder.createAnd(ra, rotateMask[32 + code.mb][32 + code.me]);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::sldx(Instruction code)
{
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
    /*Value* rs = builder.createZExt<I64>(getGPR(code.rs, TYPE_I32));
    Value* rb = builder.createZExt<I64>(builder.createAnd(getGPR(code.rb, TYPE_I8), 0x3F));
    Value* ra;

    ra = builder.createZExt(builder.createTrunc(builder.createShl(rs, rb), TYPE_I32), TYPE_I64);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);*/
}

void Recompiler::sradx(Instruction code)
{
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
    /*Value* rs = builder.createZExt<I64>(getGPR(code.rs, TYPE_I32));
    Value* rb = builder.createZExt<I64>(builder.createAnd(getGPR(code.rb, TYPE_I8), 0x3F));
    Value* ra;

    rs = builder.createShl(rs, 32);
    ra = builder.createAShr(rs, rb);
    ra = builder.createAShr(ra, 32);
    if (code.rc) {
        updateCR0(ra);
    }

    // TODO: Update XER CA

    setGPR(code.ra, ra);*/
}

void Recompiler::srawix(Instruction code)
{
    /*Value* rs = builder.createZExt<I64>(getGPR(code.rs, TYPE_I32));
    Value* ra;

    rs = builder.createShl(rs, 32);
    ra = builder.createAShr(rs, code.sh);
    ra = builder.createAShr(ra, 32);
    if (code.rc) {
        updateCR0(ra);
    }

    // TODO: Update XER CA

    setGPR(code.ra, ra);*/
}

void Recompiler::srdx(Instruction code)
{
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
    /*Value* rs = builder.createZExt<I64>(getGPR(code.rs, TYPE_I32));
    Value* rb = builder.createZExt<I64>(builder.createAnd(getGPR(code.rb, TYPE_I8), 0x3F));
    Value* ra;

    ra = builder.createLShr(rs, rb);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);*/
}

void Recompiler::subfx(Instruction code)
{
    /*Value* ra = getGPR(code.ra);
    Value* rb = getGPR(code.rb);
    Value* rd;

    if (code.oe) {
        // TODO
    } else {
        rd = builder.createSub(rb, ra);
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);*/
}

void Recompiler::subfcx(Instruction code)
{
    Value* ra = builder.createNeg(getGPR(code.ra));
    Value* rb = getGPR(code.rb);
    Value* rd;

    if (code.oe) {
        // TODO: XER OV update
    } else {
        auto result = builder.createAdd(ra, rb);
        // TODO: XER CA update
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::subfex(Instruction code)
{
}

void Recompiler::subfic(Instruction code)
{
}

void Recompiler::subfmex(Instruction code)
{
}

void Recompiler::subfzex(Instruction code)
{
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
