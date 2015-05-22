/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"

namespace cpu {
namespace ppu {

using namespace cpu::hir;

/**
 * PPC64 Instructions:
 *  - UISA: Integer instructions (Section: 4.2.1)
 */

void Recompiler::addx(Instruction code)
{
    Value<I64> ra = getGPR(code.ra);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> rd;
    Value<I1> ov;

    if (code.oe) {
        auto result = builder.CreateIntrinsic_SaddWithOverflow(ra, rb);
        rd = builder.CreateExtractValue<0>(result);
        ov = builder.CreateExtractValue<1>(result);
        // TODO: XER OV update
    } else {
        rd = builder.CreateAdd(ra, rb);
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::addcx(Instruction code)
{
    Value<I64> ra = getGPR(code.ra);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> rd;
    Value<I1> ca;

    auto result = builder.CreateIntrinsic_UaddWithOverflow(ra, rb);
    rd = builder.CreateExtractValue<0>(result);
    ca = builder.CreateExtractValue<1>(result);
    // TODO: XER CA update

    if (code.oe) {
        // TODO: XER OV update
    }
    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::addex(Instruction code)
{
    Value<I64> ra = getGPR(code.ra);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> rd;

    // TODO: Add XER[CA]
    rd = builder.CreateAdd(ra, rb);

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
    Value<I64> simm = builder.get<I64>(code.simm);
    Value<I64> ra;
    Value<I64> rd;

    if (code.ra) {
        ra = getGPR(code.ra);
        rd = builder.CreateAdd(ra, simm);
    } else {
        rd = simm;
    }

    setGPR(code.rd, rd);
}

void Recompiler::addic(Instruction code)
{
    Value<I64> ra = getGPR(code.ra);
    Value<I64> simm = builder.get<I64>(code.simm);
    Value<I64> rd;

    rd = builder.CreateAdd(ra, simm);
    // TODO: XER CA update

    setGPR(code.rd, rd);
}

void Recompiler::addic_(Instruction code)
{
    Value<I64> ra = getGPR(code.ra);
    Value<I64> simm = builder.get<I64>(code.simm);
    Value<I64> rd;

    rd = builder.CreateAdd(ra, simm);
    // TODO: XER CA update
    updateCR0(rd);

    setGPR(code.rd, rd);
}

void Recompiler::addis(Instruction code)
{
    Value<I64> ra = getGPR(code.ra);
    Value<I64> simm = builder.get<I64>(code.simm);
    Value<I64> rd;

    if (code.ra) {
        rd = builder.CreateAdd(ra, simm);
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
    Value<I64> rs = getGPR(code.rs);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> ra;

    ra = builder.CreateAnd(rs, rb);
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
    Value<I64> constant = builder.get<I64>(code.uimm);
    Value<I64> rs = getGPR(code.rs);
    Value<I64> ra;

    ra = builder.CreateAnd(rs, constant);
    updateCR0(ra);

    setGPR(code.ra, ra);
}

void Recompiler::andis_(Instruction code)
{
    Value<I64> constant = builder.get<I64>(code.uimm << 16);
    Value<I64> rs = getGPR(code.rs);
    Value<I64> ra;

    ra = builder.CreateAnd(rs, constant);
    updateCR0(ra);

    setGPR(code.ra, ra);
}

void Recompiler::cmp(Instruction code)
{
    if (code.l10) {
        updateCR(code.crfd, getGPR(code.ra), getGPR(code.rb), true);
    } else {
        updateCR(code.crfd, getGPR<I32>(code.ra), getGPR<I32>(code.rb), true);
    }
}

void Recompiler::cmpi(Instruction code)
{
    if (code.l10) {
        updateCR(code.crfd, getGPR(code.ra), builder.get<I64>(code.simm), true);
    } else {
        updateCR(code.crfd, getGPR<I32>(code.ra), builder.get<I32>(code.simm), true);
    }
}

void Recompiler::cmpl(Instruction code)
{
    if (code.l10) {
        updateCR(code.crfd, getGPR(code.ra), getGPR(code.rb), false);
    } else {
        updateCR(code.crfd, getGPR<I32>(code.ra), getGPR<I32>(code.rb), false);
    }
}

void Recompiler::cmpli(Instruction code)
{
    if (code.l10) {
        updateCR(code.crfd, getGPR(code.ra), builder.get<I64>(code.uimm), false);
    } else {
        updateCR(code.crfd, getGPR<I32>(code.ra), builder.get<I32>(code.uimm), false);
    }
}

void Recompiler::cntlzdx(Instruction code)
{
    Value<I64> rs = getGPR(code.rs);
    Value<I64> ra;

    ra = builder.CreateIntrinsic_Ctlz(rs, builder.get<I1>(false));
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::cntlzwx(Instruction code)
{
    Value<I32> rs = getGPR<I32>(code.rs);

    Value<I32> ra_i32 = builder.CreateIntrinsic_Ctlz(rs, builder.get<I1>(false));
    Value<I64> ra_i64 = builder.CreateZExt<I64>(ra_i32);
    if (code.rc) {
        updateCR0(ra_i64);
    }

    setGPR(code.ra, ra_i64);
}

void Recompiler::divdx(Instruction code)
{
    Value<I64> ra = getGPR(code.ra);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> rd;

    rd = builder.CreateSDiv(ra, rb);

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
    Value<I64> ra = getGPR(code.ra);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> rd;

    rd = builder.CreateUDiv(ra, rb);

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
    Value<I32> ra = getGPR<I32>(code.ra);
    Value<I32> rb = getGPR<I32>(code.rb);
    Value<I64> rd;

    auto result = builder.CreateSDiv(ra, rb);
    rd = builder.CreateZExt<I64>(result);

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
    Value<I32> ra = getGPR<I32>(code.ra);
    Value<I32> rb = getGPR<I32>(code.rb);
    Value<I64> rd;

    auto result = builder.CreateUDiv(ra, rb);
    rd = builder.CreateZExt<I64>(result);

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
    Value<I64> rs = getGPR(code.rs);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> ra;

    ra = builder.CreateXor(rs, rb);
    ra = builder.CreateNeg(ra);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::extsbx(Instruction code)
{
    Value<I8> rs = getGPR<I8>(code.rs);
    Value<I64> ra;

    ra = builder.CreateSExt<I64>(rs);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::extshx(Instruction code)
{
    Value<I16> rs = getGPR<I16>(code.rs);
    Value<I64> ra;

    ra = builder.CreateSExt<I64>(rs);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::extswx(Instruction code)
{
    Value<I32> rs = getGPR<I32>(code.rs);
    Value<I64> ra;

    ra = builder.CreateSExt<I64>(rs);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::mulhdx(Instruction code)
{
    Value<I64> ra = getGPR(code.ra);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> rd;

    auto ra_i128 = builder.CreateSExt<I128>(ra);
    auto rb_i128 = builder.CreateSExt<I128>(rb);
    auto result = builder.CreateMul(ra_i128, rb_i128);
    rd = builder.CreateTrunc<I64>(builder.CreateLShr(result, 64));

    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::mulhdux(Instruction code)
{
    Value<I64> ra = getGPR(code.ra);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> rd;

    auto ra_i128 = builder.CreateZExt<I128>(ra);
    auto rb_i128 = builder.CreateZExt<I128>(rb);
    auto result = builder.CreateMul(ra_i128, rb_i128);
    rd = builder.CreateTrunc<I64>(builder.CreateLShr(result, 64));

    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::mulhwx(Instruction code)
{
    Value<I32> ra = getGPR<I32>(code.ra);
    Value<I32> rb = getGPR<I32>(code.rb);
    Value<I64> rd;

    auto ra_i64 = builder.CreateSExt<I64>(ra);
    auto rb_i64 = builder.CreateSExt<I64>(rb);
    auto result = builder.CreateMul(ra_i64, rb_i64);
    rd = builder.CreateAShr(result, 32);

    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::mulhwux(Instruction code)
{
    Value<I32> ra = getGPR<I32>(code.ra);
    Value<I32> rb = getGPR<I32>(code.rb);
    Value<I64> rd;

    auto ra_i64 = builder.CreateZExt<I64>(ra);
    auto rb_i64 = builder.CreateZExt<I64>(rb);
    auto result = builder.CreateMul(ra_i64, rb_i64);
    rd = builder.CreateLShr(result, 32);

    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::mulldx(Instruction code)
{
    Value<I64> ra = getGPR(code.ra);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> rd;

    rd = builder.CreateMul(ra, rb);

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
    Value<I64> ra = getGPR(code.ra);
    Value<I64> rd;

    rd = builder.CreateMul(ra, builder.get<I64>(code.simm));

    setGPR(code.rd, rd);
}

void Recompiler::mullwx(Instruction code)
{
    Value<I32> ra = getGPR<I32>(code.ra);
    Value<I32> rb = getGPR<I32>(code.rb);
    Value<I64> rd;

    auto ra_i64 = builder.CreateSExt<I64>(ra);
    auto rb_i64 = builder.CreateSExt<I64>(rb);
    rd = builder.CreateMul(ra_i64, rb_i64);

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
    Value<I64> rs = getGPR(code.rs);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> ra;

    ra = builder.CreateAnd(rs, rb);
    ra = builder.CreateNeg(ra);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::negx(Instruction code)
{
    Value<I64> ra = getGPR(code.ra);
    Value<I64> rd;

    rd = builder.CreateSub(builder.get<I64>(0), ra);
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
    Value<I64> rs = getGPR(code.rs);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> ra;

    ra = builder.CreateOr(rs, rb);
    ra = builder.CreateNeg(ra);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::orx(Instruction code)
{
    Value<I64> rs = getGPR(code.rs);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> ra;

    ra = builder.CreateOr(rs, rb);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::orcx(Instruction code)
{
    Value<I64> rs = getGPR(code.rs);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> ra;

    rb = builder.CreateNot(rb);
    ra = builder.CreateOr(rs, rb);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::ori(Instruction code)
{
    Value<I64> constant = builder.get<I64>(code.uimm);
    Value<I64> rs = getGPR(code.rs);
    Value<I64> ra;

    ra = builder.CreateOr(rs, constant);

    setGPR(code.ra, ra);
}

void Recompiler::oris(Instruction code)
{
    Value<I64> constant = builder.get<I64>(code.uimm << 16);
    Value<I64> rs = getGPR(code.rs);
    Value<I64> ra;

    ra = builder.CreateOr(rs, constant);

    setGPR(code.ra, ra);
}

void Recompiler::rldc_lr(Instruction code)
{
}

void Recompiler::rldicx(Instruction code)
{
}

void Recompiler::rldiclx(Instruction code)
{
}

void Recompiler::rldicrx(Instruction code)
{
}

void Recompiler::rldimix(Instruction code)
{
}

void Recompiler::rlwimix(Instruction code)
{
}

void Recompiler::rlwinmx(Instruction code)
{
}

void Recompiler::rlwnmx(Instruction code)
{
}

void Recompiler::sldx(Instruction code)
{
}

void Recompiler::slwx(Instruction code)
{
}

void Recompiler::sradx(Instruction code)
{
}

void Recompiler::sradix(Instruction code)
{
}

void Recompiler::srawx(Instruction code)
{
}

void Recompiler::srawix(Instruction code)
{
}

void Recompiler::srdx(Instruction code)
{
}

void Recompiler::srwx(Instruction code)
{
}

void Recompiler::subfx(Instruction code)
{
}

void Recompiler::subfcx(Instruction code)
{
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
    Value<I64> rs = getGPR(code.rs);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> ra;

    ra = builder.CreateXor(rs, rb);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::xori(Instruction code)
{
    Value<I64> constant = builder.get<I64>(code.uimm);
    Value<I64> rs = getGPR(code.rs);
    Value<I64> ra;

    ra = builder.CreateXor(rs, constant);

    setGPR(code.ra, ra);
}

void Recompiler::xoris(Instruction code)
{
    Value<I64> constant = builder.get<I64>(code.uimm << 16);
    Value<I64> rs = getGPR(code.rs);
    Value<I64> ra;

    ra = builder.CreateXor(rs, constant);

    setGPR(code.ra, ra);
}

}  // namespace ppu
}  // namespace cpu
