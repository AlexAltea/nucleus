/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_tables.h"
#include "nucleus/cpu/ppu/interpreter/ppu_interpreter.h"
#include "nucleus/cpu/ppu/ppu_thread.h"

namespace cpu {
namespace ppu {

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
        caller = {"unknown", PPUInterpreter::unknown};
    }
    s_tablePrimary[0x04] = {"",       PPUInterpreter::callTable4};
    s_tablePrimary[0x13] = {"",       PPUInterpreter::callTable19};
    s_tablePrimary[0x1e] = {"",       PPUInterpreter::callTable30};
    s_tablePrimary[0x1f] = {"",       PPUInterpreter::callTable31};
    s_tablePrimary[0x3a] = {"",       PPUInterpreter::callTable58};
    s_tablePrimary[0x3b] = {"",       PPUInterpreter::callTable59};
    s_tablePrimary[0x3e] = {"",       PPUInterpreter::callTable62};
    s_tablePrimary[0x3f] = {"",       PPUInterpreter::callTable63};
    s_tablePrimary[0x02] = {"tdi",    PPUInterpreter::tdi};
    s_tablePrimary[0x03] = {"twi",    PPUInterpreter::twi};
    s_tablePrimary[0x07] = {"mulli",  PPUInterpreter::mulli};
    s_tablePrimary[0x08] = {"subfic", PPUInterpreter::subfic};
    s_tablePrimary[0x0a] = {"cmpli",  PPUInterpreter::cmpli};
    s_tablePrimary[0x0b] = {"cmpi",   PPUInterpreter::cmpi};
    s_tablePrimary[0x0c] = {"addic",  PPUInterpreter::addic};
    s_tablePrimary[0x0d] = {"addic_", PPUInterpreter::addic_};
    s_tablePrimary[0x0e] = {"addi",   PPUInterpreter::addi};
    s_tablePrimary[0x0f] = {"addis",  PPUInterpreter::addis};
    s_tablePrimary[0x10] = {"bc",     PPUInterpreter::bc};
    s_tablePrimary[0x11] = {"sc",     PPUInterpreter::sc};
    s_tablePrimary[0x12] = {"b",      PPUInterpreter::b};
    s_tablePrimary[0x14] = {"rlwimi", PPUInterpreter::rlwimix};
    s_tablePrimary[0x15] = {"rlwinm", PPUInterpreter::rlwinmx};
    s_tablePrimary[0x17] = {"rlwnm",  PPUInterpreter::rlwnmx};
    s_tablePrimary[0x18] = {"ori",    PPUInterpreter::ori};
    s_tablePrimary[0x19] = {"oris",   PPUInterpreter::oris};
    s_tablePrimary[0x1a] = {"xori",   PPUInterpreter::xori};
    s_tablePrimary[0x1b] = {"xoris",  PPUInterpreter::xoris};
    s_tablePrimary[0x1c] = {"andi_",  PPUInterpreter::andi_};
    s_tablePrimary[0x1d] = {"andis_", PPUInterpreter::andis_};
    s_tablePrimary[0x20] = {"lwz",    PPUInterpreter::lwz};
    s_tablePrimary[0x21] = {"lwzu",   PPUInterpreter::lwzu};
    s_tablePrimary[0x22] = {"lbz",    PPUInterpreter::lbz};
    s_tablePrimary[0x23] = {"lbzu",   PPUInterpreter::lbzu};
    s_tablePrimary[0x24] = {"stw",    PPUInterpreter::stw};
    s_tablePrimary[0x25] = {"stwu",   PPUInterpreter::stwu};
    s_tablePrimary[0x26] = {"stb",    PPUInterpreter::stb};
    s_tablePrimary[0x27] = {"stbu",   PPUInterpreter::stbu};
    s_tablePrimary[0x28] = {"lhz",    PPUInterpreter::lhz};
    s_tablePrimary[0x29] = {"lhzu",   PPUInterpreter::lhzu};
    s_tablePrimary[0x2a] = {"lha",    PPUInterpreter::lha};
    s_tablePrimary[0x2b] = {"lhau",   PPUInterpreter::lhau};
    s_tablePrimary[0x2c] = {"sth",    PPUInterpreter::sth};
    s_tablePrimary[0x2d] = {"sthu",   PPUInterpreter::sthu};
    s_tablePrimary[0x2e] = {"lmw",    PPUInterpreter::lmw};
    s_tablePrimary[0x2f] = {"stmw",   PPUInterpreter::stmw};
    s_tablePrimary[0x30] = {"lfs",    PPUInterpreter::lfs};
    s_tablePrimary[0x31] = {"lfsu",   PPUInterpreter::lfsu};
    s_tablePrimary[0x32] = {"lfd",    PPUInterpreter::lfd};
    s_tablePrimary[0x33] = {"lfdu",   PPUInterpreter::lfdu};
    s_tablePrimary[0x34] = {"stfs",   PPUInterpreter::stfs};
    s_tablePrimary[0x35] = {"stfsu",  PPUInterpreter::stfsu};
    s_tablePrimary[0x36] = {"stfd",   PPUInterpreter::stfd};
    s_tablePrimary[0x37] = {"stfdu",  PPUInterpreter::stfdu};

