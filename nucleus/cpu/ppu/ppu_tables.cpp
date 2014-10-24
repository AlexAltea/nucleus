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
    s_table4[0x020] = {"vmhaddshs",  PPUInterpreter::vmhaddshs,  dis_vmhaddshs};
    s_table4[0x021] = {"vmhraddshs", PPUInterpreter::vmhraddshs, dis_vmhraddshs};
    s_table4[0x022] = {"vmladduhm",  PPUInterpreter::vmladduhm,  dis_vmladduhm};
    s_table4[0x024] = {"vmsumubm",   PPUInterpreter::vmsumubm,   dis_vmsumubm};
    s_table4[0x025] = {"vmsummbm",   PPUInterpreter::vmsummbm,   dis_vmsummbm};
    s_table4[0x026] = {"vmsumuhm",   PPUInterpreter::vmsumuhm,   dis_vmsumuhm};
    s_table4[0x027] = {"vmsumuhs",   PPUInterpreter::vmsumuhs,   dis_vmsumuhs};
    s_table4[0x028] = {"vmsumshm",   PPUInterpreter::vmsumshm,   dis_vmsumshm};
    s_table4[0x029] = {"vmsumshs",   PPUInterpreter::vmsumshs,   dis_vmsumshs};
    s_table4[0x02A] = {"vsel",       PPUInterpreter::vsel,       dis_vsel};
    s_table4[0x02B] = {"vperm",      PPUInterpreter::vperm,      dis_vperm};
    s_table4[0x02C] = {"vsldoi",     PPUInterpreter::vsldoi,     dis_vsldoi};
    s_table4[0x02E] = {"vmaddfp",    PPUInterpreter::vmaddfp,    dis_vmaddfp};
    s_table4[0x02F] = {"vnmsubfp",   PPUInterpreter::vnmsubfp,   dis_vnmsubfp};

    // Initialize Table 4 (Extended)
    for (auto& caller : s_table4_) {
        caller = {"unknown", PPUInterpreter::unknown, dis_unknown};
    }
    s_table4_[0x000] = {"vaddubm",   PPUInterpreter::vaddubm,    dis_vaddubm};
    s_table4_[0x002] = {"vmaxub",    PPUInterpreter::vmaxub,     dis_vmaxub};
    s_table4_[0x004] = {"vrlb",      PPUInterpreter::vrlb,       dis_vrlb};
    s_table4_[0x006] = {"vcmpequb",  PPUInterpreter::vcmpequb,   dis_vcmpequb};
    s_table4_[0x008] = {"vmuloub",   PPUInterpreter::vmuloub,    dis_vmuloub};
    s_table4_[0x00A] = {"vaddfp",    PPUInterpreter::vaddfp,     dis_vaddfp};
    s_table4_[0x00C] = {"vmrghb",    PPUInterpreter::vmrghb,     dis_vmrghb};
    s_table4_[0x00E] = {"vpkuhum",   PPUInterpreter::vpkuhum,    dis_vpkuhum};
    s_table4_[0x040] = {"vadduhm",   PPUInterpreter::vadduhm,    dis_vadduhm};
    s_table4_[0x042] = {"vmaxuh",    PPUInterpreter::vmaxuh,     dis_vmaxuh};
    s_table4_[0x044] = {"vrlh",      PPUInterpreter::vrlh,       dis_vrlh};
    s_table4_[0x046] = {"vcmpequh",  PPUInterpreter::vcmpequh,   dis_vcmpequh};
    s_table4_[0x048] = {"vmulouh",   PPUInterpreter::vmulouh,    dis_vmulouh};
    s_table4_[0x04A] = {"vsubfp",    PPUInterpreter::vsubfp,     dis_vsubfp};
    s_table4_[0x04C] = {"vmrghh",    PPUInterpreter::vmrghh,     dis_vmrghh};
    s_table4_[0x04E] = {"vpkuwum",   PPUInterpreter::vpkuwum,    dis_vpkuwum};
    s_table4_[0x080] = {"vadduwm",   PPUInterpreter::vadduwm,    dis_vadduwm};
    s_table4_[0x082] = {"vmaxuw",    PPUInterpreter::vmaxuw,     dis_vmaxuw};
    s_table4_[0x084] = {"vrlw",      PPUInterpreter::vrlw,       dis_vrlw};
    s_table4_[0x086] = {"vcmpequw",  PPUInterpreter::vcmpequw,   dis_vcmpequw};
    s_table4_[0x08C] = {"vmrghw",    PPUInterpreter::vmrghw,     dis_vmrghw};
    s_table4_[0x08E] = {"vpkuhus",   PPUInterpreter::vpkuhus,    dis_vpkuhus};
    s_table4_[0x0C6] = {"vcmpeqfp",  PPUInterpreter::vcmpeqfp,   dis_vcmpeqfp};
    s_table4_[0x0CE] = {"vpkuwus",   PPUInterpreter::vpkuwus,    dis_vpkuwus};
    s_table4_[0x102] = {"vmaxsb",    PPUInterpreter::vmaxsb,     dis_vmaxsb};
    s_table4_[0x104] = {"vslb",      PPUInterpreter::vslb,       dis_vslb};
    s_table4_[0x108] = {"vmulosb",   PPUInterpreter::vmulosb,    dis_vmulosb};
    s_table4_[0x10A] = {"vrefp",     PPUInterpreter::vrefp,      dis_vrefp};
    s_table4_[0x10C] = {"vmrglb",    PPUInterpreter::vmrglb,     dis_vmrglb};
    s_table4_[0x10E] = {"vpkshus",   PPUInterpreter::vpkshus,    dis_vpkshus};
    s_table4_[0x142] = {"vmaxsh",    PPUInterpreter::vmaxsh,     dis_vmaxsh};
    s_table4_[0x144] = {"vslh",      PPUInterpreter::vslh,       dis_vslh};
    s_table4_[0x148] = {"vmulosh",   PPUInterpreter::vmulosh,    dis_vmulosh};
    s_table4_[0x14A] = {"vrsqrtefp", PPUInterpreter::vrsqrtefp,  dis_vrsqrtefp};
    s_table4_[0x14C] = {"vmrglh",    PPUInterpreter::vmrglh,     dis_vmrglh};
    s_table4_[0x14E] = {"vpkswus",   PPUInterpreter::vpkswus,    dis_vpkswus};
    s_table4_[0x180] = {"vaddcuw",   PPUInterpreter::vaddcuw,    dis_vaddcuw};
    s_table4_[0x182] = {"vmaxsw",    PPUInterpreter::vmaxsw,     dis_vmaxsw};
    s_table4_[0x184] = {"vslw",      PPUInterpreter::vslw,       dis_vslw};
    s_table4_[0x18A] = {"vexptefp",  PPUInterpreter::vexptefp,   dis_vexptefp};
    s_table4_[0x18C] = {"vmrglw",    PPUInterpreter::vmrglw,     dis_vmrglw};
    s_table4_[0x18E] = {"vpkshss",   PPUInterpreter::vpkshss,    dis_vpkshss};
    s_table4_[0x1C4] = {"vsl",       PPUInterpreter::vsl,        dis_vsl};
    s_table4_[0x1C6] = {"vcmpgefp",  PPUInterpreter::vcmpgefp,   dis_vcmpgefp};
    s_table4_[0x1CA] = {"vlogefp",   PPUInterpreter::vlogefp,    dis_vlogefp};
    s_table4_[0x1CE] = {"vpkswss",   PPUInterpreter::vpkswss,    dis_vpkswss};
    s_table4_[0x200] = {"vaddubs",   PPUInterpreter::vaddubs,    dis_vaddubs};
    s_table4_[0x202] = {"vminub",    PPUInterpreter::vminub,     dis_vminub};
    s_table4_[0x204] = {"vsrb",      PPUInterpreter::vsrb,       dis_vsrb};
    s_table4_[0x206] = {"vcmpgtub",  PPUInterpreter::vcmpgtub,   dis_vcmpgtub};
    s_table4_[0x208] = {"vmuleub",   PPUInterpreter::vmuleub,    dis_vmuleub};
    s_table4_[0x20A] = {"vrfin",     PPUInterpreter::vrfin,      dis_vrfin};
    s_table4_[0x20C] = {"vspltb",    PPUInterpreter::vspltb,     dis_vspltb};
    s_table4_[0x20E] = {"vupkhsb",   PPUInterpreter::vupkhsb,    dis_vupkhsb};
    s_table4_[0x240] = {"vadduhs",   PPUInterpreter::vadduhs,    dis_vadduhs};
    s_table4_[0x242] = {"vminuh",    PPUInterpreter::vminuh,     dis_vminuh};
    s_table4_[0x244] = {"vsrh",      PPUInterpreter::vsrh,       dis_vsrh};
    s_table4_[0x246] = {"vcmpgtuh",  PPUInterpreter::vcmpgtuh,   dis_vcmpgtuh};
    s_table4_[0x248] = {"vmuleuh",   PPUInterpreter::vmuleuh,    dis_vmuleuh};
    s_table4_[0x24A] = {"vrfiz",     PPUInterpreter::vrfiz,      dis_vrfiz};
    s_table4_[0x24C] = {"vsplth",    PPUInterpreter::vsplth,     dis_vsplth};
    s_table4_[0x24E] = {"vupkhsh",   PPUInterpreter::vupkhsh,    dis_vupkhsh};
    s_table4_[0x280] = {"vadduws",   PPUInterpreter::vadduws,    dis_vadduws};
    s_table4_[0x282] = {"vminuw",    PPUInterpreter::vminuw,     dis_vminuw};
    s_table4_[0x284] = {"vsrw",      PPUInterpreter::vsrw,       dis_vsrw};
    s_table4_[0x286] = {"vcmpgtuw",  PPUInterpreter::vcmpgtuw,   dis_vcmpgtuw};
    s_table4_[0x28A] = {"vrfip",     PPUInterpreter::vrfip,      dis_vrfip};
    s_table4_[0x28C] = {"vspltw",    PPUInterpreter::vspltw,     dis_vspltw};
    s_table4_[0x28E] = {"vupklsb",   PPUInterpreter::vupklsb,    dis_vupklsb};
    s_table4_[0x2C4] = {"vsr",       PPUInterpreter::vsr,        dis_vsr};
    s_table4_[0x2C6] = {"vcmpgtfp",  PPUInterpreter::vcmpgtfp,   dis_vcmpgtfp};
    s_table4_[0x2CA] = {"vrfim",     PPUInterpreter::vrfim,      dis_vrfim};
    s_table4_[0x2CE] = {"vupklsh",   PPUInterpreter::vupklsh,    dis_vupklsh};
    s_table4_[0x300] = {"vaddsbs",   PPUInterpreter::vaddsbs,    dis_vaddsbs};
    s_table4_[0x302] = {"vminsb",    PPUInterpreter::vminsb,     dis_vminsb};
    s_table4_[0x304] = {"vsrab",     PPUInterpreter::vsrab,      dis_vsrab};
    s_table4_[0x306] = {"vcmpgtsb",  PPUInterpreter::vcmpgtsb,   dis_vcmpgtsb};
    s_table4_[0x308] = {"vmulesb",   PPUInterpreter::vmulesb,    dis_vmulesb};
    s_table4_[0x30A] = {"vcfux",     PPUInterpreter::vcfux,      dis_vcfux};
    s_table4_[0x30C] = {"vspltisb",  PPUInterpreter::vspltisb,   dis_vspltisb};
    s_table4_[0x30E] = {"vpkpx",     PPUInterpreter::vpkpx,      dis_vpkpx};
    s_table4_[0x340] = {"vaddshs",   PPUInterpreter::vaddshs,    dis_vaddshs};
    s_table4_[0x342] = {"vminsh",    PPUInterpreter::vminsh,     dis_vminsh};
    s_table4_[0x344] = {"vsrah",     PPUInterpreter::vsrah,      dis_vsrah};
    s_table4_[0x346] = {"vcmpgtsh",  PPUInterpreter::vcmpgtsh,   dis_vcmpgtsh};
    s_table4_[0x348] = {"vmulesh",   PPUInterpreter::vmulesh,    dis_vmulesh};
    s_table4_[0x34A] = {"vcfsx",     PPUInterpreter::vcfsx,      dis_vcfsx};
    s_table4_[0x34C] = {"vspltish",  PPUInterpreter::vspltish,   dis_vspltish};
    s_table4_[0x34E] = {"vupkhpx",   PPUInterpreter::vupkhpx,    dis_vupkhpx};
    s_table4_[0x380] = {"vaddsws",   PPUInterpreter::vaddsws,    dis_vaddsws};
    s_table4_[0x382] = {"vminsw",    PPUInterpreter::vminsw,     dis_vminsw};
    s_table4_[0x384] = {"vsraw",     PPUInterpreter::vsraw,      dis_vsraw};
    s_table4_[0x386] = {"vcmpgtsw",  PPUInterpreter::vcmpgtsw,   dis_vcmpgtsw};
    s_table4_[0x38A] = {"vctuxs",    PPUInterpreter::vctuxs,     dis_vctuxs};
    s_table4_[0x38C] = {"vspltisw",  PPUInterpreter::vspltisw,   dis_vspltisw};
    s_table4_[0x3C6] = {"vcmpbfp",   PPUInterpreter::vcmpbfp,    dis_vcmpbfp};
    s_table4_[0x3CA] = {"vctsxs",    PPUInterpreter::vctsxs,     dis_vctsxs};
    s_table4_[0x3CE] = {"vupklpx",   PPUInterpreter::vupklpx,    dis_vupklpx};
    s_table4_[0x400] = {"vsububm",   PPUInterpreter::vsububm,    dis_vsububm};
    s_table4_[0x402] = {"vavgub",    PPUInterpreter::vavgub,     dis_vavgub};
    s_table4_[0x404] = {"vand",      PPUInterpreter::vand,       dis_vand};
    s_table4_[0x406] = {"vcmpequb_", PPUInterpreter::vcmpequb_,  dis_vcmpequb_};
    s_table4_[0x40A] = {"vmaxfp",    PPUInterpreter::vmaxfp,     dis_vmaxfp};
    s_table4_[0x40C] = {"vslo",      PPUInterpreter::vslo,       dis_vslo};
    s_table4_[0x440] = {"vsubuhm",   PPUInterpreter::vsubuhm,    dis_vsubuhm};
    s_table4_[0x442] = {"vavguh",    PPUInterpreter::vavguh,     dis_vavguh};
    s_table4_[0x444] = {"vandc",     PPUInterpreter::vandc,      dis_vandc};
    s_table4_[0x446] = {"vcmpequh_", PPUInterpreter::vcmpequh_,  dis_vcmpequh_};
    s_table4_[0x44A] = {"vminfp",    PPUInterpreter::vminfp,     dis_vminfp};
    s_table4_[0x44C] = {"vsro",      PPUInterpreter::vsro,       dis_vsro};
    s_table4_[0x480] = {"vsubuwm",   PPUInterpreter::vsubuwm,    dis_vsubuwm};
    s_table4_[0x482] = {"vavguw",    PPUInterpreter::vavguw,     dis_vavguw};
    s_table4_[0x484] = {"vor",       PPUInterpreter::vor,        dis_vor};
    s_table4_[0x486] = {"vcmpequw_", PPUInterpreter::vcmpequw_,  dis_vcmpequw_};
    s_table4_[0x4C4] = {"vxor",      PPUInterpreter::vxor,       dis_vxor};
    s_table4_[0x4C6] = {"vcmpeqfp_", PPUInterpreter::vcmpeqfp_,  dis_vcmpeqfp_};
    s_table4_[0x502] = {"vavgsb",    PPUInterpreter::vavgsb,     dis_vavgsb};
    s_table4_[0x504] = {"vnor",      PPUInterpreter::vnor,       dis_vnor};
    s_table4_[0x542] = {"vavgsh",    PPUInterpreter::vavgsh,     dis_vavgsh};
    s_table4_[0x580] = {"vsubcuw",   PPUInterpreter::vsubcuw,    dis_vsubcuw};
    s_table4_[0x582] = {"vavgsw",    PPUInterpreter::vavgsw,     dis_vavgsw};
    s_table4_[0x5C6] = {"vcmpgefp_", PPUInterpreter::vcmpgefp_,  dis_vcmpgefp_};
    s_table4_[0x600] = {"vsububs",   PPUInterpreter::vsububs,    dis_vsububs};
    s_table4_[0x604] = {"mfvscr",    PPUInterpreter::mfvscr,     dis_mfvscr};
    s_table4_[0x606] = {"vcmpgtub_", PPUInterpreter::vcmpgtub_,  dis_vcmpgtub_};
    s_table4_[0x608] = {"vsum4ubs",  PPUInterpreter::vsum4ubs,   dis_vsum4ubs};
    s_table4_[0x640] = {"vsubuhs",   PPUInterpreter::vsubuhs,    dis_vsubuhs};
    s_table4_[0x644] = {"mtvscr",    PPUInterpreter::mtvscr,     dis_mtvscr};
    s_table4_[0x646] = {"vcmpgtuh_", PPUInterpreter::vcmpgtuh_,  dis_vcmpgtuh_};
    s_table4_[0x648] = {"vsum4shs",  PPUInterpreter::vsum4shs,   dis_vsum4shs};
    s_table4_[0x680] = {"vsubuws",   PPUInterpreter::vsubuws,    dis_vsubuws};
    s_table4_[0x686] = {"vcmpgtuw_", PPUInterpreter::vcmpgtuw_,  dis_vcmpgtuw_};
    s_table4_[0x688] = {"vsum2sws",  PPUInterpreter::vsum2sws,   dis_vsum2sws};
    s_table4_[0x6C6] = {"vcmpgtfp_", PPUInterpreter::vcmpgtfp_,  dis_vcmpgtfp_};
    s_table4_[0x700] = {"vsubsbs",   PPUInterpreter::vsubsbs,    dis_vsubsbs};
    s_table4_[0x706] = {"vcmpgtsb_", PPUInterpreter::vcmpgtsb_,  dis_vcmpgtsb_};
    s_table4_[0x708] = {"vsum4sbs",  PPUInterpreter::vsum4sbs,   dis_vsum4sbs};
    s_table4_[0x740] = {"vsubshs",   PPUInterpreter::vsubshs,    dis_vsubshs};
    s_table4_[0x746] = {"vcmpgtsh_", PPUInterpreter::vcmpgtsh_,  dis_vcmpgtsh_};
    s_table4_[0x780] = {"vsubsws",   PPUInterpreter::vsubsws,    dis_vsubsws};
    s_table4_[0x786] = {"vcmpgtsw_", PPUInterpreter::vcmpgtsw_,  dis_vcmpgtsw_};
    s_table4_[0x788] = {"vsumsws",   PPUInterpreter::vsumsws,    dis_vsumsws};
    s_table4_[0x7C6] = {"vcmpbfp_",  PPUInterpreter::vcmpbfp_,   dis_vcmpbfp_};

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
    s_table31[0x090] = {"mtocrf",  PPUInterpreter::mtocrf,    dis_mtocrf};
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
    s_table31[0x13c] = {"xor",     PPUInterpreter::xorx,      dis_xorx};
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
    s_table59[0x12] = {"fdivs",    PPUInterpreter::fdivs,    dis_fdivs};
    s_table59[0x14] = {"fsubs",    PPUInterpreter::fsubs,    dis_fsubs};
    s_table59[0x15] = {"fadds",    PPUInterpreter::fadds,    dis_fadds};
    s_table59[0x16] = {"fsqrts",   PPUInterpreter::fsqrts,   dis_fsqrts};
    s_table59[0x18] = {"fres",     PPUInterpreter::fres,     dis_fres};
    s_table59[0x19] = {"fmuls",    PPUInterpreter::fmuls,    dis_fmuls};
    s_table59[0x1C] = {"fmsubs",   PPUInterpreter::fmsubs,   dis_fmsubs};
    s_table59[0x1D] = {"fmadds",   PPUInterpreter::fmadds,   dis_fmadds};
    s_table59[0x1E] = {"fnmsubs",  PPUInterpreter::fnmsubs,  dis_fnmsubs};
    s_table59[0x1F] = {"fnmadds",  PPUInterpreter::fnmadds,  dis_fnmadds};

    // Initialize Table 62
    for (auto& caller : s_table62) {
        caller = {"unknown", PPUInterpreter::unknown, dis_unknown};
    }
    s_table62[0x0] = {"std",   PPUInterpreter::std,   dis_std};
    s_table62[0x1] = {"stdu",  PPUInterpreter::stdu,  dis_stdu};

    // Initialize Table 63
    for (auto& caller : s_table63) {
        caller = {"", PPUInterpreter::callTable63_, dis_unknown};
    }
    s_table63[0x17] = {"fsel",      PPUInterpreter::fsel,    dis_fsel};
    s_table63[0x19] = {"fmul",      PPUInterpreter::fmul,    dis_fmul};
    s_table63[0x1C] = {"fmsub",     PPUInterpreter::fmsub,   dis_fmsub};
    s_table63[0x1D] = {"fmadd",     PPUInterpreter::fmadd,   dis_fmadd};
    s_table63[0x1E] = {"fnmsub",    PPUInterpreter::fnmsub,  dis_fnmsub};
    s_table63[0x1F] = {"fnmadd",    PPUInterpreter::fnmadd,  dis_fnmadd};

    // Initialize Table 63 (Extended)
    for (auto& caller : s_table63_) {
        caller = {"unknown", PPUInterpreter::unknown, dis_unknown};
    }
    s_table63_[0x000] = {"fcmpu",   PPUInterpreter::fcmpu,   dis_fcmpu};
    s_table63_[0x00C] = {"frsp",    PPUInterpreter::frsp,    dis_frsp};
    s_table63_[0x00E] = {"fctiw",   PPUInterpreter::fctiw,   dis_fctiw};
    s_table63_[0x00F] = {"fctiwz",  PPUInterpreter::fctiwz,  dis_fctiwz};
    s_table63_[0x012] = {"fdiv",    PPUInterpreter::fdiv,    dis_fdiv};
    s_table63_[0x014] = {"fsub",    PPUInterpreter::fsub,    dis_fsub};
    s_table63_[0x015] = {"fadd",    PPUInterpreter::fadd,    dis_fadd};
    s_table63_[0x016] = {"fsqrt",   PPUInterpreter::fsqrt,   dis_fsqrt};
    s_table63_[0x01A] = {"frsqrte", PPUInterpreter::frsqrte, dis_frsqrte};
    s_table63_[0x020] = {"fcmpo",   PPUInterpreter::fcmpo,   dis_fcmpo};
    s_table63_[0x026] = {"mtfsb1",  PPUInterpreter::mtfsb1,  dis_mtfsb1};
    s_table63_[0x028] = {"fneg",    PPUInterpreter::fneg,    dis_fneg};
    s_table63_[0x040] = {"mcrfs",   PPUInterpreter::mcrfs,   dis_mcrfs};
    s_table63_[0x046] = {"mtfsb0",  PPUInterpreter::mtfsb0,  dis_mtfsb0};
    s_table63_[0x048] = {"fmr",     PPUInterpreter::fmr,     dis_fmr};
    s_table63_[0x086] = {"mtfsfi",  PPUInterpreter::mtfsfi,  dis_mtfsfi};
    s_table63_[0x088] = {"fnabs",   PPUInterpreter::fnabs,   dis_fnabs};
    s_table63_[0x108] = {"fabs",    PPUInterpreter::fabs,    dis_fabs};
    s_table63_[0x247] = {"mffs",    PPUInterpreter::mffs,    dis_mffs};
    s_table63_[0x2C7] = {"mtfsf",   PPUInterpreter::mtfsf,   dis_mtfsf};
    s_table63_[0x32E] = {"fctid",   PPUInterpreter::fctid,   dis_fctid};
    s_table63_[0x32F] = {"fctidz",  PPUInterpreter::fctidz,  dis_fctidz};
    s_table63_[0x34E] = {"fcfid",   PPUInterpreter::fcfid,   dis_fcfid};
}
