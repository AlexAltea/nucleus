/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"
#include "nucleus/cpu/frontend/ppu/ppu_utils.h"

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
    Value<I64> rs = getGPR(code.rs);
    Value<I64> ra = rs;

    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 mb = code.mb | (code.mb_ << 5);
    if (sh) {
        auto resl = builder.CreateLShr(rs, 64 - sh);
        auto resh = builder.CreateShl(rs, sh);
        ra = builder.CreateOr(resh, resl);
    }

    ra = builder.CreateAnd(ra, rotateMask[mb][63 - sh]);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::rldiclx(Instruction code)
{
    Value<I64> rs = getGPR(code.rs);
    Value<I64> ra = rs;

    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 mb = code.mb | (code.mb_ << 5);
    if (sh) {
        auto resl = builder.CreateLShr(rs, 64 - sh);
        auto resh = builder.CreateShl(rs, sh);
        ra = builder.CreateOr(resh, resl);
    }

    ra = builder.CreateAnd(ra, rotateMask[mb][63]);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::rldicrx(Instruction code)
{
    Value<I64> rs = getGPR(code.rs);
    Value<I64> ra = rs;

    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 me = code.me_ | (code.me__ << 5);
    if (sh) {
        auto resl = builder.CreateLShr(rs, 64 - sh);
        auto resh = builder.CreateShl(rs, sh);
        ra = builder.CreateOr(resh, resl);
    }

    ra = builder.CreateAnd(ra, rotateMask[0][me]);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::rldimix(Instruction code)
{
    Value<I64> rs = getGPR(code.rs);
    Value<I64> ra = getGPR(code.ra);
    Value<I64> temp = rs;

    const U32 sh = code.sh | (code.sh_ << 5);
    const U32 mb = code.mb | (code.mb_ << 5);
    if (sh) {
        auto resl = builder.CreateLShr(rs, 64 - sh);
        auto resh = builder.CreateShl(rs, sh);
        temp = builder.CreateOr(resh, resl);
    }
    
    const U64 mask = rotateMask[mb][63 - sh];
    temp = builder.CreateAnd(temp, mask);
    ra = builder.CreateAnd(ra, ~mask);
    ra = builder.CreateOr(ra, temp);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::rlwimix(Instruction code)
{
    Value<I64> rs_trunc = builder.CreateZExt<I64>(getGPR<I32>(code.rs));
    Value<I64> rs_shift = builder.CreateShl(rs_trunc, 32);

    Value<I64> rs = builder.CreateOr(rs_trunc, rs_shift);
    Value<I64> ra = getGPR(code.ra);
    Value<I64> temp = rs;

    if (code.sh) {
        auto resl = builder.CreateLShr(rs, 64 - code.sh);
        auto resh = builder.CreateShl(rs, code.sh);
        temp = builder.CreateOr(resh, resl);
    }

    const U64 mask = rotateMask[32 + code.mb][32 + code.me];
    temp = builder.CreateAnd(temp, mask);
    ra = builder.CreateAnd(ra, ~mask);
    ra = builder.CreateOr(ra, temp);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::rlwinmx(Instruction code)
{
    Value<I64> rs_trunc = builder.CreateZExt<I64>(getGPR<I32>(code.rs));
    Value<I64> rs_shift = builder.CreateShl(rs_trunc, 32);

    Value<I64> rs = builder.CreateOr(rs_trunc, rs_shift);
    Value<I64> ra = rs;

    if (code.sh) {
        auto resl = builder.CreateLShr(rs, 64 - code.sh);
        auto resh = builder.CreateShl(rs, code.sh);
        ra = builder.CreateOr(resh, resl);
    }

    ra = builder.CreateAnd(ra, rotateMask[32 + code.mb][32 + code.me]);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::rlwnmx(Instruction code)
{
    Value<I64> rs_trunc = builder.CreateZExt<I64>(getGPR<I32>(code.rs));
    Value<I64> rs_shift = builder.CreateShl(rs_trunc, 32);

    Value<I64> rs = builder.CreateOr(rs_trunc, rs_shift);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> ra;

    auto shl = builder.CreateAnd(rb, 0x1F);
    auto shr = builder.CreateSub(builder.get<I64>(32), shl);
    auto resl = builder.CreateLShr(rs, shr);
    auto resh = builder.CreateShl(rs, shl);
    ra = builder.CreateOr(resh, resl);
    ra = builder.CreateAnd(ra, rotateMask[32 + code.mb][32 + code.me]);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::sldx(Instruction code)
{
    Value<I128> rs = builder.CreateZExt<I128>(getGPR(code.rs));
    Value<I128> rb = builder.CreateZExt<I128>(builder.CreateAnd(getGPR<I8>(code.rb), 0x7F));
    Value<I64> ra;

    ra = builder.CreateTrunc<I64>(builder.CreateShl(rs, rb));
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::slwx(Instruction code)
{
    Value<I64> rs = builder.CreateZExt<I64>(getGPR<I32>(code.rs));
    Value<I64> rb = builder.CreateZExt<I64>(builder.CreateAnd(getGPR<I8>(code.rb), 0x3F));
    Value<I64> ra;

    ra = builder.CreateZExt<I64>(builder.CreateTrunc<I32>(builder.CreateShl(rs, rb)));
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::sradx(Instruction code)
{
    Value<I128> rs = builder.CreateZExt<I128>(getGPR(code.rs));
    Value<I128> rb = builder.CreateZExt<I128>(builder.CreateAnd(getGPR<I8>(code.rb), 0x7F));
    Value<I128> temp;
    Value<I64> ra;

    rs = builder.CreateShl(rs, 64);
    temp = builder.CreateAShr(rs, rb);
    temp = builder.CreateAShr(temp, 64);
    ra = builder.CreateTrunc<I64>(temp);
    if (code.rc) {
        updateCR0(ra);
    }

    // TODO: Update XER CA

    setGPR(code.ra, ra);
}

void Recompiler::sradix(Instruction code)
{
    Value<I128> rs = builder.CreateZExt<I128>(getGPR(code.rs));
    Value<I128> temp;
    Value<I64> ra;

    const U32 sh = code.sh | (code.sh_ << 5);
    rs = builder.CreateShl(rs, 64);
    temp = builder.CreateAShr(rs, sh);
    temp = builder.CreateAShr(temp, 64);
    ra = builder.CreateTrunc<I64>(temp);
    if (code.rc) {
        updateCR0(ra);
    }

    // TODO: Update XER CA

    setGPR(code.ra, ra);
}

void Recompiler::srawx(Instruction code)
{
    Value<I64> rs = builder.CreateZExt<I64>(getGPR<I32>(code.rs));
    Value<I64> rb = builder.CreateZExt<I64>(builder.CreateAnd(getGPR<I8>(code.rb), 0x3F));
    Value<I64> ra;

    rs = builder.CreateShl(rs, 32);
    ra = builder.CreateAShr(rs, rb);
    ra = builder.CreateAShr(ra, 32);
    if (code.rc) {
        updateCR0(ra);
    }

    // TODO: Update XER CA

    setGPR(code.ra, ra);
}

void Recompiler::srawix(Instruction code)
{
    Value<I64> rs = builder.CreateZExt<I64>(getGPR<I32>(code.rs));
    Value<I64> ra;

    rs = builder.CreateShl(rs, 32);
    ra = builder.CreateAShr(rs, code.sh);
    ra = builder.CreateAShr(ra, 32);
    if (code.rc) {
        updateCR0(ra);
    }

    // TODO: Update XER CA

    setGPR(code.ra, ra);
}

void Recompiler::srdx(Instruction code)
{
    Value<I128> rs = builder.CreateZExt<I128>(getGPR(code.rs));
    Value<I128> rb = builder.CreateZExt<I128>(builder.CreateAnd(getGPR<I8>(code.rb), 0x7F));
    Value<I128> temp;
    Value<I64> ra;

    temp = builder.CreateLShr(rs, rb);
    ra = builder.CreateTrunc<I64>(temp);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::srwx(Instruction code)
{
    Value<I64> rs = builder.CreateZExt<I64>(getGPR<I32>(code.rs));
    Value<I64> rb = builder.CreateZExt<I64>(builder.CreateAnd(getGPR<I8>(code.rb), 0x3F));
    Value<I64> ra;

    ra = builder.CreateLShr(rs, rb);
    if (code.rc) {
        updateCR0(ra);
    }

    setGPR(code.ra, ra);
}

void Recompiler::subfx(Instruction code)
{
    Value<I64> ra = getGPR(code.ra);
    Value<I64> rb = getGPR(code.rb);
    Value<I64> rd;

    if (code.oe) {
        // TODO
    } else {
        rd = builder.CreateSub(rb, ra);
    }

    if (code.rc) {
        updateCR0(rd);
    }

    setGPR(code.rd, rd);
}

void Recompiler::subfcx(Instruction code)
{
    Value<I64> ra = builder.CreateNeg(getGPR(code.ra));
    Value<I64> rb = getGPR(code.rb);
    Value<I64> rd;

    if (code.oe) {
        // TODO
    } else {
        auto result = builder.CreateIntrinsic_UaddWithOverflow(ra, rb);
        rd = builder.CreateExtractValue<0>(result);
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