    // Initialize Table 4
    for (auto& caller : s_table4) {
        caller = {"", PPUInterpreter::callTable4_};
    }
    s_table4[0x020] = {"vmhaddshs",  PPUInterpreter::vmhaddshs};
    s_table4[0x021] = {"vmhraddshs", PPUInterpreter::vmhraddshs};
    s_table4[0x022] = {"vmladduhm",  PPUInterpreter::vmladduhm};
    s_table4[0x024] = {"vmsumubm",   PPUInterpreter::vmsumubm};
    s_table4[0x025] = {"vmsummbm",   PPUInterpreter::vmsummbm};
    s_table4[0x026] = {"vmsumuhm",   PPUInterpreter::vmsumuhm};
    s_table4[0x027] = {"vmsumuhs",   PPUInterpreter::vmsumuhs};
    s_table4[0x028] = {"vmsumshm",   PPUInterpreter::vmsumshm};
    s_table4[0x029] = {"vmsumshs",   PPUInterpreter::vmsumshs};
    s_table4[0x02A] = {"vsel",       PPUInterpreter::vsel};
    s_table4[0x02B] = {"vperm",      PPUInterpreter::vperm};
    s_table4[0x02C] = {"vsldoi",     PPUInterpreter::vsldoi};
    s_table4[0x02E] = {"vmaddfp",    PPUInterpreter::vmaddfp};
    s_table4[0x02F] = {"vnmsubfp",   PPUInterpreter::vnmsubfp};

