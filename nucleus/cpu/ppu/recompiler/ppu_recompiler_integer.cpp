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
        // TODO: ?
    } else {
        builder.CreateAdd(ra, rb);
    }

    if (code.rc) {
        // TODO: Do something generic
    }

    setGPR(code.rd, rd);
}

void Recompiler::addcx(Instruction code)
{
}

void Recompiler::addex(Instruction code)
{
}

void Recompiler::addi(Instruction code)
{
}

void Recompiler::addic(Instruction code)
{
}

void Recompiler::addic_(Instruction code)
{
}

void Recompiler::addis(Instruction code)
{
}

void Recompiler::addmex(Instruction code)
{
}

void Recompiler::addzex(Instruction code)
{
}

void Recompiler::andx(Instruction code)
{
}

void Recompiler::andcx(Instruction code)
{
}

void Recompiler::andi_(Instruction code)
{
}

void Recompiler::andis_(Instruction code)
{
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
}

void Recompiler::cntlzwx(Instruction code)
{
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
}

void Recompiler::extsbx(Instruction code)
{
}

void Recompiler::extshx(Instruction code)
{
}

void Recompiler::extswx(Instruction code)
{
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
}

void Recompiler::negx(Instruction code)
{
}

void Recompiler::norx(Instruction code)
{
}

void Recompiler::orx(Instruction code)
{
}

void Recompiler::orcx(Instruction code)
{
}

void Recompiler::ori(Instruction code)
{
}

void Recompiler::oris(Instruction code)
{
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
}

void Recompiler::xori(Instruction code)
{
}

void Recompiler::xoris(Instruction code)
{
}

}  // namespace ppu
}  // namespace cpu
