/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_tables.h"
#include "nucleus/cpu/ppu/interpreter/ppu_interpreter.h"
#include "nucleus/cpu/ppu/ppu_disasm.h"
#include "nucleus/cpu/ppu/ppu_thread.h"

PPUInstruction s_tablePrimary[0x40];
PPUInstruction s_table4[0x40];
PPUInstruction s_table4_[0x800];
PPUInstruction s_table19[0x400];
PPUInstruction s_table30[0x8];
PPUInstruction s_table31[0x400];
PPUInstruction s_table58[0x4];
PPUInstruction s_table59[0x20];
PPUInstruction s_table62[0x4];
PPUInstruction s_table63[0x20];
PPUInstruction s_table63_[0x400];

void initTables()
{
    // Initialize Primary Table
    for (auto& caller : s_tablePrimary) {
        caller = {"unknown", PPUInterpreter::unknown, dis_unknown};
    }
    s_tablePrimary[0x04] = {"",       PPUInterpreter::callTable4,   dis_table4};
    s_tablePrimary[0x13] = {"",       PPUInterpreter::callTable19,  dis_table19};
    s_tablePrimary[0x1e] = {"",       PPUInterpreter::callTable30,  dis_table30};
    s_tablePrimary[0x1f] = {"",       PPUInterpreter::callTable31,  dis_table31};
    s_tablePrimary[0x3a] = {"",       PPUInterpreter::callTable58,  dis_table58};
    s_tablePrimary[0x3b] = {"",       PPUInterpreter::callTable59,  dis_table59};
    s_tablePrimary[0x3e] = {"",       PPUInterpreter::callTable62,  dis_table62};
    s_tablePrimary[0x3f] = {"",       PPUInterpreter::callTable63,  dis_table63};
    s_tablePrimary[0x02] = {"tdi",    PPUInterpreter::tdi,          dis_tdi};
    s_tablePrimary[0x03] = {"twi",    PPUInterpreter::twi,          dis_twi};
    s_tablePrimary[0x07] = {"mulli",  PPUInterpreter::mulli,        dis_mulli};
    s_tablePrimary[0x08] = {"subfic", PPUInterpreter::subfic,       dis_subfic};
    s_tablePrimary[0x0a] = {"cmpli",  PPUInterpreter::cmpli,        dis_cmpli};
    s_tablePrimary[0x0b] = {"cmpi",   PPUInterpreter::cmpi,         dis_cmpi};
    s_tablePrimary[0x0c] = {"addic",  PPUInterpreter::addic,        dis_addic};
    s_tablePrimary[0x0d] = {"addic_", PPUInterpreter::addic_,       dis_addic_};
    s_tablePrimary[0x0e] = {"addi",   PPUInterpreter::addi,         dis_addi};
    s_tablePrimary[0x0f] = {"addis",  PPUInterpreter::addis,        dis_addis};
    s_tablePrimary[0x10] = {"bc",     PPUInterpreter::bc,           dis_bcx};
    s_tablePrimary[0x11] = {"sc",     PPUInterpreter::sc,           dis_sc};
    s_tablePrimary[0x12] = {"b",      PPUInterpreter::b,            dis_bx};
    s_tablePrimary[0x14] = {"rlwimi", PPUInterpreter::rlwimix,      dis_rlwimix};
    s_tablePrimary[0x15] = {"rlwinm", PPUInterpreter::rlwinmx,      dis_rlwinmx};
    s_tablePrimary[0x17] = {"rlwnm",  PPUInterpreter::rlwnmx,       dis_rlwnmx};
    s_tablePrimary[0x18] = {"ori",    PPUInterpreter::ori,          dis_ori};
    s_tablePrimary[0x19] = {"oris",   PPUInterpreter::oris,         dis_oris};
    s_tablePrimary[0x1a] = {"xori",   PPUInterpreter::xori,         dis_xori};
    s_tablePrimary[0x1b] = {"xoris",  PPUInterpreter::xoris,        dis_xoris};
    s_tablePrimary[0x1c] = {"andi_",  PPUInterpreter::andi_,        dis_andi_};
    s_tablePrimary[0x1d] = {"andis_", PPUInterpreter::andis_,       dis_andis_};
    s_tablePrimary[0x20] = {"lwz",    PPUInterpreter::lwz,          dis_lwz};
    s_tablePrimary[0x21] = {"lwzu",   PPUInterpreter::lwzu,         dis_lwzu};
    s_tablePrimary[0x22] = {"lbz",    PPUInterpreter::lbz,          dis_lbz};
    s_tablePrimary[0x23] = {"lbzu",   PPUInterpreter::lbzu,         dis_lbzu};
    s_tablePrimary[0x24] = {"stw",    PPUInterpreter::stw,          dis_stw};
    s_tablePrimary[0x25] = {"stwu",   PPUInterpreter::stwu,         dis_stwu};
    s_tablePrimary[0x26] = {"stb",    PPUInterpreter::stb,          dis_stb};
    s_tablePrimary[0x27] = {"stbu",   PPUInterpreter::stbu,         dis_stbu};
    s_tablePrimary[0x28] = {"lhz",    PPUInterpreter::lhz,          dis_lhz};
    s_tablePrimary[0x29] = {"lhzu",   PPUInterpreter::lhzu,         dis_lhzu};
    s_tablePrimary[0x2a] = {"lha",    PPUInterpreter::lha,          dis_lha};
    s_tablePrimary[0x2b] = {"lhau",   PPUInterpreter::lhau,         dis_lhau};
    s_tablePrimary[0x2c] = {"sth",    PPUInterpreter::sth,          dis_sth};
    s_tablePrimary[0x2d] = {"sthu",   PPUInterpreter::sthu,         dis_sthu};
    s_tablePrimary[0x2e] = {"lmw",    PPUInterpreter::lmw,          dis_lmw};
    s_tablePrimary[0x2f] = {"stmw",   PPUInterpreter::stmw,         dis_stmw};
    s_tablePrimary[0x30] = {"lfs",    PPUInterpreter::lfs,          dis_unknown};
    s_tablePrimary[0x31] = {"lfsu",   PPUInterpreter::lfsu,         dis_unknown};
    s_tablePrimary[0x32] = {"lfd",    PPUInterpreter::lfd,          dis_unknown};
    s_tablePrimary[0x33] = {"lfdu",   PPUInterpreter::lfdu,         dis_unknown};
    s_tablePrimary[0x34] = {"stfs",   PPUInterpreter::stfs,         dis_unknown};
    s_tablePrimary[0x35] = {"stfsu",  PPUInterpreter::stfsu,        dis_unknown};
    s_tablePrimary[0x36] = {"stfd",   PPUInterpreter::stfd,         dis_unknown};
    s_tablePrimary[0x37] = {"stfdu",  PPUInterpreter::stfdu,        dis_unknown};


    // Initialize Table 4
    for (auto& caller : s_table4) {
        caller = {"", PPUInterpreter::callTable4_, dis_table4_};
    }

    // Initialize Table 4 (Extended)
    for (auto& caller : s_table4_) {
        caller = {"unknown", PPUInterpreter::unknown, dis_unknown};
    }

    // Initialize Table 19
    for (auto& caller : s_table19) {
        caller = {"unknown", PPUInterpreter::unknown, dis_unknown};
    }
    s_table19[0x000] = {"mcrf",     PPUInterpreter::mcrf,     dis_mcrf};
    s_table19[0x010] = {"bclr",     PPUInterpreter::bclr,     dis_bclrx};
    s_table19[0x021] = {"crnor",    PPUInterpreter::crnor,    dis_crnor};
    s_table19[0x081] = {"crandc",   PPUInterpreter::crandc,   dis_crandc};
    s_table19[0x096] = {"isync",    PPUInterpreter::isync,    dis_isync};
    s_table19[0x0c1] = {"crxor",    PPUInterpreter::crxor,    dis_crxor};
    s_table19[0x0e1] = {"crnand",   PPUInterpreter::crnand,   dis_crnand};
    s_table19[0x101] = {"crand",    PPUInterpreter::crand,    dis_crand};
    s_table19[0x121] = {"creqv",    PPUInterpreter::creqv,    dis_creqv};
    s_table19[0x1a1] = {"crorc",    PPUInterpreter::crorc,    dis_crorc};
    s_table19[0x1c1] = {"cror",     PPUInterpreter::cror,     dis_cror};
    s_table19[0x210] = {"bcctr",    PPUInterpreter::bcctr,    dis_bcctrx};

    // Initialize Table 30
    for (auto& caller : s_table30) {
        caller = {"unknown", PPUInterpreter::unknown, dis_unknown};
    }
    s_table30[0x0] = {"rldicl",    PPUInterpreter::rldiclx,   dis_rldiclx};
    s_table30[0x1] = {"rldicr",    PPUInterpreter::rldicrx,   dis_rldicrx};
    s_table30[0x2] = {"rldic",     PPUInterpreter::rldicx,    dis_rldicx};
    s_table30[0x3] = {"rldimi",    PPUInterpreter::rldimix,   dis_rldimix};
    s_table30[0x4] = {"rldc_lr",   PPUInterpreter::rldc_lr,   dis_unknown};

    // Initialize Table 31
    for (auto& caller : s_table31) {
        caller = {"unknown", PPUInterpreter::unknown, dis_unknown};
    }
    s_table31[0x000] = {"cmp",     PPUInterpreter::cmp,       dis_cmp};
    s_table31[0x004] = {"tw",      PPUInterpreter::tw,        dis_tw};
    s_table31[0x006] = {"lvsl",    PPUInterpreter::lvsl,      dis_unknown};
    s_table31[0x007] = {"lvebx",   PPUInterpreter::lvebx,     dis_unknown};
    s_table31[0x008] = {"subfc",   PPUInterpreter::subfc,     dis_subfcx};
    s_table31[0x009] = {"mulhdu",  PPUInterpreter::mulhdu,    dis_mulhdux};
    s_table31[0x00a] = {"addc",    PPUInterpreter::addc,      dis_addcx};
    s_table31[0x00b] = {"mulhwu",  PPUInterpreter::mulhwu,    dis_mulhwux};
    s_table31[0x013] = {"mfocrf",  PPUInterpreter::mfocrf,    dis_mfocrf};
    s_table31[0x014] = {"lwarx",   PPUInterpreter::lwarx,     dis_lwarx};
    s_table31[0x015] = {"ldx",     PPUInterpreter::ldx,       dis_ldx};
    s_table31[0x017] = {"lwzx",    PPUInterpreter::lwzx,      dis_lwzx};
    s_table31[0x018] = {"slw",     PPUInterpreter::slw,       dis_slwx};
    s_table31[0x01a] = {"cntlzw",  PPUInterpreter::cntlzw,    dis_cntlzwx};
    s_table31[0x01b] = {"sld",     PPUInterpreter::sld,       dis_sldx};
    s_table31[0x01c] = {"and",     PPUInterpreter::andx,      dis_andx};
    s_table31[0x020] = {"cmpl",    PPUInterpreter::cmpl,      dis_cmpl};
    s_table31[0x026] = {"lvsr",    PPUInterpreter::lvsr,      dis_unknown};
    s_table31[0x027] = {"lvehx",   PPUInterpreter::lvehx,     dis_unknown};
    s_table31[0x028] = {"subf",    PPUInterpreter::subf,      dis_subfx};
    s_table31[0x035] = {"ldux",    PPUInterpreter::ldux,      dis_ldux};
    s_table31[0x036] = {"dcbst",   PPUInterpreter::dcbst,     dis_dcbst};
    s_table31[0x037] = {"lwzux",   PPUInterpreter::lwzux,     dis_lwzux};
    s_table31[0x03a] = {"cntlzd",  PPUInterpreter::cntlzd,    dis_cntlzdx};
    s_table31[0x03c] = {"andc",    PPUInterpreter::andc,      dis_andcx};
    s_table31[0x044] = {"td",      PPUInterpreter::td,        dis_td};
    s_table31[0x047] = {"lvewx",   PPUInterpreter::lvewx,     dis_unknown};
    s_table31[0x049] = {"mulhd",   PPUInterpreter::mulhd,     dis_mulhdx};
    s_table31[0x04b] = {"mulhw",   PPUInterpreter::mulhw,     dis_mulhwx};
    s_table31[0x054] = {"ldarx",   PPUInterpreter::ldarx,     dis_ldarx};
    s_table31[0x056] = {"dcbf",    PPUInterpreter::dcbf,      dis_dcbf};
    s_table31[0x057] = {"lbzx",    PPUInterpreter::lbzx,      dis_lbzx};
    s_table31[0x067] = {"lvx",     PPUInterpreter::lvx,       dis_unknown};
    s_table31[0x068] = {"neg",     PPUInterpreter::neg,       dis_negx};
    s_table31[0x077] = {"lbzux",   PPUInterpreter::lbzux,     dis_lbzux};
    s_table31[0x07c] = {"nor",     PPUInterpreter::nor,       dis_norx};
    s_table31[0x087] = {"stvebx",  PPUInterpreter::stvebx,    dis_unknown};
    s_table31[0x088] = {"subfe",   PPUInterpreter::subfe,     dis_subfex};
    s_table31[0x08a] = {"adde",    PPUInterpreter::adde,      dis_addex};
    s_table31[0x090] = {"mtocrf",  PPUInterpreter::mtocrf,    dis_unknown};
    s_table31[0x095] = {"stdx",    PPUInterpreter::stdx,      dis_stdx};
    s_table31[0x096] = {"stwcx_",  PPUInterpreter::stwcx_,    dis_stwcx_};
    s_table31[0x097] = {"stwx",    PPUInterpreter::stwx,      dis_stwx};
    s_table31[0x0a7] = {"stvehx",  PPUInterpreter::stvehx,    dis_unknown};
    s_table31[0x0b5] = {"stdux",   PPUInterpreter::stdux,     dis_stdux};
    s_table31[0x0b7] = {"stwux",   PPUInterpreter::stwux,     dis_stwux};
    s_table31[0x0c7] = {"stvewx",  PPUInterpreter::stvewx,    dis_unknown};
    s_table31[0x0c8] = {"subfze",  PPUInterpreter::subfze,    dis_subfzex};
    s_table31[0x0ca] = {"addze",   PPUInterpreter::addze,     dis_addzex};
    s_table31[0x0d6] = {"stdcx_",  PPUInterpreter::stdcx_,    dis_stdcx_};
    s_table31[0x0d7] = {"stbx",    PPUInterpreter::stbx,      dis_stbx};
    s_table31[0x0e7] = {"stvx",    PPUInterpreter::stvx,      dis_unknown};
    s_table31[0x0e8] = {"subfme",  PPUInterpreter::subfme,    dis_subfmex};
    s_table31[0x0e9] = {"mulld",   PPUInterpreter::mulld,     dis_mulldx};
    s_table31[0x0ea] = {"addme",   PPUInterpreter::addme,     dis_addmex};
    s_table31[0x0eb] = {"mullw",   PPUInterpreter::mullw,     dis_mullwx};
    s_table31[0x0f6] = {"dcbtst",  PPUInterpreter::dcbtst,    dis_dcbtst};
    s_table31[0x0f7] = {"stbux",   PPUInterpreter::stbux,     dis_stbux};
    s_table31[0x10a] = {"add",     PPUInterpreter::add,       dis_addx};
    s_table31[0x116] = {"dcbt",    PPUInterpreter::dcbt,      dis_dcbt};
    s_table31[0x117] = {"lhzx",    PPUInterpreter::lhzx,      dis_lhzx};
    s_table31[0x11c] = {"eqv",     PPUInterpreter::eqv,       dis_eqvx};
    s_table31[0x136] = {"eciwx",   PPUInterpreter::eciwx,     dis_eciwx};
    s_table31[0x137] = {"lhzux",   PPUInterpreter::lhzux,     dis_lhzux};
    s_table31[0x13c] = {"xor",     PPUInterpreter::xorx,      dis_unknown};
    s_table31[0x153] = {"mfspr",   PPUInterpreter::mfspr,     dis_mfspr};
    s_table31[0x155] = {"lwax",    PPUInterpreter::lwax,      dis_lwax};
    s_table31[0x156] = {"dst",     PPUInterpreter::dst,       dis_unknown};
    s_table31[0x157] = {"lhax",    PPUInterpreter::lhax,      dis_lhax};
    s_table31[0x167] = {"lvxl",    PPUInterpreter::lvxl,      dis_unknown};
    s_table31[0x173] = {"mftb",    PPUInterpreter::mftb,      dis_mftb};
    s_table31[0x175] = {"lwaux",   PPUInterpreter::lwaux,     dis_lwaux};
    s_table31[0x176] = {"dstst",   PPUInterpreter::dstst,     dis_unknown};
    s_table31[0x177] = {"lhaux",   PPUInterpreter::lhaux,     dis_lhaux};
    s_table31[0x197] = {"sthx",    PPUInterpreter::sthx,      dis_sthx};
    s_table31[0x19c] = {"orc",     PPUInterpreter::orc,       dis_orcx};
    s_table31[0x1b6] = {"ecowx",   PPUInterpreter::ecowx,     dis_ecowx};
    s_table31[0x1b7] = {"sthux",   PPUInterpreter::sthux,     dis_sthux};
    s_table31[0x1bc] = {"or",      PPUInterpreter::orx,       dis_orx};
    s_table31[0x1c9] = {"divdu",   PPUInterpreter::divdu,     dis_divdux};
    s_table31[0x1cb] = {"divwu",   PPUInterpreter::divwu,     dis_divwux};
    s_table31[0x1d3] = {"mtspr",   PPUInterpreter::mtspr,     dis_mtspr};
    s_table31[0x1dc] = {"nand",    PPUInterpreter::nand,      dis_nandx};
    s_table31[0x1e7] = {"stvxl",   PPUInterpreter::stvxl,     dis_unknown};
    s_table31[0x1e9] = {"divd",    PPUInterpreter::divd,      dis_divdx};
    s_table31[0x1eb] = {"divw",    PPUInterpreter::divw,      dis_divwx};
    s_table31[0x207] = {"lvlx",    PPUInterpreter::lvlx,      dis_unknown};
    s_table31[0x214] = {"ldbrx",   PPUInterpreter::ldbrx,     dis_unknown};
    s_table31[0x215] = {"lswx",    PPUInterpreter::lswx,      dis_lswx};
    s_table31[0x216] = {"lwbrx",   PPUInterpreter::lwbrx,     dis_lwbrx};
    s_table31[0x217] = {"lfsx",    PPUInterpreter::lfsx,      dis_unknown};
    s_table31[0x218] = {"srw",     PPUInterpreter::srw,       dis_unknown};
    s_table31[0x21b] = {"srd",     PPUInterpreter::srd,       dis_unknown};
    s_table31[0x227] = {"lvrx",    PPUInterpreter::lvrx,      dis_unknown};
    s_table31[0x237] = {"lfsux",   PPUInterpreter::lfsux,     dis_unknown};
    s_table31[0x255] = {"lswi",    PPUInterpreter::lswi,      dis_lswi};
    s_table31[0x256] = {"sync",    PPUInterpreter::sync,      dis_sync};
    s_table31[0x257] = {"lfdx",    PPUInterpreter::lfdx,      dis_unknown};
    s_table31[0x277] = {"lfdux",   PPUInterpreter::lfdux,     dis_unknown};
    s_table31[0x287] = {"stvlx",   PPUInterpreter::stvlx,     dis_unknown};
    s_table31[0x296] = {"stswx",   PPUInterpreter::stswx,     dis_stswx};
    s_table31[0x296] = {"stwbrx",  PPUInterpreter::stwbrx,    dis_stwbrx};
    s_table31[0x297] = {"stfsx",   PPUInterpreter::stfsx,     dis_unknown};
    s_table31[0x2a7] = {"stvrx",   PPUInterpreter::stvrx,     dis_unknown};
    s_table31[0x2b7] = {"stfsux",  PPUInterpreter::stfsux,    dis_unknown};
    s_table31[0x2d5] = {"stswi",   PPUInterpreter::stswi,     dis_stswi};
    s_table31[0x2d7] = {"stfdx",   PPUInterpreter::stfdx,     dis_unknown};
    s_table31[0x2d7] = {"stfdux",  PPUInterpreter::stfdux,    dis_unknown};
    s_table31[0x307] = {"lvlxl",   PPUInterpreter::lvlxl,     dis_unknown};
    s_table31[0x316] = {"lhbrx",   PPUInterpreter::lhbrx,     dis_lhbrx};
    s_table31[0x318] = {"sraw",    PPUInterpreter::sraw,      dis_srawx};
    s_table31[0x31a] = {"srad",    PPUInterpreter::srad,      dis_sradx};
    s_table31[0x327] = {"lvrxl",   PPUInterpreter::lvrxl,     dis_unknown};
    s_table31[0x336] = {"dss",     PPUInterpreter::dss,       dis_unknown};
    s_table31[0x338] = {"srawi",   PPUInterpreter::srawi,     dis_srawix};
    s_table31[0x33a] = {"sradi",   PPUInterpreter::sradi,     dis_unknown};
    s_table31[0x33b] = {"sradi",   PPUInterpreter::sradi,     dis_unknown};
    s_table31[0x356] = {"eieio",   PPUInterpreter::eieio,     dis_eieio};
    s_table31[0x387] = {"stvlxl",  PPUInterpreter::stvlxl,    dis_unknown};
    s_table31[0x396] = {"sthbrx",  PPUInterpreter::sthbrx,    dis_sthbrx};
    s_table31[0x39a] = {"extsh",   PPUInterpreter::extsh,     dis_extshx};
    s_table31[0x387] = {"stvrxl",  PPUInterpreter::stvrxl,    dis_unknown};
    s_table31[0x3ba] = {"extsb",   PPUInterpreter::extsb,     dis_extsbx};
    s_table31[0x3d7] = {"stfiwx",  PPUInterpreter::stfiwx,    dis_unknown};
    s_table31[0x3da] = {"extsw",   PPUInterpreter::extsw,     dis_extswx};
    s_table31[0x3d6] = {"icbi",    PPUInterpreter::icbi,      dis_icbi};
    s_table31[0x3f6] = {"dcbz",    PPUInterpreter::dcbz,      dis_dcbz};

    // Initialize Table 58
    for (auto& caller : s_table58) {
        caller = {"unknown", PPUInterpreter::unknown, dis_unknown};
    }
    s_table58[0x0] = {"ld",    PPUInterpreter::ld,    dis_ld};
    s_table58[0x1] = {"ldu",   PPUInterpreter::ldu,   dis_ldu};
    s_table58[0x2] = {"lwa",   PPUInterpreter::lwa,   dis_lwa};

    // Initialize Table 59
    for (auto& caller : s_table59) {
        caller = {"unknown", PPUInterpreter::unknown, dis_unknown};
    }

    // Initialize Table 62
    for (auto& caller : s_table62) {
        caller = {"unknown", PPUInterpreter::unknown, dis_unknown};
    }


    // Initialize Table 63
    for (auto& caller : s_table63) {
        caller = {"", PPUInterpreter::callTable63_, dis_unknown};
    }
    s_table62[0x0] = {"std",   PPUInterpreter::std,   dis_std};
    s_table62[0x1] = {"stdu",  PPUInterpreter::stdu,  dis_stdu};

    // Initialize Table 63 (Extended)
    for (auto& caller : s_table63_) {
        caller = {"unknown", PPUInterpreter::unknown, dis_unknown};
    }
}