    // Initialize Table 4 (Extended)
    for (auto& caller : s_table4_) {
        caller = {"unknown", PPUInterpreter::unknown};
    }
    s_table4_[0x000] = {"vaddubm",   PPUInterpreter::vaddubm};
    s_table4_[0x002] = {"vmaxub",    PPUInterpreter::vmaxub};
    s_table4_[0x004] = {"vrlb",      PPUInterpreter::vrlb};
    s_table4_[0x006] = {"vcmpequb",  PPUInterpreter::vcmpequb};
    s_table4_[0x008] = {"vmuloub",   PPUInterpreter::vmuloub};
    s_table4_[0x00A] = {"vaddfp",    PPUInterpreter::vaddfp};
    s_table4_[0x00C] = {"vmrghb",    PPUInterpreter::vmrghb};
    s_table4_[0x00E] = {"vpkuhum",   PPUInterpreter::vpkuhum};
    s_table4_[0x040] = {"vadduhm",   PPUInterpreter::vadduhm};
    s_table4_[0x042] = {"vmaxuh",    PPUInterpreter::vmaxuh};
    s_table4_[0x044] = {"vrlh",      PPUInterpreter::vrlh};
    s_table4_[0x046] = {"vcmpequh",  PPUInterpreter::vcmpequh};
    s_table4_[0x048] = {"vmulouh",   PPUInterpreter::vmulouh};
    s_table4_[0x04A] = {"vsubfp",    PPUInterpreter::vsubfp};
    s_table4_[0x04C] = {"vmrghh",    PPUInterpreter::vmrghh};
    s_table4_[0x04E] = {"vpkuwum",   PPUInterpreter::vpkuwum};
    s_table4_[0x080] = {"vadduwm",   PPUInterpreter::vadduwm};
    s_table4_[0x082] = {"vmaxuw",    PPUInterpreter::vmaxuw};
    s_table4_[0x084] = {"vrlw",      PPUInterpreter::vrlw};
    s_table4_[0x086] = {"vcmpequw",  PPUInterpreter::vcmpequw};
    s_table4_[0x08C] = {"vmrghw",    PPUInterpreter::vmrghw};
    s_table4_[0x08E] = {"vpkuhus",   PPUInterpreter::vpkuhus};
    s_table4_[0x0C6] = {"vcmpeqfp",  PPUInterpreter::vcmpeqfp};
    s_table4_[0x0CE] = {"vpkuwus",   PPUInterpreter::vpkuwus};
    s_table4_[0x102] = {"vmaxsb",    PPUInterpreter::vmaxsb};
    s_table4_[0x104] = {"vslb",      PPUInterpreter::vslb};
    s_table4_[0x108] = {"vmulosb",   PPUInterpreter::vmulosb};
    s_table4_[0x10A] = {"vrefp",     PPUInterpreter::vrefp};
    s_table4_[0x10C] = {"vmrglb",    PPUInterpreter::vmrglb};
    s_table4_[0x10E] = {"vpkshus",   PPUInterpreter::vpkshus};
    s_table4_[0x142] = {"vmaxsh",    PPUInterpreter::vmaxsh};
    s_table4_[0x144] = {"vslh",      PPUInterpreter::vslh};
    s_table4_[0x148] = {"vmulosh",   PPUInterpreter::vmulosh};
    s_table4_[0x14A] = {"vrsqrtefp", PPUInterpreter::vrsqrtefp};
    s_table4_[0x14C] = {"vmrglh",    PPUInterpreter::vmrglh};
    s_table4_[0x14E] = {"vpkswus",   PPUInterpreter::vpkswus};
    s_table4_[0x180] = {"vaddcuw",   PPUInterpreter::vaddcuw};
    s_table4_[0x182] = {"vmaxsw",    PPUInterpreter::vmaxsw};
    s_table4_[0x184] = {"vslw",      PPUInterpreter::vslw};
    s_table4_[0x18A] = {"vexptefp",  PPUInterpreter::vexptefp};
    s_table4_[0x18C] = {"vmrglw",    PPUInterpreter::vmrglw};
    s_table4_[0x18E] = {"vpkshss",   PPUInterpreter::vpkshss};
    s_table4_[0x1C4] = {"vsl",       PPUInterpreter::vsl};
    s_table4_[0x1C6] = {"vcmpgefp",  PPUInterpreter::vcmpgefp};
    s_table4_[0x1CA] = {"vlogefp",   PPUInterpreter::vlogefp};
    s_table4_[0x1CE] = {"vpkswss",   PPUInterpreter::vpkswss};
    s_table4_[0x200] = {"vaddubs",   PPUInterpreter::vaddubs};
    s_table4_[0x202] = {"vminub",    PPUInterpreter::vminub};
    s_table4_[0x204] = {"vsrb",      PPUInterpreter::vsrb};
    s_table4_[0x206] = {"vcmpgtub",  PPUInterpreter::vcmpgtub};
    s_table4_[0x208] = {"vmuleub",   PPUInterpreter::vmuleub};
    s_table4_[0x20A] = {"vrfin",     PPUInterpreter::vrfin};
    s_table4_[0x20C] = {"vspltb",    PPUInterpreter::vspltb};
    s_table4_[0x20E] = {"vupkhsb",   PPUInterpreter::vupkhsb};
    s_table4_[0x240] = {"vadduhs",   PPUInterpreter::vadduhs};
    s_table4_[0x242] = {"vminuh",    PPUInterpreter::vminuh};
    s_table4_[0x244] = {"vsrh",      PPUInterpreter::vsrh};
    s_table4_[0x246] = {"vcmpgtuh",  PPUInterpreter::vcmpgtuh};
    s_table4_[0x248] = {"vmuleuh",   PPUInterpreter::vmuleuh};
    s_table4_[0x24A] = {"vrfiz",     PPUInterpreter::vrfiz};
    s_table4_[0x24C] = {"vsplth",    PPUInterpreter::vsplth};
    s_table4_[0x24E] = {"vupkhsh",   PPUInterpreter::vupkhsh};
    s_table4_[0x280] = {"vadduws",   PPUInterpreter::vadduws};
    s_table4_[0x282] = {"vminuw",    PPUInterpreter::vminuw};
    s_table4_[0x284] = {"vsrw",      PPUInterpreter::vsrw};
    s_table4_[0x286] = {"vcmpgtuw",  PPUInterpreter::vcmpgtuw};
    s_table4_[0x28A] = {"vrfip",     PPUInterpreter::vrfip};
    s_table4_[0x28C] = {"vspltw",    PPUInterpreter::vspltw};
    s_table4_[0x28E] = {"vupklsb",   PPUInterpreter::vupklsb};
    s_table4_[0x2C4] = {"vsr",       PPUInterpreter::vsr};
    s_table4_[0x2C6] = {"vcmpgtfp",  PPUInterpreter::vcmpgtfp};
    s_table4_[0x2CA] = {"vrfim",     PPUInterpreter::vrfim};
    s_table4_[0x2CE] = {"vupklsh",   PPUInterpreter::vupklsh};
    s_table4_[0x300] = {"vaddsbs",   PPUInterpreter::vaddsbs};
    s_table4_[0x302] = {"vminsb",    PPUInterpreter::vminsb};
    s_table4_[0x304] = {"vsrab",     PPUInterpreter::vsrab};
    s_table4_[0x306] = {"vcmpgtsb",  PPUInterpreter::vcmpgtsb};
    s_table4_[0x308] = {"vmulesb",   PPUInterpreter::vmulesb};
    s_table4_[0x30A] = {"vcfux",     PPUInterpreter::vcfux};
    s_table4_[0x30C] = {"vspltisb",  PPUInterpreter::vspltisb};
    s_table4_[0x30E] = {"vpkpx",     PPUInterpreter::vpkpx};
    s_table4_[0x340] = {"vaddshs",   PPUInterpreter::vaddshs};
    s_table4_[0x342] = {"vminsh",    PPUInterpreter::vminsh};
    s_table4_[0x344] = {"vsrah",     PPUInterpreter::vsrah};
    s_table4_[0x346] = {"vcmpgtsh",  PPUInterpreter::vcmpgtsh};
    s_table4_[0x348] = {"vmulesh",   PPUInterpreter::vmulesh};
    s_table4_[0x34A] = {"vcfsx",     PPUInterpreter::vcfsx};
    s_table4_[0x34C] = {"vspltish",  PPUInterpreter::vspltish};
    s_table4_[0x34E] = {"vupkhpx",   PPUInterpreter::vupkhpx};
    s_table4_[0x380] = {"vaddsws",   PPUInterpreter::vaddsws};
    s_table4_[0x382] = {"vminsw",    PPUInterpreter::vminsw};
    s_table4_[0x384] = {"vsraw",     PPUInterpreter::vsraw};
    s_table4_[0x386] = {"vcmpgtsw",  PPUInterpreter::vcmpgtsw};
    s_table4_[0x38A] = {"vctuxs",    PPUInterpreter::vctuxs};
    s_table4_[0x38C] = {"vspltisw",  PPUInterpreter::vspltisw};
    s_table4_[0x3C6] = {"vcmpbfp",   PPUInterpreter::vcmpbfp};
    s_table4_[0x3CA] = {"vctsxs",    PPUInterpreter::vctsxs};
    s_table4_[0x3CE] = {"vupklpx",   PPUInterpreter::vupklpx};
    s_table4_[0x400] = {"vsububm",   PPUInterpreter::vsububm};
    s_table4_[0x402] = {"vavgub",    PPUInterpreter::vavgub};
    s_table4_[0x404] = {"vand",      PPUInterpreter::vand};
    s_table4_[0x406] = {"vcmpequb_", PPUInterpreter::vcmpequb_};
    s_table4_[0x40A] = {"vmaxfp",    PPUInterpreter::vmaxfp};
    s_table4_[0x40C] = {"vslo",      PPUInterpreter::vslo};
    s_table4_[0x440] = {"vsubuhm",   PPUInterpreter::vsubuhm};
    s_table4_[0x442] = {"vavguh",    PPUInterpreter::vavguh};
    s_table4_[0x444] = {"vandc",     PPUInterpreter::vandc};
    s_table4_[0x446] = {"vcmpequh_", PPUInterpreter::vcmpequh_};
    s_table4_[0x44A] = {"vminfp",    PPUInterpreter::vminfp};
    s_table4_[0x44C] = {"vsro",      PPUInterpreter::vsro};
    s_table4_[0x480] = {"vsubuwm",   PPUInterpreter::vsubuwm};
    s_table4_[0x482] = {"vavguw",    PPUInterpreter::vavguw};
    s_table4_[0x484] = {"vor",       PPUInterpreter::vor};
    s_table4_[0x486] = {"vcmpequw_", PPUInterpreter::vcmpequw_};
    s_table4_[0x4C4] = {"vxor",      PPUInterpreter::vxor};
    s_table4_[0x4C6] = {"vcmpeqfp_", PPUInterpreter::vcmpeqfp_};
    s_table4_[0x502] = {"vavgsb",    PPUInterpreter::vavgsb};
    s_table4_[0x504] = {"vnor",      PPUInterpreter::vnor};
    s_table4_[0x542] = {"vavgsh",    PPUInterpreter::vavgsh};
    s_table4_[0x580] = {"vsubcuw",   PPUInterpreter::vsubcuw};
    s_table4_[0x582] = {"vavgsw",    PPUInterpreter::vavgsw};
    s_table4_[0x5C6] = {"vcmpgefp_", PPUInterpreter::vcmpgefp_};
    s_table4_[0x600] = {"vsububs",   PPUInterpreter::vsububs};
    s_table4_[0x604] = {"mfvscr",    PPUInterpreter::mfvscr};
    s_table4_[0x606] = {"vcmpgtub_", PPUInterpreter::vcmpgtub_};
    s_table4_[0x608] = {"vsum4ubs",  PPUInterpreter::vsum4ubs};
    s_table4_[0x640] = {"vsubuhs",   PPUInterpreter::vsubuhs};
    s_table4_[0x644] = {"mtvscr",    PPUInterpreter::mtvscr};
    s_table4_[0x646] = {"vcmpgtuh_", PPUInterpreter::vcmpgtuh_};
    s_table4_[0x648] = {"vsum4shs",  PPUInterpreter::vsum4shs};
    s_table4_[0x680] = {"vsubuws",   PPUInterpreter::vsubuws};
    s_table4_[0x686] = {"vcmpgtuw_", PPUInterpreter::vcmpgtuw_};
    s_table4_[0x688] = {"vsum2sws",  PPUInterpreter::vsum2sws};
    s_table4_[0x6C6] = {"vcmpgtfp_", PPUInterpreter::vcmpgtfp_};
    s_table4_[0x700] = {"vsubsbs",   PPUInterpreter::vsubsbs};
    s_table4_[0x706] = {"vcmpgtsb_", PPUInterpreter::vcmpgtsb_};
    s_table4_[0x708] = {"vsum4sbs",  PPUInterpreter::vsum4sbs};
    s_table4_[0x740] = {"vsubshs",   PPUInterpreter::vsubshs};
    s_table4_[0x746] = {"vcmpgtsh_", PPUInterpreter::vcmpgtsh_};
    s_table4_[0x780] = {"vsubsws",   PPUInterpreter::vsubsws};
    s_table4_[0x786] = {"vcmpgtsw_", PPUInterpreter::vcmpgtsw_};
    s_table4_[0x788] = {"vsumsws",   PPUInterpreter::vsumsws};
    s_table4_[0x7C6] = {"vcmpbfp_",  PPUInterpreter::vcmpbfp_};

