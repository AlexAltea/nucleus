/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_disasm.h"
#include "nucleus/common.h"
#include "nucleus/format.h"
#include "nucleus/cpu/ppu/ppu_tables.h"

// Render the disassembled instruction given it's name and operands
template<typename... Args>
std::string dis_global(const char* name, const char* operandsFormat, Args... fields)
{
    const std::string& operands = format(operandsFormat, std::forward<Args>(fields)...);
    return format("%-8s %s", name, operands.c_str());
}

// Call tables
std::string dis_table4  (PPUFields code) { return s_table4[code.op4].disassembler(code); }
std::string dis_table4_ (PPUFields code) { return s_table4_[code.op4_].disassembler(code); }
std::string dis_table19 (PPUFields code) { return s_table19[code.op19].disassembler(code); }
std::string dis_table30 (PPUFields code) { return s_table30[code.op30].disassembler(code); }
std::string dis_table31 (PPUFields code) { return s_table31[code.op31].disassembler(code); }
std::string dis_table58 (PPUFields code) { return s_table58[code.op58].disassembler(code); }
std::string dis_table59 (PPUFields code) { return s_table59[code.op59].disassembler(code); }
std::string dis_table62 (PPUFields code) { return s_table62[code.op62].disassembler(code); }
std::string dis_table63 (PPUFields code) { return s_table63[code.op63].disassembler(code); }
std::string dis_table63_(PPUFields code) { return s_table63_[code.op63_].disassembler(code); }

/**
 * PPU Instructions
 */
