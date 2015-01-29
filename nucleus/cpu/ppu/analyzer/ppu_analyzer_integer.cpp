/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_analyzer.h"

namespace cpu {
namespace ppu {

/**
 * PPC64 Instructions:
 *  - UISA: Integer instructions (Section: 4.2.1)
 */

void Analyzer::addx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::addcx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(xer, REG_WRITE); // XER CA

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::addex(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(xer, REG_READ); // XER CA
    setFlag(xer, REG_WRITE); // XER CA

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::addi(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::addic(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(xer, REG_WRITE); // XER CA
}

void Analyzer::addic_(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(xer, REG_WRITE); // XER CA
    setFlag(cr[0], REG_WRITE);
}

void Analyzer::addis(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::addmex(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(xer, REG_READ); // XER CA
    setFlag(xer, REG_WRITE); // XER CA

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::addzex(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(xer, REG_READ); // XER CA
    setFlag(xer, REG_WRITE); // XER CA

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::andx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::andcx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::andi_(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
    setFlag(cr[0], REG_WRITE);
}

void Analyzer::andis_(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
    setFlag(cr[0], REG_WRITE);
}

void Analyzer::cmp(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(cr[code.crfd], REG_WRITE);
}

void Analyzer::cmpi(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(cr[code.crfd], REG_WRITE);
}

void Analyzer::cmpl(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(cr[code.crfd], REG_WRITE);
}

void Analyzer::cmpli(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(cr[code.crfd], REG_WRITE);
}

void Analyzer::divdx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::cntlzdx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::cntlzwx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::divdux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::divwx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::divwux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::eqvx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::extsbx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::extshx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::extswx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::mulhdx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::mulhdux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::mulhwx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::mulhwux(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::mulldx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::mulli(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
}

void Analyzer::mullwx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::nandx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::negx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::norx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::orx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::orcx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::ori(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::oris(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::rldc_lr(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::rldicx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::rldiclx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::rldicrx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::rldimix(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::rlwimix(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::rlwinmx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::rlwnmx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::sldx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::slwx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::sradx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
    setFlag(xer, REG_WRITE); // XER CA

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::sradix(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
    setFlag(xer, REG_WRITE); // XER CA

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::srawx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
    setFlag(xer, REG_WRITE); // XER CA

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::srawix(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
    setFlag(xer, REG_WRITE); // XER CA

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::srdx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::srwx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::subfx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::subfcx(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(xer, REG_WRITE); // XER CA

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::subfex(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(xer, REG_READ); // XER CA
    setFlag(xer, REG_WRITE); // XER CA

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::subfic(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(xer, REG_WRITE); // XER CA
}

void Analyzer::subfmex(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(xer, REG_READ); // XER CA
    setFlag(xer, REG_WRITE); // XER CA

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::subfzex(Instruction code)
{
    setFlag(gpr[code.ra], REG_READ);
    setFlag(gpr[code.rd], REG_WRITE);
    setFlag(xer, REG_READ); // XER CA
    setFlag(xer, REG_WRITE); // XER CA

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
    if (code.oe) {
        setFlag(xer, REG_WRITE); // XER OV
    }
}

void Analyzer::xorx(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.rb], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);

    if (code.rc) {
        setFlag(cr[0], REG_WRITE);
    }
}

void Analyzer::xori(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

void Analyzer::xoris(Instruction code)
{
    setFlag(gpr[code.rs], REG_READ);
    setFlag(gpr[code.ra], REG_WRITE);
}

}  // namespace ppu
}  // namespace cpu