    // Initialize Table 19
    for (auto& caller : s_table19) {
        caller = {"unknown", PPUInterpreter::unknown};
    }
    s_table19[0x000] = {"mcrf",     PPUInterpreter::mcrf};
    s_table19[0x010] = {"bclr",     PPUInterpreter::bclr};
    s_table19[0x021] = {"crnor",    PPUInterpreter::crnor};
    s_table19[0x081] = {"crandc",   PPUInterpreter::crandc};
    s_table19[0x096] = {"isync",    PPUInterpreter::isync};
    s_table19[0x0c1] = {"crxor",    PPUInterpreter::crxor};
    s_table19[0x0e1] = {"crnand",   PPUInterpreter::crnand};
    s_table19[0x101] = {"crand",    PPUInterpreter::crand};
    s_table19[0x121] = {"creqv",    PPUInterpreter::creqv};
    s_table19[0x1a1] = {"crorc",    PPUInterpreter::crorc};
    s_table19[0x1c1] = {"cror",     PPUInterpreter::cror};
    s_table19[0x210] = {"bcctr",    PPUInterpreter::bcctr};

    // Initialize Table 30
    for (auto& caller : s_table30) {
        caller = {"unknown", PPUInterpreter::unknown};
    }
    s_table30[0x0] = {"rldicl",    PPUInterpreter::rldiclx};
    s_table30[0x1] = {"rldicr",    PPUInterpreter::rldicrx};
    s_table30[0x2] = {"rldic",     PPUInterpreter::rldicx};
    s_table30[0x3] = {"rldimi",    PPUInterpreter::rldimix};
    s_table30[0x4] = {"rldc_lr",   PPUInterpreter::rldc_lr};

