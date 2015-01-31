/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"

namespace cpu {
namespace ppu {

/**
 * PPC64 Instructions:
 *  - UISA: Integer instructions (Section: 4.2.1)
 */

void Recompiler::addx(Instruction code)
{
    llvm::Value* ra = getGPR(code.ra);
    llvm::Value* rb = getGPR(code.rb);
    llvm::Value* rd;

    if (code.oe) {
        // TODO: XER OV update
    } else {
        rd = builder.CreateAdd(ra, rb);
    }

    if (code.rc) {
        // TODO: CR0 update
    }

    setGPR(code.rd, rd);
}

void Recompiler::addcx(Instruction code)
{
    llvm::Value* ra = getGPR(code.ra);
    llvm::Value* rb = getGPR(code.rb);
    llvm::Value* rd;

    // TODO: ?

    setGPR(code.rd, rd);
}

void Recompiler::addex(Instruction code)
{
    llvm::Value* ra = getGPR(code.ra);
    llvm::Value* rb = getGPR(code.rb);
    llvm::Value* rd;

    // TODO: ?

    setGPR(code.rd, rd);
}

void Recompiler::addi(Instruction code)
{
    llvm::Value* ra = getGPR(code.ra);
    llvm::Value* simm = builder.getInt64(code.simm);
    llvm::Value* rd;

    if (code.ra) {
        rd = builder.CreateAdd(ra, simm);
    } else {
        rd = simm;
    }
    
    setGPR(code.rd, rd);
}

void Recompiler::addic(Instruction code)
{
    llvm::Value* ra = getGPR(code.ra);
    llvm::Value* simm = builder.getInt64(code.simm);
    llvm::Value* rd;

    rd = builder.CreateAdd(ra, simm);
    // TODO: XER CA update

    setGPR(code.rd, rd);
}

void Recompiler::addic_(Instruction code)
{
    llvm::Value* ra = getGPR(code.ra);
    llvm::Value* simm = builder.getInt64(code.simm);
    llvm::Value* rd;

    rd = builder.CreateAdd(ra, simm);
    // TODO: XER CA update
    // TODO: CR0 update

    setGPR(code.rd, rd);
}

void Recompiler::addis(Instruction code)
{
    llvm::Value* ra = getGPR(code.ra);
    llvm::Value* simm = builder.getInt64(code.simm << 16);
    llvm::Value* rd;

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
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* rb = getGPR(code.rb);
    llvm::Value* ra;

    ra = builder.CreateAnd(rs, rb);
    if (code.rc) {
        // TODO: CR0 update
    }

    setGPR(code.ra, ra);
}

void Recompiler::andcx(Instruction code)
{
}

void Recompiler::andi_(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* ra;

    ra = builder.CreateAnd(rs, code.uimm);
    // TODO: CR0 update

    setGPR(code.ra, ra);
}

void Recompiler::andis_(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* ra;

    ra = builder.CreateAnd(rs, code.uimm << 16);
    // TODO: CR0 update

    setGPR(code.ra, ra);
}

void Recompiler::cmp(Instruction code)
{
}

void Recompiler::cmpi(Instruction code)
{
}

void Recompiler::cmpl(Instruction code)
{
}

void Recompiler::cmpli(Instruction code)
{
}

void Recompiler::divdx(Instruction code)
{
}

void Recompiler::cntlzdx(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* ra;

    llvm::Function* ctlz = getIntrinsic64(llvm::Intrinsic::ctlz);
    // ra = builder.CreateCall2(ctlz, rs, builder.getInt1(false)); // TODO: For some reason llvm::Function* isn't accepted
    if (code.rc) {
        // TODO: CR0 update
    }

    setGPR(code.ra, ra);
}

void Recompiler::cntlzwx(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* ra;

    llvm::Function* ctlz = getIntrinsic32(llvm::Intrinsic::ctlz);
    // ra = builder.CreateCall2(ctlz, rs, builder.getInt1(false)); // TODO: For some reason llvm::Function* isn't accepted
    if (code.rc) {
        // TODO: CR0 update
    }

    setGPR(code.ra, ra);
}

void Recompiler::divdux(Instruction code)
{
}

void Recompiler::divwx(Instruction code)
{
}

void Recompiler::divwux(Instruction code)
{
}

void Recompiler::eqvx(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* rb = getGPR(code.rb);
    llvm::Value* ra;

    ra = builder.CreateXor(rs, rb);
    ra = builder.CreateNeg(ra);
    if (code.rc) {
        // TODO: CR0 update
    }

    setGPR(code.ra, ra);
}

void Recompiler::extsbx(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* ra;

    // TODO: ?
    if (code.rc) {
        // TODO: CR0 update
    }

    setGPR(code.ra, ra);
}

void Recompiler::extshx(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* ra;

    // TODO: ?
    if (code.rc) {
        // TODO: CR0 update
    }

    setGPR(code.ra, ra);
}

void Recompiler::extswx(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* ra;

    // TODO: ?
    if (code.rc) {
        // TODO: CR0 update
    }

    setGPR(code.ra, ra);
}

void Recompiler::mulhdx(Instruction code)
{
}

void Recompiler::mulhdux(Instruction code)
{
}

void Recompiler::mulhwx(Instruction code)
{
}

void Recompiler::mulhwux(Instruction code)
{
}

void Recompiler::mulldx(Instruction code)
{
}

void Recompiler::mulli(Instruction code)
{
}

void Recompiler::mullwx(Instruction code)
{
}

void Recompiler::nandx(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* rb = getGPR(code.rb);
    llvm::Value* ra;

    ra = builder.CreateAnd(rs, rb);
    ra = builder.CreateNeg(ra);
    if (code.rc) {
        // TODO: CR0 update
    }

    setGPR(code.ra, ra);
}

void Recompiler::negx(Instruction code)
{
    llvm::Value* ra = getGPR(code.ra);
    llvm::Value* rd;

    if (code.oe) {
        // TODO: XER OV update
    } else {
        rd = builder.CreateSub(builder.getInt64(0), ra);
    }

    if (code.rc) {
        // TODO: CR0 update
    }

    setGPR(code.rd, rd);
}

void Recompiler::norx(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* rb = getGPR(code.rb);
    llvm::Value* ra;

    ra = builder.CreateOr(rs, rb);
    ra = builder.CreateNeg(ra);
    if (code.rc) {
        // TODO: CR0 update
    }

    setGPR(code.ra, ra);
}

void Recompiler::orx(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* rb = getGPR(code.rb);
    llvm::Value* ra;

    ra = builder.CreateOr(rs, rb);
    if (code.rc) {
        // TODO: CR0 update
    }

    setGPR(code.ra, ra);
}

void Recompiler::orcx(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* rb = getGPR(code.rb);
    llvm::Value* ra;

    rb = builder.CreateNot(rb);
    ra = builder.CreateOr(rs, rb);
    if (code.rc) {
        // TODO: CR0 update
    }

    setGPR(code.ra, ra);
}

void Recompiler::ori(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* ra;

    ra = builder.CreateOr(rs, code.uimm);

    setGPR(code.ra, ra);
}

void Recompiler::oris(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* ra;

    ra = builder.CreateOr(rs, code.uimm << 16);

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
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* rb = getGPR(code.rb);
    llvm::Value* ra;

    ra = builder.CreateXor(rs, rb);
    if (code.rc) {
        // TODO: CR0 update
    }

    setGPR(code.ra, ra);
}

void Recompiler::xori(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* ra;

    ra = builder.CreateXor(rs, code.uimm);

    setGPR(code.ra, ra);
}

void Recompiler::xoris(Instruction code)
{
    llvm::Value* rs = getGPR(code.rs);
    llvm::Value* ra;

    ra = builder.CreateXor(rs, code.uimm << 16);

    setGPR(code.ra, ra);
}

}  // namespace ppu
}  // namespace cpu
