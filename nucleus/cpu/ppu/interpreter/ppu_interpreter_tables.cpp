/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "ppu_interpreter_tables.h"
#include "nucleus/cpu/ppu/interpreter/ppu_interpreter.h"
#include "nucleus/cpu/ppu/ppu_thread.h"

typedef void (*func_t)(PPUInstruction, PPUThread&);

extern func_t s_tablePrimary[0x40];
extern func_t s_table4[0x40];
extern func_t s_table4_[0x800];
extern func_t s_table19[0x400];
extern func_t s_table30[0x8];
extern func_t s_table31[0x400];
extern func_t s_table58[0x4];
extern func_t s_table59[0x20];
extern func_t s_table62[0x4];
extern func_t s_table63[0x20];
extern func_t s_table63_[0x400];

void initTables()
{
    // Initialize Primary Table
    for (auto& caller : s_tablePrimary) {
        caller = PPUInterpreter::unknown;
    }
    s_tablePrimary[0x04] = PPUInterpreter::callTable4;
    s_tablePrimary[0x13] = PPUInterpreter::callTable19;
    s_tablePrimary[0x1e] = PPUInterpreter::callTable30;
    s_tablePrimary[0x1f] = PPUInterpreter::callTable31;
    s_tablePrimary[0x3a] = PPUInterpreter::callTable58;
    s_tablePrimary[0x3b] = PPUInterpreter::callTable59;
    s_tablePrimary[0x3e] = PPUInterpreter::callTable62;
    s_tablePrimary[0x3f] = PPUInterpreter::callTable63;
    s_tablePrimary[0x02] = PPUInterpreter::tdi;
    s_tablePrimary[0x03] = PPUInterpreter::twi;
    s_tablePrimary[0x07] = PPUInterpreter::mulli;
    s_tablePrimary[0x08] = PPUInterpreter::subfic;
    s_tablePrimary[0x0a] = PPUInterpreter::cmpli;
    s_tablePrimary[0x0b] = PPUInterpreter::cmpi;
    s_tablePrimary[0x0c] = PPUInterpreter::addic;
    s_tablePrimary[0x0d] = PPUInterpreter::addic_;
    s_tablePrimary[0x0e] = PPUInterpreter::addi;
    s_tablePrimary[0x0f] = PPUInterpreter::addis;
    s_tablePrimary[0x10] = PPUInterpreter::bc;
    s_tablePrimary[0x11] = PPUInterpreter::sc;
    s_tablePrimary[0x12] = PPUInterpreter::b;
    s_tablePrimary[0x14] = PPUInterpreter::rlwimi;
    s_tablePrimary[0x15] = PPUInterpreter::rlwinm;
    s_tablePrimary[0x17] = PPUInterpreter::rlwnm;
    s_tablePrimary[0x18] = PPUInterpreter::ori;
    s_tablePrimary[0x19] = PPUInterpreter::oris;
    s_tablePrimary[0x1a] = PPUInterpreter::xori;
    s_tablePrimary[0x1b] = PPUInterpreter::xoris;
    s_tablePrimary[0x1c] = PPUInterpreter::andi_;
    s_tablePrimary[0x1d] = PPUInterpreter::andis_;
    s_tablePrimary[0x20] = PPUInterpreter::lwz;
    s_tablePrimary[0x21] = PPUInterpreter::lwzu;
    s_tablePrimary[0x22] = PPUInterpreter::lbz;
    s_tablePrimary[0x23] = PPUInterpreter::lbzu;
    s_tablePrimary[0x24] = PPUInterpreter::stw;
    s_tablePrimary[0x25] = PPUInterpreter::stwu;
    s_tablePrimary[0x26] = PPUInterpreter::stb;
    s_tablePrimary[0x27] = PPUInterpreter::stbu;
    s_tablePrimary[0x28] = PPUInterpreter::lhz;
    s_tablePrimary[0x29] = PPUInterpreter::lhzu;
    s_tablePrimary[0x2a] = PPUInterpreter::lha;
    s_tablePrimary[0x2b] = PPUInterpreter::lhau;
    s_tablePrimary[0x2c] = PPUInterpreter::sth;
    s_tablePrimary[0x2d] = PPUInterpreter::sthu;
    s_tablePrimary[0x2e] = PPUInterpreter::lmw;
    s_tablePrimary[0x2f] = PPUInterpreter::stmw;
    s_tablePrimary[0x30] = PPUInterpreter::lfs;
    s_tablePrimary[0x31] = PPUInterpreter::lfsu;
    s_tablePrimary[0x32] = PPUInterpreter::lfd;
    s_tablePrimary[0x33] = PPUInterpreter::lfdu;
    s_tablePrimary[0x34] = PPUInterpreter::stfs;
    s_tablePrimary[0x35] = PPUInterpreter::stfsu;
    s_tablePrimary[0x36] = PPUInterpreter::stfd;
    s_tablePrimary[0x37] = PPUInterpreter::stfdu;

    // Initialize Table 4
    for (auto& caller : s_table4) {
        caller = PPUInterpreter::callTable4_;
    }

    // Initialize Table 4 (Extended)
    for (auto& caller : s_table4_) {
        caller = PPUInterpreter::unknown;
    }

    // Initialize Table 19 
    for (auto& caller : s_table19) {
        caller = PPUInterpreter::unknown;
    }
    s_table19[0x000] = PPUInterpreter::mcrf;
    s_table19[0x010] = PPUInterpreter::bclr;
    s_table19[0x021] = PPUInterpreter::crnor;
    s_table19[0x081] = PPUInterpreter::crandc;
    s_table19[0x096] = PPUInterpreter::isync;
    s_table19[0x0c1] = PPUInterpreter::crxor;
    s_table19[0x0e1] = PPUInterpreter::crnand;
    s_table19[0x101] = PPUInterpreter::crand;
    s_table19[0x121] = PPUInterpreter::creqv;
    s_table19[0x1a1] = PPUInterpreter::crorc;
    s_table19[0x1c1] = PPUInterpreter::cror;
    s_table19[0x210] = PPUInterpreter::bcctr;

    // Initialize Table 30
    for (auto& caller : s_table30) {
        caller = PPUInterpreter::unknown;
    }
    s_table30[0x0] = PPUInterpreter::rldicl;
    s_table30[0x1] = PPUInterpreter::rldicr;
    s_table30[0x2] = PPUInterpreter::rldic;
    s_table30[0x3] = PPUInterpreter::rldimi;
    s_table30[0x4] = PPUInterpreter::rldc_lr;

    // Initialize Table 31
    for (auto& caller : s_table31) {
        caller = PPUInterpreter::unknown;
    }
    s_table31[0x000] = PPUInterpreter::cmp;
    s_table31[0x004] = PPUInterpreter::tw;
    s_table31[0x006] = PPUInterpreter::lvsl;
    s_table31[0x007] = PPUInterpreter::lvebx;
    s_table31[0x008] = PPUInterpreter::subfc;
    s_table31[0x009] = PPUInterpreter::mulhdu;
    s_table31[0x00a] = PPUInterpreter::addc;
    s_table31[0x00b] = PPUInterpreter::mulhwu;
    s_table31[0x013] = PPUInterpreter::mfocrf;
    s_table31[0x014] = PPUInterpreter::lwarx;
    s_table31[0x015] = PPUInterpreter::ldx;
    s_table31[0x017] = PPUInterpreter::lwzx;
    s_table31[0x018] = PPUInterpreter::slw;
    s_table31[0x01a] = PPUInterpreter::cntlzw;
    s_table31[0x01b] = PPUInterpreter::sld;
    s_table31[0x01c] = PPUInterpreter::and;
    s_table31[0x020] = PPUInterpreter::cmpl;
    s_table31[0x026] = PPUInterpreter::lvsr;
    s_table31[0x027] = PPUInterpreter::lvehx;
    s_table31[0x028] = PPUInterpreter::subf;
    s_table31[0x035] = PPUInterpreter::ldux;
    s_table31[0x036] = PPUInterpreter::dcbst;
    s_table31[0x037] = PPUInterpreter::lwzux;
    s_table31[0x03a] = PPUInterpreter::cntlzd;
    s_table31[0x03c] = PPUInterpreter::andc;
    s_table31[0x03c] = PPUInterpreter::td;
    s_table31[0x047] = PPUInterpreter::lvewx;
    s_table31[0x049] = PPUInterpreter::mulhd;
    s_table31[0x04b] = PPUInterpreter::mulhw;
    s_table31[0x054] = PPUInterpreter::ldarx;
    s_table31[0x056] = PPUInterpreter::dcbf;
    s_table31[0x057] = PPUInterpreter::lbzx;
    s_table31[0x067] = PPUInterpreter::lvx;
    s_table31[0x068] = PPUInterpreter::neg;
    s_table31[0x077] = PPUInterpreter::lbzux;
    s_table31[0x07c] = PPUInterpreter::nor;
    s_table31[0x087] = PPUInterpreter::stvebx;
    s_table31[0x088] = PPUInterpreter::subfe;
    s_table31[0x08a] = PPUInterpreter::adde;
    s_table31[0x090] = PPUInterpreter::mtocrf;
    s_table31[0x095] = PPUInterpreter::stdx;
    s_table31[0x096] = PPUInterpreter::stwcx_;
    s_table31[0x097] = PPUInterpreter::stwx;
    s_table31[0x0a7] = PPUInterpreter::stvehx;
    s_table31[0x0b5] = PPUInterpreter::stdux;
    s_table31[0x0b7] = PPUInterpreter::stwux;
    s_table31[0x0c7] = PPUInterpreter::stvewx;
    s_table31[0x0c8] = PPUInterpreter::subfze;
    s_table31[0x0ca] = PPUInterpreter::addze;
    s_table31[0x0d6] = PPUInterpreter::stdcx_;
    s_table31[0x0d7] = PPUInterpreter::stbx;
    s_table31[0x0e7] = PPUInterpreter::stvx;
    s_table31[0x0e8] = PPUInterpreter::subfme;
    s_table31[0x0e9] = PPUInterpreter::mulld;
    s_table31[0x0ea] = PPUInterpreter::addme;
    s_table31[0x0eb] = PPUInterpreter::mullw;
    s_table31[0x0f6] = PPUInterpreter::dcbtst;
    s_table31[0x0f7] = PPUInterpreter::stbux;
    s_table31[0x10a] = PPUInterpreter::add;
    s_table31[0x116] = PPUInterpreter::dcbt;
    s_table31[0x117] = PPUInterpreter::lhzx;
    s_table31[0x11c] = PPUInterpreter::eqv;
    s_table31[0x136] = PPUInterpreter::eciwx;
    s_table31[0x137] = PPUInterpreter::lhzux;
    s_table31[0x13c] = PPUInterpreter::xor;
    s_table31[0x153] = PPUInterpreter::mfspr;
    s_table31[0x155] = PPUInterpreter::lwax;
    s_table31[0x156] = PPUInterpreter::dst;
    s_table31[0x157] = PPUInterpreter::lhax;
    s_table31[0x167] = PPUInterpreter::lvxl;
    s_table31[0x173] = PPUInterpreter::mftb;
    s_table31[0x175] = PPUInterpreter::lwaux;
    s_table31[0x176] = PPUInterpreter::dstst;
    s_table31[0x177] = PPUInterpreter::lhaux;
    s_table31[0x197] = PPUInterpreter::sthx;
    s_table31[0x19c] = PPUInterpreter::orc;
    s_table31[0x1b6] = PPUInterpreter::ecowx;
    s_table31[0x1b7] = PPUInterpreter::sthux;
    s_table31[0x1bc] = PPUInterpreter::or;
    s_table31[0x1c9] = PPUInterpreter::divdu;
    s_table31[0x1cb] = PPUInterpreter::divwu;
    s_table31[0x1d3] = PPUInterpreter::mtspr;
    s_table31[0x1dc] = PPUInterpreter::nand;
    s_table31[0x1e7] = PPUInterpreter::stvxl;
    s_table31[0x1e9] = PPUInterpreter::divd;
    s_table31[0x1eb] = PPUInterpreter::divw;
    s_table31[0x207] = PPUInterpreter::lvlx;
    s_table31[0x214] = PPUInterpreter::ldbrx;
    s_table31[0x215] = PPUInterpreter::lswx;
    s_table31[0x216] = PPUInterpreter::lwbrx;
    s_table31[0x217] = PPUInterpreter::lfsx;
    s_table31[0x218] = PPUInterpreter::srw;
    s_table31[0x21b] = PPUInterpreter::srd;
    s_table31[0x227] = PPUInterpreter::lvrx;
    s_table31[0x237] = PPUInterpreter::lfsux;
    s_table31[0x255] = PPUInterpreter::lswi;
    s_table31[0x256] = PPUInterpreter::sync;
    s_table31[0x257] = PPUInterpreter::lfdx;
    s_table31[0x277] = PPUInterpreter::lfdux;
    s_table31[0x287] = PPUInterpreter::stvlx;
    s_table31[0x296] = PPUInterpreter::stswx;
    s_table31[0x296] = PPUInterpreter::stwbrx;
    s_table31[0x297] = PPUInterpreter::stfsx;
    s_table31[0x2a7] = PPUInterpreter::stvrx;
    s_table31[0x2b7] = PPUInterpreter::stfsux;
    s_table31[0x2d5] = PPUInterpreter::stswi;
    s_table31[0x2d7] = PPUInterpreter::stfdx;
    s_table31[0x2d7] = PPUInterpreter::stfdux;
    s_table31[0x307] = PPUInterpreter::lvlxl;
    s_table31[0x316] = PPUInterpreter::lhbrx;
    s_table31[0x318] = PPUInterpreter::sraw;
    s_table31[0x31a] = PPUInterpreter::srad;
    s_table31[0x327] = PPUInterpreter::lvrxl;
    s_table31[0x336] = PPUInterpreter::dss;
    s_table31[0x338] = PPUInterpreter::srawi;
    s_table31[0x33a] = PPUInterpreter::sradi1;
    s_table31[0x33b] = PPUInterpreter::sradi2;
    s_table31[0x356] = PPUInterpreter::eieio;
    s_table31[0x387] = PPUInterpreter::stvlxl;
    s_table31[0x396] = PPUInterpreter::sthbrx;
    s_table31[0x39a] = PPUInterpreter::extsh;
    s_table31[0x387] = PPUInterpreter::stvrxl;
    s_table31[0x3ba] = PPUInterpreter::extsb;
    s_table31[0x3d7] = PPUInterpreter::stfiwx;
    s_table31[0x3da] = PPUInterpreter::extsw;
    s_table31[0x3d6] = PPUInterpreter::icbi;
    s_table31[0x3f6] = PPUInterpreter::dcbz;

    // Initialize Table 58
    for (auto& caller : s_table58) {
        caller = PPUInterpreter::unknown;
    }
    s_table58[0x0] = PPUInterpreter::ld;
    s_table58[0x1] = PPUInterpreter::ldu;
    s_table58[0x2] = PPUInterpreter::lwa;

    // Initialize Table 59
    for (auto& caller : s_table59) {
        caller = PPUInterpreter::unknown;
    }

    // Initialize Table 62
    for (auto& caller : s_table62) {
        caller = PPUInterpreter::unknown;
    }


    // Initialize Table 63
    for (auto& caller : s_table63) {
        caller = PPUInterpreter::callTable63_;
    }
    s_table62[0x0] = PPUInterpreter::std;
    s_table62[0x1] = PPUInterpreter::stdu;

    // Initialize Table 63 (Extended)
    for (auto& caller : s_table63_) {
        caller = PPUInterpreter::unknown;
    }
}