    // Initialize Table 31
    for (auto& caller : s_table31) {
        caller = {"unknown", PPUInterpreter::unknown};
    }
    s_table31[0x000] = {"cmp",     PPUInterpreter::cmp};
    s_table31[0x004] = {"tw",      PPUInterpreter::tw};
    s_table31[0x006] = {"lvsl",    PPUInterpreter::lvsl};
    s_table31[0x007] = {"lvebx",   PPUInterpreter::lvebx};
    s_table31[0x008] = {"subfc",   PPUInterpreter::subfc};
    s_table31[0x009] = {"mulhdu",  PPUInterpreter::mulhdu};
    s_table31[0x00A] = {"addc",    PPUInterpreter::addc};
    s_table31[0x00B] = {"mulhwu",  PPUInterpreter::mulhwu};
    s_table31[0x013] = {"mfocrf",  PPUInterpreter::mfocrf};
    s_table31[0x014] = {"lwarx",   PPUInterpreter::lwarx};
    s_table31[0x015] = {"ldx",     PPUInterpreter::ldx};
    s_table31[0x017] = {"lwzx",    PPUInterpreter::lwzx};
    s_table31[0x018] = {"slw",     PPUInterpreter::slw};
    s_table31[0x01A] = {"cntlzw",  PPUInterpreter::cntlzw};
    s_table31[0x01B] = {"sld",     PPUInterpreter::sld};
    s_table31[0x01C] = {"and",     PPUInterpreter::andx};
    s_table31[0x020] = {"cmpl",    PPUInterpreter::cmpl};
    s_table31[0x026] = {"lvsr",    PPUInterpreter::lvsr};
    s_table31[0x027] = {"lvehx",   PPUInterpreter::lvehx};
    s_table31[0x028] = {"subf",    PPUInterpreter::subf};
    s_table31[0x035] = {"ldux",    PPUInterpreter::ldux};
    s_table31[0x036] = {"dcbst",   PPUInterpreter::dcbst};
    s_table31[0x037] = {"lwzux",   PPUInterpreter::lwzux};
    s_table31[0x03A] = {"cntlzd",  PPUInterpreter::cntlzd};
    s_table31[0x03C] = {"andc",    PPUInterpreter::andc};
    s_table31[0x044] = {"td",      PPUInterpreter::td};
    s_table31[0x047] = {"lvewx",   PPUInterpreter::lvewx};
    s_table31[0x049] = {"mulhd",   PPUInterpreter::mulhd};
    s_table31[0x04B] = {"mulhw",   PPUInterpreter::mulhw};
    s_table31[0x054] = {"ldarx",   PPUInterpreter::ldarx};
    s_table31[0x056] = {"dcbf",    PPUInterpreter::dcbf};
    s_table31[0x057] = {"lbzx",    PPUInterpreter::lbzx};
    s_table31[0x067] = {"lvx",     PPUInterpreter::lvx};
    s_table31[0x068] = {"neg",     PPUInterpreter::neg};
    s_table31[0x077] = {"lbzux",   PPUInterpreter::lbzux};
    s_table31[0x07C] = {"nor",     PPUInterpreter::nor};
    s_table31[0x087] = {"stvebx",  PPUInterpreter::stvebx};
    s_table31[0x088] = {"subfe",   PPUInterpreter::subfe};
    s_table31[0x08A] = {"adde",    PPUInterpreter::adde};
    s_table31[0x090] = {"mtocrf",  PPUInterpreter::mtocrf};
    s_table31[0x095] = {"stdx",    PPUInterpreter::stdx};
    s_table31[0x096] = {"stwcx_",  PPUInterpreter::stwcx_};
    s_table31[0x097] = {"stwx",    PPUInterpreter::stwx};
    s_table31[0x0A7] = {"stvehx",  PPUInterpreter::stvehx};
    s_table31[0x0B5] = {"stdux",   PPUInterpreter::stdux};
    s_table31[0x0B7] = {"stwux",   PPUInterpreter::stwux};
    s_table31[0x0C7] = {"stvewx",  PPUInterpreter::stvewx};
    s_table31[0x0C8] = {"subfze",  PPUInterpreter::subfze};
    s_table31[0x0CA] = {"addze",   PPUInterpreter::addze};
    s_table31[0x0D6] = {"stdcx_",  PPUInterpreter::stdcx_};
    s_table31[0x0D7] = {"stbx",    PPUInterpreter::stbx};
    s_table31[0x0E7] = {"stvx",    PPUInterpreter::stvx};
    s_table31[0x0E8] = {"subfme",  PPUInterpreter::subfme};
    s_table31[0x0E9] = {"mulld",   PPUInterpreter::mulld};
    s_table31[0x0EA] = {"addme",   PPUInterpreter::addme};
    s_table31[0x0EB] = {"mullw",   PPUInterpreter::mullw};
    s_table31[0x0F6] = {"dcbtst",  PPUInterpreter::dcbtst};
    s_table31[0x0F7] = {"stbux",   PPUInterpreter::stbux};
    s_table31[0x10A] = {"add",     PPUInterpreter::add};
    s_table31[0x116] = {"dcbt",    PPUInterpreter::dcbt};
    s_table31[0x117] = {"lhzx",    PPUInterpreter::lhzx};
    s_table31[0x11C] = {"eqv",     PPUInterpreter::eqv};
    s_table31[0x136] = {"eciwx",   PPUInterpreter::eciwx};
    s_table31[0x137] = {"lhzux",   PPUInterpreter::lhzux};
    s_table31[0x13C] = {"xor",     PPUInterpreter::xorx};
    s_table31[0x153] = {"mfspr",   PPUInterpreter::mfspr};
    s_table31[0x155] = {"lwax",    PPUInterpreter::lwax};
    s_table31[0x156] = {"dst",     PPUInterpreter::dst};
    s_table31[0x157] = {"lhax",    PPUInterpreter::lhax};
    s_table31[0x167] = {"lvxl",    PPUInterpreter::lvxl};
    s_table31[0x173] = {"mftb",    PPUInterpreter::mftb};
    s_table31[0x175] = {"lwaux",   PPUInterpreter::lwaux};
    s_table31[0x176] = {"dstst",   PPUInterpreter::dstst};
    s_table31[0x177] = {"lhaux",   PPUInterpreter::lhaux};
    s_table31[0x197] = {"sthx",    PPUInterpreter::sthx};
    s_table31[0x19C] = {"orc",     PPUInterpreter::orc};
    s_table31[0x1B6] = {"ecowx",   PPUInterpreter::ecowx};
    s_table31[0x1B7] = {"sthux",   PPUInterpreter::sthux};
    s_table31[0x1BC] = {"or",      PPUInterpreter::orx};
    s_table31[0x1C9] = {"divdu",   PPUInterpreter::divdu};
    s_table31[0x1CB] = {"divwu",   PPUInterpreter::divwu};
    s_table31[0x1D3] = {"mtspr",   PPUInterpreter::mtspr};
    s_table31[0x1DC] = {"nand",    PPUInterpreter::nand};
    s_table31[0x1E7] = {"stvxl",   PPUInterpreter::stvxl};
    s_table31[0x1E9] = {"divd",    PPUInterpreter::divd};
    s_table31[0x1EB] = {"divw",    PPUInterpreter::divw};
    s_table31[0x207] = {"lvlx",    PPUInterpreter::lvlx};
    s_table31[0x214] = {"ldbrx",   PPUInterpreter::ldbrx};
    s_table31[0x215] = {"lswx",    PPUInterpreter::lswx};
    s_table31[0x216] = {"lwbrx",   PPUInterpreter::lwbrx};
    s_table31[0x217] = {"lfsx",    PPUInterpreter::lfsx};
    s_table31[0x218] = {"srw",     PPUInterpreter::srw};
    s_table31[0x21B] = {"srd",     PPUInterpreter::srd};
    s_table31[0x227] = {"lvrx",    PPUInterpreter::lvrx};
    s_table31[0x237] = {"lfsux",   PPUInterpreter::lfsux};
    s_table31[0x255] = {"lswi",    PPUInterpreter::lswi};
    s_table31[0x256] = {"sync",    PPUInterpreter::sync};
    s_table31[0x257] = {"lfdx",    PPUInterpreter::lfdx};
    s_table31[0x277] = {"lfdux",   PPUInterpreter::lfdux};
    s_table31[0x287] = {"stvlx",   PPUInterpreter::stvlx};
    s_table31[0x295] = {"stswx",   PPUInterpreter::stswx};
    s_table31[0x296] = {"stwbrx",  PPUInterpreter::stwbrx};
    s_table31[0x297] = {"stfsx",   PPUInterpreter::stfsx};
    s_table31[0x2A7] = {"stvrx",   PPUInterpreter::stvrx};
    s_table31[0x2B7] = {"stfsux",  PPUInterpreter::stfsux};
    s_table31[0x2D5] = {"stswi",   PPUInterpreter::stswi};
    s_table31[0x2D7] = {"stfdx",   PPUInterpreter::stfdx};
    s_table31[0x2F7] = {"stfdux",  PPUInterpreter::stfdux};
    s_table31[0x307] = {"lvlxl",   PPUInterpreter::lvlxl};
    s_table31[0x316] = {"lhbrx",   PPUInterpreter::lhbrx};
    s_table31[0x318] = {"sraw",    PPUInterpreter::sraw};
    s_table31[0x31A] = {"srad",    PPUInterpreter::srad};
    s_table31[0x327] = {"lvrxl",   PPUInterpreter::lvrxl};
    s_table31[0x336] = {"dss",     PPUInterpreter::dss};
    s_table31[0x338] = {"srawi",   PPUInterpreter::srawi};
    s_table31[0x33A] = {"sradi",   PPUInterpreter::sradi};
    s_table31[0x33B] = {"sradi",   PPUInterpreter::sradi};
    s_table31[0x356] = {"eieio",   PPUInterpreter::eieio};
    s_table31[0x387] = {"stvlxl",  PPUInterpreter::stvlxl};
    s_table31[0x396] = {"sthbrx",  PPUInterpreter::sthbrx};
    s_table31[0x39A] = {"extsh",   PPUInterpreter::extsh};
    s_table31[0x387] = {"stvrxl",  PPUInterpreter::stvrxl};
    s_table31[0x3BA] = {"extsb",   PPUInterpreter::extsb};
    s_table31[0x3D7] = {"stfiwx",  PPUInterpreter::stfiwx};
    s_table31[0x3DA] = {"extsw",   PPUInterpreter::extsw};
    s_table31[0x3D6] = {"icbi",    PPUInterpreter::icbi};
    s_table31[0x3F6] = {"dcbz",    PPUInterpreter::dcbz};