std::string dis_addx(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("add", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("add.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("addo", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else {
        return dis_global("addo.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_addcx(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("addc", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("addc.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("addco", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else {
        return dis_global("addco.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_addex(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("adde", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("adde.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("addeo", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else {
        return dis_global("addeo.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_addi(PPUFields code)
{
    if (code.ra == 0) {
        return dis_global("li", "r%d, 0x%X", code.rd, code.simm);
    }
    else if (code.simm < 0) {
        return dis_global("addi", "r%d, r%d, -0x%X", code.rd, code.ra, -code.simm);
    }
    else {
        return dis_global("addi", "r%d, r%d, 0x%X", code.rd, code.ra, code.simm);
    }
}

std::string dis_addic(PPUFields code)
{
    if (code.simm < 0) {
        return dis_global("subic", "r%d, r%d, 0x%X", code.rd, code.ra, code.simm);
    } else {
        return dis_global("addic", "r%d, r%d, 0x%X", code.rd, code.ra, code.simm);
    }
}

std::string dis_addic_(PPUFields code)
{
    if (code.simm < 0) {
        return dis_global("subic.", "r%d, r%d, 0x%X", code.rd, code.ra, code.simm);
    } else {
        return dis_global("addic.", "r%d, r%d, 0x%X", code.rd, code.ra, code.simm);
    }
}

std::string dis_addis(PPUFields code)
{
    if (code.ra == 0) {
        if (code.simm < 0) {
            return dis_global("lis", "r%d, -0x%X", code.rd, -code.simm);
        }
        else {
            return dis_global("lis", "r%d, 0x%X", code.rd, code.simm);
        }
    }
    else if (code.simm < 0) {
        return dis_global("subis", "r%d, r%d, 0x%X", code.rd, code.ra, code.simm);
    }
    else {
        return dis_global("addis", "r%d, r%d, 0x%X", code.rd, code.ra, code.simm);
    }
}

std::string dis_addmex(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("addme", "r%d, r%d", code.rd, code.ra);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("addme.", "r%d, r%d", code.rd, code.ra);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("addmeo", "r%d, r%d", code.rd, code.ra);
    }
    else {
        return dis_global("addmeo.", "r%d, r%d", code.rd, code.ra);
    }
}

std::string dis_addzex(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("addze", "r%d, r%d", code.rd, code.ra);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("addze.", "r%d, r%d", code.rd, code.ra);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("addzeo", "r%d, r%d", code.rd, code.ra);
    }
    else {
        return dis_global("addzeo.", "r%d, r%d", code.rd, code.ra);
    }
}

std::string dis_andx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("and", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    } else {
        return dis_global("and.", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    }
}

std::string dis_andcx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("andc", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    } else {
        return dis_global("andc.", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    }
}

std::string dis_andi_(PPUFields code)
{
    return dis_global("andi.", "r%d, r%d, 0x%X", code.ra, code.rs, code.uimm);
}

std::string dis_andis_(PPUFields code)
{
    return dis_global("andis.", "r%d, r%d, 0x%X", code.ra, code.rs, code.uimm);
}

std::string dis_bx(PPUFields code)
{
    if (code.aa == 0 && code.lk == 0) {
        return dis_global("b", "0x%X", code.li * 4);
    }
    else if (code.aa == 1 && code.lk == 0) {
        return dis_global("ba", "0x%X", code.li * 4);
    }
    else if (code.aa == 0 && code.lk == 1) {
        return dis_global("bl", "0x%X", code.li * 4);
    }
    else {
        return dis_global("bla", "0x%X", code.li * 4);
    }
}

std::string dis_bcx(PPUFields code)
{
    if (code.aa == 0 && code.lk == 0) {
        std::string instr = "";
        switch (code.bi % 4) {
        case 0: instr = (code.bo & 0x08) ? "blt" : "bge";
        case 1: instr = (code.bo & 0x08) ? "bgt" : "ble";
        case 2: instr = (code.bo & 0x08) ? "beq" : "bne";
        }
        instr += (code.bo & 0x02) ? ((code.bo & 0x01) ? "+" : "-") : "";

        if (code.bd < 0) {
            return dis_global(instr.c_str(), "cr%d, -0x%X", code.bi / 4, -code.bd);
        }
        else {
            return dis_global(instr.c_str(), "cr%d, 0x%X", code.bi / 4, code.bd);
        }
    }
    else if (code.aa == 1 && code.lk == 0) {
        return dis_global("bca", "0x%X, 0x%X, 0x%X", code.bo, code.bi, code.bd * 4);
    }
    else if (code.aa == 0 && code.lk == 1) {
        return dis_global("bcl", "0x%X, 0x%X, 0x%X", code.bo, code.bi, code.bd * 4);
    }
    else {
        return dis_global("bcla", "0x%X, 0x%X, 0x%X", code.bo, code.bi, code.bd * 4);
    }
}

std::string dis_bcctrx(PPUFields code)
{
    if (code.lk == 0) {
        return dis_global("bcctr", "0x%X, 0x%X, 0x%X", code.bo, code.bi, code.bh);
    } else {
        return dis_global("bcctrl", "0x%X, 0x%X, 0x%X", code.bo, code.bi, code.bh);
    }
}

std::string dis_bclrx(PPUFields code)
{
    if (code.lk == 0) {
        return dis_global("bclr", "0x%X, 0x%X, 0x%X", code.bo, code.bi, code.bh);
    } else {
        return dis_global("bclrl", "0x%X, 0x%X, 0x%X", code.bo, code.bi, code.bh);
    }
}

std::string dis_cmp(PPUFields code)
{
    if (code.l10 == 1) {
        return dis_global("cmpd", "cr%d, r%d, r%d", code.crfd, code.ra, code.rb);
    } else {
        return dis_global("cmpw", "cr%d, r%d, r%d", code.crfd, code.ra, code.rb);
    }
}

std::string dis_cmpi(PPUFields code)
{
    if (code.l10 == 1) {
        return dis_global("cmpdi", "cr%d, r%d, 0x%X", code.crfd, code.ra, code.simm);
    } else {
        return dis_global("cmpwi", "cr%d, r%d, 0x%X", code.crfd, code.ra, code.simm);
    }
}

std::string dis_cmpl(PPUFields code)
{
    if (code.l10 == 1) {
        return dis_global("cmpld", "cr%d, r%d, r%d", code.crfd, code.ra, code.rb);
    } else {
        return dis_global("cmplw", "cr%d, r%d, r%d", code.crfd, code.ra, code.rb);
    }
}

std::string dis_cmpli(PPUFields code)
{
    if (code.l10 == 1) {
        return dis_global("cmpldi", "cr%d, r%d, 0x%X", code.crfd, code.ra, code.simm);
    } else {
        return dis_global("cmplwi", "cr%d, r%d, 0x%X", code.crfd, code.ra, code.simm);
    }
}

std::string dis_cntlzdx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("cntlzd", "r%d, r%d", code.ra, code.rs);
    } else {
        return dis_global("cntlzd.", "r%d, r%d", code.ra, code.rs);
    }
}

std::string dis_cntlzwx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("cntlzw", "r%d, r%d", code.ra, code.rs);
    } else {
        return dis_global("cntlzw.", "r%d, r%d", code.ra, code.rs);
    }
}

std::string dis_crand(PPUFields code)
{
    return dis_global("crand", "cr%d, cr%d, cr%d", code.crbd, code.crba, code.crbb);
}

std::string dis_crandc(PPUFields code)
{
    return dis_global("crandc", "cr%d, cr%d, cr%d", code.crbd, code.crba, code.crbb);
}

std::string dis_creqv(PPUFields code)
{
    return dis_global("creqv", "cr%d, cr%d, cr%d", code.crbd, code.crba, code.crbb);
}

std::string dis_crnand(PPUFields code)
{
    return dis_global("crnand", "cr%d, cr%d, cr%d", code.crbd, code.crba, code.crbb);
}

std::string dis_crnor(PPUFields code)
{
    return dis_global("crnor", "cr%d, cr%d, cr%d", code.crbd, code.crba, code.crbb);
}

std::string dis_cror(PPUFields code)
{
    return dis_global("cror", "cr%d, cr%d, cr%d", code.crbd, code.crba, code.crbb);
}

std::string dis_crorc(PPUFields code)
{
    return dis_global("cror", "cr%d, cr%d, cr%d", code.crbd, code.crba, code.crbb);
}

std::string dis_crxor(PPUFields code)
{
    return dis_global("crxor", "cr%d, cr%d, cr%d", code.crbd, code.crba, code.crbb);
}

std::string dis_dcbf(PPUFields code)
{
    return dis_global("dcbf", "r%d, r%d", code.ra, code.rb);
}

std::string dis_dcbst(PPUFields code)
{
    return dis_global("dcbst", "r%d, r%d", code.ra, code.rb);
}

std::string dis_dcbt(PPUFields code)
{
    return dis_global("dcbt", "r%d, r%d, 0x%X", code.ra, code.rb, code.th);
}

std::string dis_dcbtst(PPUFields code)
{
    return dis_global("dcbtst", "r%d, r%d", code.ra, code.rb);
}

std::string dis_dcbz(PPUFields code)
{
    return dis_global("dcbz", "r%d, r%d", code.ra, code.rb);
}

std::string dis_divdx(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("divd", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("divd.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("divdo", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else {
        return dis_global("divdo.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_divdux(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("divdu", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("divdu.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("divduo", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else {
        return dis_global("divduo.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_divwx(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("divw", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("divw.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("divwo", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else {
        return dis_global("divwo.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_divwux(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("divwu", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("divwu.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("divwuo", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else {
        return dis_global("divwuo.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_eciwx(PPUFields code)
{
    return dis_global("eciwx", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_ecowx(PPUFields code)
{
    return dis_global("ecowx", "r%d, r%d, r%d", code.rs, code.ra, code.rb);
}

std::string dis_eieio(PPUFields code)
{
    return dis_global("eieio", "");
}

std::string dis_eqvx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("eqv", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    } else {
        return dis_global("eqv.", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    }
}

std::string dis_extsbx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("extsb", "r%d, r%d", code.ra, code.rs);
    } else {
        return dis_global("extsb.", "r%d, r%d", code.ra, code.rs);
    }
}

std::string dis_extshx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("extsh", "r%d, r%d", code.ra, code.rs);
    } else {
        return dis_global("extsh.", "r%d, r%d", code.ra, code.rs);
    }
}

std::string dis_extswx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("extsw", "r%d, r%d", code.ra, code.rs);
    } else {
        return dis_global("extsw.", "r%d, r%d", code.ra, code.rs);
    }
}

// Floating-Point

std::string dis_icbi(PPUFields code)
{
    return dis_global("icbi", "r%d, r%d", code.ra, code.rb);
}

std::string dis_isync(PPUFields code)
{
    return dis_global("isync", "");
}

std::string dis_lbz(PPUFields code)
{
    return dis_global("lbz", "r%d, 0x%X(r%d)", code.rd, code.d, code.ra);
}

std::string dis_lbzu(PPUFields code)
{
    return dis_global("lbzu", "r%d, 0x%X(r%d)", code.rd, code.d, code.ra);
}

std::string dis_lbzux(PPUFields code)
{
    return dis_global("lbzux", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_lbzx(PPUFields code)
{
    return dis_global("lbzx", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_ld(PPUFields code)
{
    if (code.ds < 0) {
        return dis_global("ld", "r%d, -0x%X(r%d)", code.rd, -code.ds * 4, code.ra);
    } else {
        return dis_global("ld", "r%d, 0x%X(r%d)", code.rd, code.ds * 4, code.ra);
    }
}

std::string dis_ldarx(PPUFields code)
{
    return dis_global("ldarx", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_ldu(PPUFields code)
{
    return dis_global("ldu", "r%d, 0x%X(r%d)", code.rd, code.ds, code.ra);
}

std::string dis_ldux(PPUFields code)
{
    return dis_global("ldux", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_ldx(PPUFields code)
{
    return dis_global("ldx", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

// Floating-Point

std::string dis_lha(PPUFields code)
{
    return dis_global("lha", "r%d, 0x%X(r%d)", code.rd, code.d, code.ra);
}

std::string dis_lhau(PPUFields code)
{
    return dis_global("lhau", "r%d, 0x%X(r%d)", code.rd, code.d, code.ra);
}

std::string dis_lhaux(PPUFields code)
{
    return dis_global("lhaux", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_lhax(PPUFields code)
{
    return dis_global("lhax", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_lhbrx(PPUFields code)
{
    return dis_global("lhbrx", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_lhz(PPUFields code)
{
    return dis_global("lhz", "r%d, 0x%X(r%d)", code.rd, code.d, code.ra);
}

std::string dis_lhzu(PPUFields code)
{
    return dis_global("lhzu", "r%d, 0x%X(r%d)", code.rd, code.d, code.ra);
}

std::string dis_lhzux(PPUFields code)
{
    return dis_global("lhzux", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_lhzx(PPUFields code)
{
    return dis_global("lhzx", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_lmw(PPUFields code)
{
    return dis_global("lmw", "r%d, 0x%X(r%d)", code.rd, code.d, code.ra);
}

std::string dis_lswi(PPUFields code)
{
    return dis_global("lswi", "r%d, r%d, 0x%X", code.rd, code.ra, code.nb);
}

std::string dis_lswx(PPUFields code)
{
    return dis_global("lswx", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_lwa(PPUFields code)
{
    return dis_global("lwa", "r%d, 0x%X(r%d)", code.rd, code.ds, code.ra);
}

std::string dis_lwarx(PPUFields code)
{
    return dis_global("lwarx", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_lwaux(PPUFields code)
{
    return dis_global("lwaux", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_lwax(PPUFields code)
{
    return dis_global("lwax", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_lwbrx(PPUFields code)
{
    return dis_global("lwbrx", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_lwz(PPUFields code)
{
    if (code.d >= 0) {
        return dis_global("lwz", "r%d, 0x%X(r%d)", code.rd, code.d, code.ra);
    } else {
        return dis_global("lwz", "r%d, -0x%X(r%d)", code.rd, -code.d, code.ra);
    }
}

std::string dis_lwzu(PPUFields code)
{
    return dis_global("lwzu", "r%d, 0x%X(r%d)", code.rd, code.d, code.ra);
}

std::string dis_lwzux(PPUFields code)
{
    return dis_global("lwzux", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_lwzx(PPUFields code)
{
    return dis_global("lwzx", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
}

std::string dis_mcrf(PPUFields code)
{
    return dis_global("mcrf", "cr%d, cr%d", code.crfd, code.crfs);
}

std::string dis_mfcr(PPUFields code)
{
    return dis_global("mfcr", "r%d", code.rd);
}

std::string dis_mfocrf(PPUFields code)
{
    return dis_global("mfocrf", "r%d, 0x%X", code.rd, code.crm);
}

// mffsx to mtsrin

std::string dis_mfspr(PPUFields code)
{
    switch ((code.spr >> 5) | ((code.spr & 0x1F) << 5)) {
    case 1:  return dis_global("mfspr", "r%d, xer", code.rd);
    case 8:  return dis_global("mfspr", "r%d, lr", code.rd);
    case 9:  return dis_global("mfspr", "r%d, ctr", code.rd);
    default: return dis_global("mfspr", "r%d, spr%d", code.rd, code.spr);
    }
}

std::string dis_mftb(PPUFields code)
{
    const u32 tbr = (code.spr >> 5) | ((code.spr & 0x1F) << 5);
    return dis_global("mftb", "r%d, %d", code.rd, tbr);
}

std::string dis_mtocrf(PPUFields code)
{
    if (code.l11 == 0 && code.crm == 0xFF) {
        return dis_global("mtcr", "r%d", code.rs);
    } else if (code.l11 == 0) {
        return dis_global("mtcrf", "0x%X, r%d", code.crm, code.rs);
    } else {
        return dis_global("mtocrf", "0x%X, r%d", code.crm, code.rs);
    }
}

std::string dis_mtspr(PPUFields code)
{
    switch ((code.spr >> 5) | ((code.spr & 0x1F) << 5)) {
    case 1:  return dis_global("mtspr", "xer, r%d", code.rs);
    case 8:  return dis_global("mtspr", "lr, r%d", code.rs);
    case 9:  return dis_global("mtspr", "ctr, r%d", code.rs);
    default: return dis_global("mtspr", "spr%d, r%d", code.spr, code.rs);
    }
}

std::string dis_mulhdx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("mulhd", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    } else {
        return dis_global("mulhd.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_mulhdux(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("mulhdu", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    } else {
        return dis_global("mulhdu.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_mulhwx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("mulhw", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    } else {
        return dis_global("mulhw.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_mulhwux(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("mulhwu", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    } else {
        return dis_global("mulhwu.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_mulldx(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("mulld", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("mulld.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("mulldo", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else {
        return dis_global("mulldo.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_mulli(PPUFields code)
{
    return dis_global("mulli", "r%d, r%d, 0x%X", code.rd, code.ra, code.simm);
}

std::string dis_mullwx(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("mullw", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("mullw.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("mullwo", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else {
        return dis_global("mullwo.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_nandx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("nand", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    } else {
        return dis_global("nand.", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    }
}

std::string dis_negx(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("neg", "r%d, r%d", code.rd, code.ra);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("neg.", "r%d, r%d", code.rd, code.ra);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("nego", "r%d, r%d", code.rd, code.ra);
    }
    else {
        return dis_global("nego.", "r%d, r%d", code.rd, code.ra);
    }
}

std::string dis_norx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("nor", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    } else {
        return dis_global("nor.", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    }
}

std::string dis_orx(PPUFields code)
{
    if (code.rc == 0 && code.rs == code.rb) {
        return dis_global("mr", "r%d, r%d", code.ra, code.rs);
    }
    else if (code.rc == 0) {
        return dis_global("or", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    }
    else {
        return dis_global("or.", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    }
}

std::string dis_orcx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("orc", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    } else {
        return dis_global("orc.", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    }
}

std::string dis_ori(PPUFields code)
{
    if (code.ra == 0 && code.rs == 0 && code.uimm == 0) {
        return dis_global("nop", "");
    } else {
        return dis_global("ori", "r%d, r%d, 0x%X", code.ra, code.rs, code.uimm);
    }
}

std::string dis_oris(PPUFields code)
{
    return dis_global("oris", "r%d, r%d, 0x%X", code.ra, code.rs, code.uimm);
}

std::string dis_rfid(PPUFields code)
{
    return dis_global("rfid", "");
}

std::string dis_rldclx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("rldclx", "r%d, r%d, r%d, 0x%X", code.ra, code.rs, code.rb, code.me);
    } else {
        return dis_global("rldclx.", "r%d, r%d, r%d, 0x%X", code.ra, code.rs, code.rb, code.me);
    }
}

std::string dis_rldcrx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("rldcrx", "r%d, r%d, r%d, 0x%X", code.ra, code.rs, code.rb, code.me);
    } else {
        return dis_global("rldcrx.", "r%d, r%d, r%d, 0x%X", code.ra, code.rs, code.rb, code.me);
    }
}

std::string dis_rldicx(PPUFields code)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 mb = code.mb | (code.mb_ << 5);
    if (code.rc == 0) {
        return dis_global("rldic", "r%d, r%d, 0x%X, 0x%X", code.ra, code.rs, sh, mb);
    } else {
        return dis_global("rldic.", "r%d, r%d, 0x%X, 0x%X", code.ra, code.rs, sh, mb);
    }
}

std::string dis_rldiclx(PPUFields code)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 mb = code.mb | (code.mb_ << 5);
    if (code.rc == 0) {
        if (sh == 0) {
            return dis_global("clrldi", "r%d, r%d, %d", code.ra, code.rs, mb);
        }
        else {
            return dis_global("rldicl", "r%d, r%d, %d, %d", code.ra, code.rs, sh, mb);
        }

    } else {
        return dis_global("rldicl.", "r%d, r%d, 0x%X, 0x%X", code.ra, code.rs, sh, mb);
    }
}

std::string dis_rldicrx(PPUFields code)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 me = code.me_ | (code.me__ << 5);
    if (code.rc == 0) {
        return dis_global("rldicr", "r%d, r%d, 0x%X, 0x%X", code.ra, code.rs, sh, me);
    } else {
        return dis_global("rldicr.", "r%d, r%d, 0x%X, 0x%X", code.ra, code.rs, sh, me);
    }
}

std::string dis_rldimix(PPUFields code)
{
    const u32 sh = code.sh | (code.sh_ << 5);
    const u32 mb = code.mb | (code.mb_ << 5);
    if (code.rc == 0) {
        return dis_global("rldimi", "r%d, r%d, 0x%X, 0x%X", code.ra, code.rs, sh, mb);
    } else {
        return dis_global("rldimi.", "r%d, r%d, 0x%X, 0x%X", code.ra, code.rs, sh, mb);
    }
}

std::string dis_rlwimix(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("rlwimi", "r%d, r%d, 0x%X, 0x%X, 0x%X", code.ra, code.rs, code.sh, code.mb, code.me);
    } else {
        return dis_global("rlwimi.", "r%d, r%d, 0x%X, 0x%X, 0x%X", code.ra, code.rs, code.sh, code.mb, code.me);
    }
}

std::string dis_rlwinmx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("rlwinm", "r%d, r%d, 0x%X, 0x%X, 0x%X", code.ra, code.rs, code.sh, code.mb, code.me);
    } else {
        return dis_global("rlwinm.", "r%d, r%d, 0x%X, 0x%X, 0x%X", code.ra, code.rs, code.sh, code.mb, code.me);
    }
}

std::string dis_rlwnmx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("rlwnm", "r%d, r%d, r%d, 0x%X, 0x%X", code.ra, code.rs, code.rb, code.mb, code.me);
    } else {
        return dis_global("rlwnm.", "r%d, r%d, r%d, 0x%X, 0x%X", code.ra, code.rs, code.rb, code.mb, code.me);
    }
}

std::string dis_sc(PPUFields code)
{
    return dis_global("sc", "");
}

std::string dis_slbia(PPUFields code)
{
    return dis_global("slbia", "");
}

std::string dis_slbie(PPUFields code)
{
    return dis_global("slbie", "r%d", code.rb);
}

// slbmfee - slbmfev

std::string dis_sldx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("sld", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    } else {
        return dis_global("sld.", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    }
}

std::string dis_slwx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("slw", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    } else {
        return dis_global("slw.", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    }
}

std::string dis_sradx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("srad", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    } else {
        return dis_global("srad.", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    }
}

std::string dis_sradix(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("sradi", "r%d, r%d, 0x%X", code.ra, code.rs, code.sh);
    } else {
        return dis_global("sradi.", "r%d, r%d, 0x%X", code.ra, code.rs, code.sh);
    }
}

std::string dis_srawx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("sraw", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    } else {
        return dis_global("sraw.", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    }
}

std::string dis_srawix(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("srawi", "r%d, r%d, 0x%X", code.ra, code.rs, code.sh);
    } else {
        return dis_global("srawi.", "r%d, r%d, 0x%X", code.ra, code.rs, code.sh);
    }
}

std::string dis_srdx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("srd", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    } else {
        return dis_global("srd.", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    }
}

std::string dis_srwx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("srw", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    } else {
        return dis_global("srw.", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    }
}

std::string dis_stb(PPUFields code)
{
    return dis_global("stb", "r%d, 0x%X(r%d)", code.rs, code.d, code.ra);
}

std::string dis_stbu(PPUFields code)
{
    return dis_global("stbu", "r%d, 0x%X(r%d)", code.rs, code.d, code.ra);
}

std::string dis_stbux(PPUFields code)
{
    return dis_global("stbux", "r%d, r%d, r%d", code.rs, code.ra, code.rb);
}

std::string dis_stbx(PPUFields code)
{
    return dis_global("stbx", "r%d, r%d, r%d", code.rs, code.ra, code.rb);
}

std::string dis_std(PPUFields code)
{
    return dis_global("std", "r%d, 0x%X(r%d)", code.rs, code.ds * 4, code.ra);
}

std::string dis_stdcx_(PPUFields code)
{
    return dis_global("stdcx.", "r%d, r%d, r%d", code.rs, code.ra, code.rb);
}

std::string dis_stdu(PPUFields code)
{
    if (code.ds >= 0) {
        return dis_global("stdu", "r%d, 0x%X(r%d)", code.rs, code.ds * 4, code.ra);
    } else {
        return dis_global("stdu", "r%d, -0x%X(r%d)", code.rs, -code.ds * 4, code.ra);
    }
}

std::string dis_stdux(PPUFields code)
{
    return dis_global("stdux", "r%d, r%d, r%d", code.rs, code.ra, code.rb);
}

std::string dis_stdx(PPUFields code)
{
    return dis_global("stdx", "r%d, r%d, r%d", code.rs, code.ra, code.rb);
}

// Floating-point

std::string dis_sth(PPUFields code)
{
    return dis_global("sth", "r%d, 0x%X(r%d)", code.rs, code.d, code.ra);
}

std::string dis_sthbrx(PPUFields code)
{
    return dis_global("sthbrx", "r%d, r%d, r%d", code.rs, code.ra, code.rb);
}

std::string dis_sthu(PPUFields code)
{
    return dis_global("sthu", "r%d, 0x%X(r%d)", code.rs, code.d, code.ra);
}

std::string dis_sthux(PPUFields code)
{
    return dis_global("sthux", "r%d, r%d, r%d", code.rs, code.ra, code.rb);
}

std::string dis_sthx(PPUFields code)
{
    return dis_global("sthx", "r%d, r%d, r%d", code.rs, code.ra, code.rb);
}

std::string dis_stmw(PPUFields code)
{
    return dis_global("stmw", "r%d, 0x%X(r%d)", code.rs, code.d, code.ra);
}

std::string dis_stswi(PPUFields code)
{
    return dis_global("stswi", "r%d, r%d, 0x%X", code.rs, code.ra, code.nb);
}

std::string dis_stswx(PPUFields code)
{
    return dis_global("stswx", "r%d, r%d, r%d", code.rs, code.ra, code.rb);
}

std::string dis_stw(PPUFields code)
{
    return dis_global("stw", "r%d, 0x%X(r%d)", code.rs, code.d, code.ra);
}

std::string dis_stwbrx(PPUFields code)
{
    return dis_global("stwbrx", "r%d, r%d, r%d", code.rs, code.ra, code.rb);
}

std::string dis_stwcx_(PPUFields code)
{
    return dis_global("stwcx.", "r%d, r%d, r%d", code.rs, code.ra, code.rb);
}

std::string dis_stwu(PPUFields code)
{
    return dis_global("stwu", "r%d, 0x%X(r%d)", code.rs, code.d, code.ra);
}

std::string dis_stwux(PPUFields code)
{
    return dis_global("stwux", "r%d, r%d, r%d", code.rs, code.ra, code.rb);
}

std::string dis_stwx(PPUFields code)
{
    return dis_global("stwx", "r%d, r%d, r%d", code.rs, code.ra, code.rb);
}

std::string dis_subfx(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("subf", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("subf.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("subfo", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else {
        return dis_global("subfo.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_subfcx(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("subfc", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("subfc.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("subfco", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else {
        return dis_global("subfco.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_subfex(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("subfe", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("subfe.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("subfeo", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
    else {
        return dis_global("subfeo.", "r%d, r%d, r%d", code.rd, code.ra, code.rb);
    }
}

std::string dis_subfic(PPUFields code)
{
    return dis_global("subfic", "r%d, r%d, 0x%X", code.rd, code.ra, code.simm);
}

std::string dis_subfmex(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("subfme", "r%d, r%d", code.rd, code.ra);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("subfme.", "r%d, r%d", code.rd, code.ra);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("subfmeo", "r%d, r%d", code.rd, code.ra);
    }
    else {
        return dis_global("subfmeo.", "r%d, r%d", code.rd, code.ra);
    }
}

std::string dis_subfzex(PPUFields code)
{
    if (code.oe == 0 && code.rc == 0) {
        return dis_global("subfze", "r%d, r%d", code.rd, code.ra);
    }
    else if (code.oe == 0 && code.rc == 1) {
        return dis_global("subfze.", "r%d, r%d", code.rd, code.ra);
    }
    else if (code.oe == 1 && code.rc == 0) {
        return dis_global("subfzeo", "r%d, r%d", code.rd, code.ra);
    }
    else {
        return dis_global("subfzeo.", "r%d, r%d", code.rd, code.ra);
    }
}

std::string dis_sync(PPUFields code)
{
    return dis_global("sync", "%d", code.l9_10);
}

std::string dis_td(PPUFields code)
{
    return dis_global("td", "0x%X, r%d, r%d", code.to, code.ra, code.rb);
}

std::string dis_tdi(PPUFields code)
{
    return dis_global("tdi", "0x%X, r%d, 0x%X", code.to, code.ra, code.simm);
}

// tlbia - tlbsync

std::string dis_tw(PPUFields code)
{
    return dis_global("tw", "0x%X, r%d, r%d", code.to, code.ra, code.rb);
}

std::string dis_twi(PPUFields code)
{
    return dis_global("twi", "0x%X, r%d, 0x%X", code.to, code.ra, code.simm);
}

std::string dis_xorx(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("xor", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    } else {
        return dis_global("xor.", "r%d, r%d, r%d", code.ra, code.rs, code.rb);
    }
}

std::string dis_xori(PPUFields code)
{
    return dis_global("xori", "r%d, r%d, 0x%X", code.ra, code.rs, code.uimm);
}

std::string dis_xoris(PPUFields code)
{
    return dis_global("xoris", "r%d, r%d, 0x%X", code.ra, code.rs, code.uimm);
}

/**
 * Floating-Point Unit instructions
 */
std::string dis_fabs(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fabs", "fr%d, fr%d", code.frd, code.frb);
    } else {
        return dis_global("fabs.", "fr%d, fr%d", code.frd, code.frb);
    }
}

std::string dis_fadd(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fadd", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    } else {
        return dis_global("fadd.", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    }
}

std::string dis_fadds(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fadds", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    } else {
        return dis_global("fadds.", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    }
}

std::string dis_fcfid(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fcfid", "fr%d, fr%d", code.frd, code.frb);
    } else {
        return dis_global("fcfid.", "fr%d, fr%d", code.frd, code.frb);
    }
}

std::string dis_fcmpo(PPUFields code)
{
    return dis_global("fcmpo", "crf%d, fr%d, fr%d", code.crfd, code.fra, code.frb);
}

std::string dis_fcmpu(PPUFields code)
{
    return dis_global("fcmpu", "crf%d, fr%d, fr%d", code.crfd, code.fra, code.frb);
}

std::string dis_fctid(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fctid", "fr%d, fr%d", code.frd, code.frb);
    } else {
        return dis_global("fctid.", "fr%d, fr%d", code.frd, code.frb);
    }
}

std::string dis_fctidz(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fctidz", "fr%d, fr%d", code.frd, code.frb);
    } else {
        return dis_global("fctidz.", "fr%d, fr%d", code.frd, code.frb);
    }
}

std::string dis_fctiw(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fctiw", "fr%d, fr%d", code.frd, code.frb);
    } else {
        return dis_global("fctiw.", "fr%d, fr%d", code.frd, code.frb);
    }
}

std::string dis_fctiwz(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fctiwz", "fr%d, fr%d", code.frd, code.frb);
    } else {
        return dis_global("fctiwz.", "fr%d, fr%d", code.frd, code.frb);
    }
}

std::string dis_fdiv(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fdiv", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    } else {
        return dis_global("fdiv.", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    }
}

std::string dis_fdivs(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fdivs", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    } else {
        return dis_global("fdivs.", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    }
}

std::string dis_fmadd(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fmadd", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    } else {
        return dis_global("fmadd.", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    }
}

std::string dis_fmadds(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fmadds", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    } else {
        return dis_global("fmadds.", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    }
}

std::string dis_fmr(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fmr", "fr%d, fr%d", code.frd, code.frb);
    } else {
        return dis_global("fmr.", "fr%d, fr%d", code.frd, code.frb);
    }
}

std::string dis_fmsub(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fmsub", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    } else {
        return dis_global("fmsub.", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    }
}

std::string dis_fmsubs(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fmsubs", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    } else {
        return dis_global("fmsubs.", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    }
}

std::string dis_fmul(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fmul", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    } else {
        return dis_global("fmul.", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    }
}

std::string dis_fmuls(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fmuls", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    } else {
        return dis_global("fmuls.", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    }
}

std::string dis_fnabs(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fnabs", "fr%d, fr%d", code.frd, code.frb);
    } else {
        return dis_global("fnabs.", "fr%d, fr%d", code.frd, code.frb);
    }
}

std::string dis_fneg(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fneg", "fr%d, fr%d", code.frd, code.frb);
    } else {
        return dis_global("fneg.", "fr%d, fr%d", code.frd, code.frb);
    }
}

std::string dis_fnmadd(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fnmadd", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    } else {
        return dis_global("fnmadd.", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    }
}

std::string dis_fnmadds(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fnmadds", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    } else {
        return dis_global("fnmadds.", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    }
}

std::string dis_fnmsub(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fnmsub", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    } else {
        return dis_global("fnmsub.", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    }
}

std::string dis_fnmsubs(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fnmsubs", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    } else {
        return dis_global("fnmsubs.", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    }
}

std::string dis_fres(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fres", "fr%d, fr%d", code.frd, code.frb);
    } else {
        return dis_global("fres.", "fr%d, fr%d", code.frd, code.frb);
    }
}

std::string dis_frsp(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("frsp", "fr%d, fr%d", code.frd, code.frb);
    } else {
        return dis_global("frsp.", "fr%d, fr%d", code.frd, code.frb);
    }
}

std::string dis_frsqrte(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("frsqrte", "fr%d, fr%d", code.frd, code.frb);
    } else {
        return dis_global("frsqrte.", "fr%d, fr%d", code.frd, code.frb);
    }
}

std::string dis_fsel(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fsel", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    } else {
        return dis_global("fsel.", "fr%d, fr%d, fr%d, fr%d", code.frd, code.fra, code.frc, code.frb);
    }
}

std::string dis_fsqrt(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fsqrt", "fr%d, fr%d", code.frd, code.frb);
    } else {
        return dis_global("fsqrt.", "fr%d, fr%d", code.frd, code.frb);
    }
}

std::string dis_fsqrts(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fsqrts", "fr%d, fr%d", code.frd, code.frb);
    } else {
        return dis_global("fsqrts.", "fr%d, fr%d", code.frd, code.frb);
    }
}

std::string dis_fsub(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fsub", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    } else {
        return dis_global("fsub.", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    }
}

std::string dis_fsubs(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("fsubs", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    } else {
        return dis_global("fsubs.", "fr%d, fr%d, fr%d", code.frd, code.fra, code.frb);
    }
}

std::string dis_lfd(PPUFields code)
{
    return dis_global("lfd", "fr%d, 0x%X(r%d)", code.frd, code.d, code.ra);
}

std::string dis_lfdu(PPUFields code)
{
    return dis_global("lfdu", "fr%d, 0x%X(r%d)", code.frd, code.d, code.ra);
}

std::string dis_lfs(PPUFields code)
{
    return dis_global("lfs", "fr%d, 0x%X(r%d)", code.frd, code.d, code.ra);
}

std::string dis_lfsu(PPUFields code)
{
    return dis_global("lfsu", "fr%d, 0x%X(r%d)", code.frd, code.d, code.ra);
}

std::string dis_mcrfs(PPUFields code)
{
    return dis_global("mcrfs", "cr%d, cr%d", code.crfd, code.crfs);
}

std::string dis_mffs(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("mffs", "fr%d", code.frd);
    } else {
        return dis_global("mffs.", "fr%d", code.frd);
    }
}

std::string dis_mtfsb0(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("mtfsb0", "crb%d", code.crbd);
    } else {
        return dis_global("mtfsb0.", "crb%d", code.crbd);
    }
}

std::string dis_mtfsb1(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("mtfsb1", "crb%d", code.crbd);
    } else {
        return dis_global("mtfsb1.", "crb%d", code.crbd);
    }
}

std::string dis_mtfsf(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("mtfsf", "0x%X, fr%d", code.fm, code.frb);
    } else {
        return dis_global("mtfsf.", "0x%X, fr%d", code.fm, code.frb);
    }
}

std::string dis_mtfsfi(PPUFields code)
{
    if (code.rc == 0) {
        return dis_global("mtfsfi", "crf%d, 0x%X", code.crfd, code.imm);
    } else {
        return dis_global("mtfsfi.", "crf%d, 0x%X", code.crfd, code.imm);
    }
}

std::string dis_stfd(PPUFields code)
{
    return dis_global("stfd", "fr%d, 0x%X(r%d)", code.frs, code.d, code.ra);
}

std::string dis_stfdu(PPUFields code)
{
    return dis_global("stfdu", "fr%d, 0x%X(r%d)", code.frs, code.d, code.ra);
}

std::string dis_stfs(PPUFields code)
{
    return dis_global("stfs", "fr%d, 0x%X(r%d)", code.frs, code.d, code.ra);
}

std::string dis_stfsu(PPUFields code)
{
    return dis_global("stfsu", "fr%d, 0x%X(r%d)", code.frs, code.d, code.ra);
}

/**
 * Vector/SIMD instructions
 */
std::string dis_mfvscr(PPUFields code)
{
    return dis_global("mfvscr", "v%d", code.vd);
}

std::string dis_mtvscr(PPUFields code)
{
    return dis_global("mtvscr", "v%d", code.vb);
}

std::string dis_vaddcuw(PPUFields code)
{
    return dis_global("vaddcuw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vaddfp(PPUFields code)
{
    return dis_global("vaddfp", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vaddsbs(PPUFields code)
{
    return dis_global("vaddsbs", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vaddshs(PPUFields code)
{
    return dis_global("vaddshs", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vaddsws(PPUFields code)
{
    return dis_global("vaddsws", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vaddubm(PPUFields code)
{
    return dis_global("vaddubm", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vaddubs(PPUFields code)
{
    return dis_global("vaddubs", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vadduhm(PPUFields code)
{
    return dis_global("vadduhm", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vadduhs(PPUFields code)
{
    return dis_global("vadduhs", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vadduwm(PPUFields code)
{
    return dis_global("vadduwm", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vadduws(PPUFields code)
{
    return dis_global("vadduws", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vand(PPUFields code)
{
    return dis_global("vand", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vandc(PPUFields code)
{
    return dis_global("vandc", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vavgsb(PPUFields code)
{
    return dis_global("vavgsb", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vavgsh(PPUFields code)
{
    return dis_global("vavgsh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vavgsw(PPUFields code)
{
    return dis_global("vavgsw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vavgub(PPUFields code)
{
    return dis_global("vavgub", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vavguh(PPUFields code)
{
    return dis_global("vavguh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vavguw(PPUFields code)
{
    return dis_global("vavguw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcfsx(PPUFields code)
{
    return dis_global("vcfsx", "v%d, v%d, 0x%X", code.vd, code.vb, code.vuimm);
}

std::string dis_vcfux(PPUFields code)
{
    return dis_global("vcfux", "v%d, v%d, 0x%X", code.vd, code.vb, code.vuimm);
}

std::string dis_vcmpbfp(PPUFields code)
{
    return dis_global("vcmpbfp", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpbfp_(PPUFields code)
{
    return dis_global("vcmpbfp.", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpeqfp(PPUFields code)
{
    return dis_global("vcmpeqfp", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpeqfp_(PPUFields code)
{
    return dis_global("vcmpeqfp.", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpequb(PPUFields code)
{
    return dis_global("vcmpequb", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpequb_(PPUFields code)
{
    return dis_global("vcmpequb.", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpequh(PPUFields code)
{
    return dis_global("vcmpequh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpequh_(PPUFields code)
{
    return dis_global("vcmpequh.", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpequw(PPUFields code)
{
    return dis_global("vcmpequw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpequw_(PPUFields code)
{
    return dis_global("vcmpequw.", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgefp(PPUFields code)
{
    return dis_global("vcmpgefp", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgefp_(PPUFields code)
{
    return dis_global("vcmpgefp.", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgtfp(PPUFields code)
{
    return dis_global("vcmpgtfp", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgtfp_(PPUFields code)
{
    return dis_global("vcmpgtfp.", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgtsb(PPUFields code)
{
    return dis_global("vcmpgtsb", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgtsb_(PPUFields code)
{
    return dis_global("vcmpgtsb.", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgtsh(PPUFields code)
{
    return dis_global("vcmpgtsh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgtsh_(PPUFields code)
{
    return dis_global("vcmpgtsh.", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgtsw(PPUFields code)
{
    return dis_global("vcmpgtsw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgtsw_(PPUFields code)
{
    return dis_global("vcmpgtsw.", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgtub(PPUFields code)
{
    return dis_global("vcmpgtub", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgtub_(PPUFields code)
{
    return dis_global("vcmpgtub.", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgtuh(PPUFields code)
{
    return dis_global("vcmpgtuh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgtuh_(PPUFields code)
{
    return dis_global("vcmpgtuh.", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgtuw(PPUFields code)
{
    return dis_global("vcmpgtuw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vcmpgtuw_(PPUFields code)
{
    return dis_global("vcmpgtuw.", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vctsxs(PPUFields code)
{
    return dis_global("vctsxs", "v%d, v%d, 0x%X", code.vd, code.vb, code.vuimm);
}

std::string dis_vctuxs(PPUFields code)
{
    return dis_global("vctuxs", "v%d, v%d, 0x%X", code.vd, code.vb, code.vuimm);
}

std::string dis_vexptefp(PPUFields code)
{
    return dis_global("vexptefp", "v%d, v%d", code.vd, code.vb);
}

std::string dis_vlogefp(PPUFields code)
{
    return dis_global("vlogefp", "v%d, v%d", code.vd, code.vb);
}

std::string dis_vmaddfp(PPUFields code)
{
    return dis_global("vmaddfp", "v%d, v%d, v%d, v%d", code.vd, code.va, code.vc, code.vb);
}

std::string dis_vmaxfp(PPUFields code)
{
    return dis_global("vmaxfp", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmaxsb(PPUFields code)
{
    return dis_global("vmaxsb", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmaxsh(PPUFields code)
{
    return dis_global("vmaxsh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmaxsw(PPUFields code)
{
    return dis_global("vmaxsw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmaxub(PPUFields code)
{
    return dis_global("vmaxub", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmaxuh(PPUFields code)
{
    return dis_global("vmaxuh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmaxuw(PPUFields code)
{
    return dis_global("vmaxuw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmhaddshs(PPUFields code)
{
    return dis_global("vmhaddshs", "v%d, v%d, v%d, v%d", code.vd, code.va, code.vb, code.vc);
}

std::string dis_vmhraddshs(PPUFields code)
{
    return dis_global("vmhraddshs", "v%d, v%d, v%d, v%d", code.vd, code.va, code.vb, code.vc);
}

std::string dis_vminfp(PPUFields code)
{
    return dis_global("vminfp", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vminsb(PPUFields code)
{
    return dis_global("vminsb", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vminsh(PPUFields code)
{
    return dis_global("vminsh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vminsw(PPUFields code)
{
    return dis_global("vminsw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vminub(PPUFields code)
{
    return dis_global("vminub", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vminuh(PPUFields code)
{
    return dis_global("vminuh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vminuw(PPUFields code)
{
    return dis_global("vminuw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmladduhm(PPUFields code)
{
    return dis_global("vmladduhm", "v%d, v%d, v%d, v%d", code.vd, code.va, code.vb, code.vc);
}

std::string dis_vmrghb(PPUFields code)
{
    return dis_global("vmrghb", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmrghh(PPUFields code)
{
    return dis_global("vmrghh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmrghw(PPUFields code)
{
    return dis_global("vmrghw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmrglb(PPUFields code)
{
    return dis_global("vmrglb", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmrglh(PPUFields code)
{
    return dis_global("vmrglh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmrglw(PPUFields code)
{
    return dis_global("vmrglw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmsummbm(PPUFields code)
{
    return dis_global("vmsummbm", "v%d, v%d, v%d, v%d", code.vd, code.va, code.vb, code.vc);
}

std::string dis_vmsumshm(PPUFields code)
{
    return dis_global("vmsumshm", "v%d, v%d, v%d, v%d", code.vd, code.va, code.vb, code.vc);
}

std::string dis_vmsumshs(PPUFields code)
{
    return dis_global("vmsumshs", "v%d, v%d, v%d, v%d", code.vd, code.va, code.vb, code.vc);
}

std::string dis_vmsumubm(PPUFields code)
{
    return dis_global("vmsumubm", "v%d, v%d, v%d, v%d", code.vd, code.va, code.vb, code.vc);
}

std::string dis_vmsumuhm(PPUFields code)
{
    return dis_global("vmsumuhm", "v%d, v%d, v%d, v%d", code.vd, code.va, code.vb, code.vc);
}

std::string dis_vmsumuhs(PPUFields code)
{
    return dis_global("vmsumuhs", "v%d, v%d, v%d, v%d", code.vd, code.va, code.vb, code.vc);
}

std::string dis_vmulesb(PPUFields code)
{
    return dis_global("vmulesb", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmulesh(PPUFields code)
{
    return dis_global("vmulesh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmuleub(PPUFields code)
{
    return dis_global("vmuleub", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmuleuh(PPUFields code)
{
    return dis_global("vmuleuh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmulosb(PPUFields code)
{
    return dis_global("vmulosb", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmulosh(PPUFields code)
{
    return dis_global("vmulosh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmuloub(PPUFields code)
{
    return dis_global("vmuloub", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vmulouh(PPUFields code)
{
    return dis_global("vmulouh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vnmsubfp(PPUFields code)
{
    return dis_global("vmsumubm", "v%d, v%d, v%d, v%d", code.vd, code.va, code.vc, code.vb);
}

std::string dis_vnor(PPUFields code)
{
    return dis_global("vnor", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vor(PPUFields code)
{
    return dis_global("vor", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vperm(PPUFields code)
{
    return dis_global("vperm", "v%d, v%d, v%d, v%d", code.vd, code.va, code.vb, code.vc);
}

std::string dis_vpkpx(PPUFields code)
{
    return dis_global("vpkpx", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vpkshss(PPUFields code)
{
    return dis_global("vpkshss", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vpkshus(PPUFields code)
{
    return dis_global("vpkshus", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vpkswss(PPUFields code)
{
    return dis_global("vpkswss", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vpkswus(PPUFields code)
{
    return dis_global("vpkswus", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vpkuhum(PPUFields code)
{
    return dis_global("vpkuhum", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vpkuhus(PPUFields code)
{
    return dis_global("vpkuhus", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vpkuwum(PPUFields code)
{
    return dis_global("vpkuwum", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vpkuwus(PPUFields code)
{
    return dis_global("vpkuwus", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vrefp(PPUFields code)
{
    return dis_global("vrefp", "v%d, v%d", code.vd, code.vb);
}

std::string dis_vrfim(PPUFields code)
{
    return dis_global("vrfim", "v%d, v%d", code.vd, code.vb);
}

std::string dis_vrfin(PPUFields code)
{
    return dis_global("vrfin", "v%d, v%d", code.vd, code.vb);
}

std::string dis_vrfip(PPUFields code)
{
    return dis_global("vrfip", "v%d, v%d", code.vd, code.vb);
}

std::string dis_vrfiz(PPUFields code)
{
    return dis_global("vrfiz", "v%d, v%d", code.vd, code.vb);
}

std::string dis_vrlb(PPUFields code)
{
    return dis_global("vrlb", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vrlh(PPUFields code)
{
    return dis_global("vrlh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vrlw(PPUFields code)
{
    return dis_global("vrlw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vrsqrtefp(PPUFields code)
{
    return dis_global("vrsqrtefp", "v%d, v%d", code.vd, code.vb);
}

std::string dis_vsel(PPUFields code)
{
    return dis_global("vsel", "v%d, v%d, v%d, v%d", code.vd, code.va, code.vb, code.vc);
}

std::string dis_vsl(PPUFields code)
{
    return dis_global("vsl", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vslb(PPUFields code)
{
    return dis_global("vslb", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsldoi(PPUFields code)
{
    return dis_global("vsldoi", "v%d, v%d, v%d, 0x%X", code.vd, code.va, code.vb, code.vshb);
}

std::string dis_vslh(PPUFields code)
{
    return dis_global("vslh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vslo(PPUFields code)
{
    return dis_global("vslo", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vslw(PPUFields code)
{
    return dis_global("vslw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vspltb(PPUFields code)
{
    return dis_global("vspltb", "v%d, v%d, 0x%X", code.vd, code.vb, code.vuimm);
}

std::string dis_vsplth(PPUFields code)
{
    return dis_global("vsplth", "v%d, v%d, 0x%X", code.vd, code.vb, code.vuimm);
}

std::string dis_vspltisb(PPUFields code)
{
    return dis_global("vspltisb", "v%d, 0x%X", code.vd, code.vsimm);
}

std::string dis_vspltish(PPUFields code)
{
    return dis_global("vspltish", "v%d, 0x%X", code.vd, code.vsimm);
}

std::string dis_vspltisw(PPUFields code)
{
    return dis_global("vspltisw", "v%d, 0x%X", code.vd, code.vsimm);
}

std::string dis_vspltw(PPUFields code)
{
    return dis_global("vspltw", "v%d, v%d, 0x%X", code.vd, code.vb, code.vuimm);
}

std::string dis_vsr(PPUFields code)
{
    return dis_global("vsr", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsrab(PPUFields code)
{
    return dis_global("vsrab", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsrah(PPUFields code)
{
    return dis_global("vsrah", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsraw(PPUFields code)
{
    return dis_global("vsraw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsrb(PPUFields code)
{
    return dis_global("vsrb", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsrh(PPUFields code)
{
    return dis_global("vsrh", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsro(PPUFields code)
{
    return dis_global("vsro", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsrw(PPUFields code)
{
    return dis_global("vsrw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsubcuw(PPUFields code)
{
    return dis_global("vsubcuw", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsubfp(PPUFields code)
{
    return dis_global("vsubfp", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsubsbs(PPUFields code)
{
    return dis_global("vsubsbs", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsubshs(PPUFields code)
{
    return dis_global("vsubshs", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsubsws(PPUFields code)
{
    return dis_global("vsubsws", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsububm(PPUFields code)
{
    return dis_global("vsububm", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsububs(PPUFields code)
{
    return dis_global("vsububs", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsubuhm(PPUFields code)
{
    return dis_global("vsubuhm", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsubuhs(PPUFields code)
{
    return dis_global("vsubuhs", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsubuwm(PPUFields code)
{
    return dis_global("vsubuwm", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsubuws(PPUFields code)
{
    return dis_global("vsubuws", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsum2sws(PPUFields code)
{
    return dis_global("vsum2sws", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsum4sbs(PPUFields code)
{
    return dis_global("vsum4sbs", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsum4shs(PPUFields code)
{
    return dis_global("vsum4shs", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsum4ubs(PPUFields code)
{
    return dis_global("vsum4ubs", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vsumsws(PPUFields code)
{
    return dis_global("vsumsws", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}

std::string dis_vupkhpx(PPUFields code)
{
    return dis_global("vupkhpx", "v%d, v%d", code.vd, code.vb);
}

std::string dis_vupkhsb(PPUFields code)
{
    return dis_global("vupkhsb", "v%d, v%d", code.vd, code.vb);
}

std::string dis_vupkhsh(PPUFields code)
{
    return dis_global("vupkhsh", "v%d, v%d", code.vd, code.vb);
}

std::string dis_vupklpx(PPUFields code)
{
    return dis_global("vupklpx", "v%d, v%d", code.vd, code.vb);
}

std::string dis_vupklsb(PPUFields code)
{
    return dis_global("vupklsb", "v%d, v%d", code.vd, code.vb);
}

std::string dis_vupklsh(PPUFields code)
{
    return dis_global("vupklsh", "v%d, v%d", code.vd, code.vb);
}

std::string dis_vxor(PPUFields code)
{
    return dis_global("vxor", "v%d, v%d, v%d", code.vd, code.va, code.vb);
}


// Unknown
std::string dis_unknown(PPUFields code)
{
    return dis_global("???", "");
}