    // Initialize Table 58
    for (auto& caller : s_table58) {
        caller = {"unknown", PPUInterpreter::unknown};
    }
    s_table58[0x0] = {"ld",    PPUInterpreter::ld};
    s_table58[0x1] = {"ldu",   PPUInterpreter::ldu};
    s_table58[0x2] = {"lwa",   PPUInterpreter::lwa};

    // Initialize Table 59
    for (auto& caller : s_table59) {
        caller = {"unknown", PPUInterpreter::unknown};
    }
    s_table59[0x12] = {"fdivs",    PPUInterpreter::fdivs};
    s_table59[0x14] = {"fsubs",    PPUInterpreter::fsubs};
    s_table59[0x15] = {"fadds",    PPUInterpreter::fadds};
    s_table59[0x16] = {"fsqrts",   PPUInterpreter::fsqrts};
    s_table59[0x18] = {"fres",     PPUInterpreter::fres};
    s_table59[0x19] = {"fmuls",    PPUInterpreter::fmuls};
    s_table59[0x1C] = {"fmsubs",   PPUInterpreter::fmsubs};
    s_table59[0x1D] = {"fmadds",   PPUInterpreter::fmadds};
    s_table59[0x1E] = {"fnmsubs",  PPUInterpreter::fnmsubs};
    s_table59[0x1F] = {"fnmadds",  PPUInterpreter::fnmadds};

    // Initialize Table 62
    for (auto& caller : s_table62) {
        caller = {"unknown", PPUInterpreter::unknown};
    }
    s_table62[0x0] = {"std",   PPUInterpreter::std};
    s_table62[0x1] = {"stdu",  PPUInterpreter::stdu};

    // Initialize Table 63
    for (auto& caller : s_table63) {
        caller = {"", PPUInterpreter::callTable63_};
    }
    s_table63[0x17] = {"fsel",      PPUInterpreter::fsel};
    s_table63[0x19] = {"fmul",      PPUInterpreter::fmul};
    s_table63[0x1C] = {"fmsub",     PPUInterpreter::fmsub};
    s_table63[0x1D] = {"fmadd",     PPUInterpreter::fmadd};
    s_table63[0x1E] = {"fnmsub",    PPUInterpreter::fnmsub};
    s_table63[0x1F] = {"fnmadd",    PPUInterpreter::fnmadd};

    // Initialize Table 63 (Extended)
    for (auto& caller : s_table63_) {
        caller = {"unknown", PPUInterpreter::unknown};
    }
    s_table63_[0x000] = {"fcmpu",   PPUInterpreter::fcmpu};
    s_table63_[0x00C] = {"frsp",    PPUInterpreter::frsp};
    s_table63_[0x00E] = {"fctiw",   PPUInterpreter::fctiw};
    s_table63_[0x00F] = {"fctiwz",  PPUInterpreter::fctiwz};
    s_table63_[0x012] = {"fdiv",    PPUInterpreter::fdiv};
    s_table63_[0x014] = {"fsub",    PPUInterpreter::fsub};
    s_table63_[0x015] = {"fadd",    PPUInterpreter::fadd};
    s_table63_[0x016] = {"fsqrt",   PPUInterpreter::fsqrt};
    s_table63_[0x01A] = {"frsqrte", PPUInterpreter::frsqrte};
    s_table63_[0x020] = {"fcmpo",   PPUInterpreter::fcmpo};
    s_table63_[0x026] = {"mtfsb1",  PPUInterpreter::mtfsb1};
    s_table63_[0x028] = {"fneg",    PPUInterpreter::fneg};
    s_table63_[0x040] = {"mcrfs",   PPUInterpreter::mcrfs};
    s_table63_[0x046] = {"mtfsb0",  PPUInterpreter::mtfsb0};
    s_table63_[0x048] = {"fmr",     PPUInterpreter::fmr};
    s_table63_[0x086] = {"mtfsfi",  PPUInterpreter::mtfsfi};
    s_table63_[0x088] = {"fnabs",   PPUInterpreter::fnabs};
    s_table63_[0x108] = {"fabs",    PPUInterpreter::fabs};
    s_table63_[0x247] = {"mffs",    PPUInterpreter::mffs};
    s_table63_[0x2C7] = {"mtfsf",   PPUInterpreter::mtfsf};
    s_table63_[0x32E] = {"fctid",   PPUInterpreter::fctid};
    s_table63_[0x32F] = {"fctidz",  PPUInterpreter::fctidz};
    s_table63_[0x34E] = {"fcfid",   PPUInterpreter::fcfid};
}

}  // namespace ppu
}  // namespace cpu
