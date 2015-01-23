/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_tables.h"
#include "nucleus/cpu/ppu/interpreter/ppu_interpreter.h"
#include "nucleus/cpu/ppu/ppu_thread.h"

// Instruction entry
#define INSTRUCTION(name) { ENTRY_INSTRUCTION, #name, PPUInterpreter::name, nullptr }

// Table entry
# define TABLE(caller) { ENTRY_TABLE, nullptr, nullptr, caller }

namespace cpu {
namespace ppu {

Entry s_tablePrimary[0x40];
Entry s_table4[0x40];
Entry s_table4_[0x800];
Entry s_table19[0x400];
Entry s_table30[0x8];
Entry s_table31[0x400];
Entry s_table58[0x4];
Entry s_table59[0x20];
Entry s_table62[0x4];
Entry s_table63[0x20];
Entry s_table63_[0x400];

const Entry& get_entry(Instruction code)
{
    if (s_tablePrimary[code.opcode].type == ENTRY_TABLE) {
        return s_tablePrimary[code.opcode].caller(code);
    }
    return s_tablePrimary[code.opcode];
}

const Entry& get_table4(Instruction code)
{
    if (s_table4[code.op4].type == ENTRY_TABLE) {
        return s_table4[code.op4].caller(code);
    }
    return s_table4[code.op4];
}

const Entry& get_table63(Instruction code)
{
    if (s_table63[code.op63].type == ENTRY_TABLE) {
        return s_table63[code.op63].caller(code);
    }
    return s_table63[code.op63];
}

const Entry& get_table4_ (Instruction code) { return s_table4_[code.op4_]; }
const Entry& get_table19 (Instruction code) { return s_table19[code.op19]; }
const Entry& get_table30 (Instruction code) { return s_table30[code.op30]; }
const Entry& get_table31 (Instruction code) { return s_table31[code.op31]; }
const Entry& get_table58 (Instruction code) { return s_table58[code.op58]; }
const Entry& get_table59 (Instruction code) { return s_table59[code.op59]; }
const Entry& get_table62 (Instruction code) { return s_table62[code.op62]; }
const Entry& get_table63_(Instruction code) { return s_table63_[code.op63_]; }

void initTables()
{
    // Initialize Primary Table
    for (auto& caller : s_tablePrimary) {
        caller = { ENTRY_INVALID };
    }
    s_tablePrimary[0x04] = TABLE(get_table4);
    s_tablePrimary[0x13] = TABLE(get_table19);
    s_tablePrimary[0x1e] = TABLE(get_table30);
    s_tablePrimary[0x1f] = TABLE(get_table31);
    s_tablePrimary[0x3a] = TABLE(get_table58);
    s_tablePrimary[0x3b] = TABLE(get_table59);
    s_tablePrimary[0x3e] = TABLE(get_table62);
    s_tablePrimary[0x3f] = TABLE(get_table63);
    s_tablePrimary[0x02] = INSTRUCTION(tdi);
    s_tablePrimary[0x03] = INSTRUCTION(twi);
    s_tablePrimary[0x07] = INSTRUCTION(mulli);
    s_tablePrimary[0x08] = INSTRUCTION(subfic);
    s_tablePrimary[0x0a] = INSTRUCTION(cmpli);
    s_tablePrimary[0x0b] = INSTRUCTION(cmpi);
    s_tablePrimary[0x0c] = INSTRUCTION(addic);
    s_tablePrimary[0x0d] = INSTRUCTION(addic_);
    s_tablePrimary[0x0e] = INSTRUCTION(addi);
    s_tablePrimary[0x0f] = INSTRUCTION(addis);
    s_tablePrimary[0x10] = INSTRUCTION(bc);
    s_tablePrimary[0x11] = INSTRUCTION(sc);
    s_tablePrimary[0x12] = INSTRUCTION(b);
    s_tablePrimary[0x14] = INSTRUCTION(rlwimix);
    s_tablePrimary[0x15] = INSTRUCTION(rlwinmx);
    s_tablePrimary[0x17] = INSTRUCTION(rlwnmx);
    s_tablePrimary[0x18] = INSTRUCTION(ori);
    s_tablePrimary[0x19] = INSTRUCTION(oris);
    s_tablePrimary[0x1a] = INSTRUCTION(xori);
    s_tablePrimary[0x1b] = INSTRUCTION(xoris);
    s_tablePrimary[0x1c] = INSTRUCTION(andi_);
    s_tablePrimary[0x1d] = INSTRUCTION(andis_);
    s_tablePrimary[0x20] = INSTRUCTION(lwz);
    s_tablePrimary[0x21] = INSTRUCTION(lwzu);
    s_tablePrimary[0x22] = INSTRUCTION(lbz);
    s_tablePrimary[0x23] = INSTRUCTION(lbzu);
    s_tablePrimary[0x24] = INSTRUCTION(stw);
    s_tablePrimary[0x25] = INSTRUCTION(stwu);
    s_tablePrimary[0x26] = INSTRUCTION(stb);
    s_tablePrimary[0x27] = INSTRUCTION(stbu);
    s_tablePrimary[0x28] = INSTRUCTION(lhz);
    s_tablePrimary[0x29] = INSTRUCTION(lhzu);
    s_tablePrimary[0x2a] = INSTRUCTION(lha);
    s_tablePrimary[0x2b] = INSTRUCTION(lhau);
    s_tablePrimary[0x2c] = INSTRUCTION(sth);
    s_tablePrimary[0x2d] = INSTRUCTION(sthu);
    s_tablePrimary[0x2e] = INSTRUCTION(lmw);
    s_tablePrimary[0x2f] = INSTRUCTION(stmw);
    s_tablePrimary[0x30] = INSTRUCTION(lfs);
    s_tablePrimary[0x31] = INSTRUCTION(lfsu);
    s_tablePrimary[0x32] = INSTRUCTION(lfd);
    s_tablePrimary[0x33] = INSTRUCTION(lfdu);
    s_tablePrimary[0x34] = INSTRUCTION(stfs);
    s_tablePrimary[0x35] = INSTRUCTION(stfsu);
    s_tablePrimary[0x36] = INSTRUCTION(stfd);
    s_tablePrimary[0x37] = INSTRUCTION(stfdu);

    // Initialize Table 4
    for (auto& caller : s_table4) {
        caller = TABLE(get_table4_);
    }
    s_table4[0x020] = INSTRUCTION(vmhaddshs);
    s_table4[0x021] = INSTRUCTION(vmhraddshs);
    s_table4[0x022] = INSTRUCTION(vmladduhm);
    s_table4[0x024] = INSTRUCTION(vmsumubm);
    s_table4[0x025] = INSTRUCTION(vmsummbm);
    s_table4[0x026] = INSTRUCTION(vmsumuhm);
    s_table4[0x027] = INSTRUCTION(vmsumuhs);
    s_table4[0x028] = INSTRUCTION(vmsumshm);
    s_table4[0x029] = INSTRUCTION(vmsumshs);
    s_table4[0x02A] = INSTRUCTION(vsel);
    s_table4[0x02B] = INSTRUCTION(vperm);
    s_table4[0x02C] = INSTRUCTION(vsldoi);
    s_table4[0x02E] = INSTRUCTION(vmaddfp);
    s_table4[0x02F] = INSTRUCTION(vnmsubfp);

    // Initialize Table 4 (Extended)
    for (auto& caller : s_table4_) {
        caller = { ENTRY_INVALID };
    }
    s_table4_[0x000] = INSTRUCTION(vaddubm);
    s_table4_[0x002] = INSTRUCTION(vmaxub);
    s_table4_[0x004] = INSTRUCTION(vrlb);
    s_table4_[0x006] = INSTRUCTION(vcmpequb);
    s_table4_[0x008] = INSTRUCTION(vmuloub);
    s_table4_[0x00A] = INSTRUCTION(vaddfp);
    s_table4_[0x00C] = INSTRUCTION(vmrghb);
    s_table4_[0x00E] = INSTRUCTION(vpkuhum);
    s_table4_[0x040] = INSTRUCTION(vadduhm);
    s_table4_[0x042] = INSTRUCTION(vmaxuh);
    s_table4_[0x044] = INSTRUCTION(vrlh);
    s_table4_[0x046] = INSTRUCTION(vcmpequh);
    s_table4_[0x048] = INSTRUCTION(vmulouh);
    s_table4_[0x04A] = INSTRUCTION(vsubfp);
    s_table4_[0x04C] = INSTRUCTION(vmrghh);
    s_table4_[0x04E] = INSTRUCTION(vpkuwum);
    s_table4_[0x080] = INSTRUCTION(vadduwm);
    s_table4_[0x082] = INSTRUCTION(vmaxuw);
    s_table4_[0x084] = INSTRUCTION(vrlw);
    s_table4_[0x086] = INSTRUCTION(vcmpequw);
    s_table4_[0x08C] = INSTRUCTION(vmrghw);
    s_table4_[0x08E] = INSTRUCTION(vpkuhus);
    s_table4_[0x0C6] = INSTRUCTION(vcmpeqfp);
    s_table4_[0x0CE] = INSTRUCTION(vpkuwus);
    s_table4_[0x102] = INSTRUCTION(vmaxsb);
    s_table4_[0x104] = INSTRUCTION(vslb);
    s_table4_[0x108] = INSTRUCTION(vmulosb);
    s_table4_[0x10A] = INSTRUCTION(vrefp);
    s_table4_[0x10C] = INSTRUCTION(vmrglb);
    s_table4_[0x10E] = INSTRUCTION(vpkshus);
    s_table4_[0x142] = INSTRUCTION(vmaxsh);
    s_table4_[0x144] = INSTRUCTION(vslh);
    s_table4_[0x148] = INSTRUCTION(vmulosh);
    s_table4_[0x14A] = INSTRUCTION(vrsqrtefp);
    s_table4_[0x14C] = INSTRUCTION(vmrglh);
    s_table4_[0x14E] = INSTRUCTION(vpkswus);
    s_table4_[0x180] = INSTRUCTION(vaddcuw);
    s_table4_[0x182] = INSTRUCTION(vmaxsw);
    s_table4_[0x184] = INSTRUCTION(vslw);
    s_table4_[0x18A] = INSTRUCTION(vexptefp);
    s_table4_[0x18C] = INSTRUCTION(vmrglw);
    s_table4_[0x18E] = INSTRUCTION(vpkshss);
    s_table4_[0x1C4] = INSTRUCTION(vsl);
    s_table4_[0x1C6] = INSTRUCTION(vcmpgefp);
    s_table4_[0x1CA] = INSTRUCTION(vlogefp);
    s_table4_[0x1CE] = INSTRUCTION(vpkswss);
    s_table4_[0x200] = INSTRUCTION(vaddubs);
    s_table4_[0x202] = INSTRUCTION(vminub);
    s_table4_[0x204] = INSTRUCTION(vsrb);
    s_table4_[0x206] = INSTRUCTION(vcmpgtub);
    s_table4_[0x208] = INSTRUCTION(vmuleub);
    s_table4_[0x20A] = INSTRUCTION(vrfin);
    s_table4_[0x20C] = INSTRUCTION(vspltb);
    s_table4_[0x20E] = INSTRUCTION(vupkhsb);
    s_table4_[0x240] = INSTRUCTION(vadduhs);
    s_table4_[0x242] = INSTRUCTION(vminuh);
    s_table4_[0x244] = INSTRUCTION(vsrh);
    s_table4_[0x246] = INSTRUCTION(vcmpgtuh);
    s_table4_[0x248] = INSTRUCTION(vmuleuh);
    s_table4_[0x24A] = INSTRUCTION(vrfiz);
    s_table4_[0x24C] = INSTRUCTION(vsplth);
    s_table4_[0x24E] = INSTRUCTION(vupkhsh);
    s_table4_[0x280] = INSTRUCTION(vadduws);
    s_table4_[0x282] = INSTRUCTION(vminuw);
    s_table4_[0x284] = INSTRUCTION(vsrw);
    s_table4_[0x286] = INSTRUCTION(vcmpgtuw);
    s_table4_[0x28A] = INSTRUCTION(vrfip);
    s_table4_[0x28C] = INSTRUCTION(vspltw);
    s_table4_[0x28E] = INSTRUCTION(vupklsb);
    s_table4_[0x2C4] = INSTRUCTION(vsr);
    s_table4_[0x2C6] = INSTRUCTION(vcmpgtfp);
    s_table4_[0x2CA] = INSTRUCTION(vrfim);
    s_table4_[0x2CE] = INSTRUCTION(vupklsh);
    s_table4_[0x300] = INSTRUCTION(vaddsbs);
    s_table4_[0x302] = INSTRUCTION(vminsb);
    s_table4_[0x304] = INSTRUCTION(vsrab);
    s_table4_[0x306] = INSTRUCTION(vcmpgtsb);
    s_table4_[0x308] = INSTRUCTION(vmulesb);
    s_table4_[0x30A] = INSTRUCTION(vcfux);
    s_table4_[0x30C] = INSTRUCTION(vspltisb);
    s_table4_[0x30E] = INSTRUCTION(vpkpx);
    s_table4_[0x340] = INSTRUCTION(vaddshs);
    s_table4_[0x342] = INSTRUCTION(vminsh);
    s_table4_[0x344] = INSTRUCTION(vsrah);
    s_table4_[0x346] = INSTRUCTION(vcmpgtsh);
    s_table4_[0x348] = INSTRUCTION(vmulesh);
    s_table4_[0x34A] = INSTRUCTION(vcfsx);
    s_table4_[0x34C] = INSTRUCTION(vspltish);
    s_table4_[0x34E] = INSTRUCTION(vupkhpx);
    s_table4_[0x380] = INSTRUCTION(vaddsws);
    s_table4_[0x382] = INSTRUCTION(vminsw);
    s_table4_[0x384] = INSTRUCTION(vsraw);
    s_table4_[0x386] = INSTRUCTION(vcmpgtsw);
    s_table4_[0x38A] = INSTRUCTION(vctuxs);
    s_table4_[0x38C] = INSTRUCTION(vspltisw);
    s_table4_[0x3C6] = INSTRUCTION(vcmpbfp);
    s_table4_[0x3CA] = INSTRUCTION(vctsxs);
    s_table4_[0x3CE] = INSTRUCTION(vupklpx);
    s_table4_[0x400] = INSTRUCTION(vsububm);
    s_table4_[0x402] = INSTRUCTION(vavgub);
    s_table4_[0x404] = INSTRUCTION(vand);
    s_table4_[0x406] = INSTRUCTION(vcmpequb_);
    s_table4_[0x40A] = INSTRUCTION(vmaxfp);
    s_table4_[0x40C] = INSTRUCTION(vslo);
    s_table4_[0x440] = INSTRUCTION(vsubuhm);
    s_table4_[0x442] = INSTRUCTION(vavguh);
    s_table4_[0x444] = INSTRUCTION(vandc);
    s_table4_[0x446] = INSTRUCTION(vcmpequh_);
    s_table4_[0x44A] = INSTRUCTION(vminfp);
    s_table4_[0x44C] = INSTRUCTION(vsro);
    s_table4_[0x480] = INSTRUCTION(vsubuwm);
    s_table4_[0x482] = INSTRUCTION(vavguw);
    s_table4_[0x484] = INSTRUCTION(vor);
    s_table4_[0x486] = INSTRUCTION(vcmpequw_);
    s_table4_[0x4C4] = INSTRUCTION(vxor);
    s_table4_[0x4C6] = INSTRUCTION(vcmpeqfp_);
    s_table4_[0x502] = INSTRUCTION(vavgsb);
    s_table4_[0x504] = INSTRUCTION(vnor);
    s_table4_[0x542] = INSTRUCTION(vavgsh);
    s_table4_[0x580] = INSTRUCTION(vsubcuw);
    s_table4_[0x582] = INSTRUCTION(vavgsw);
    s_table4_[0x5C6] = INSTRUCTION(vcmpgefp_);
    s_table4_[0x600] = INSTRUCTION(vsububs);
    s_table4_[0x604] = INSTRUCTION(mfvscr);
    s_table4_[0x606] = INSTRUCTION(vcmpgtub_);
    s_table4_[0x608] = INSTRUCTION(vsum4ubs);
    s_table4_[0x640] = INSTRUCTION(vsubuhs);
    s_table4_[0x644] = INSTRUCTION(mtvscr);
    s_table4_[0x646] = INSTRUCTION(vcmpgtuh_);
    s_table4_[0x648] = INSTRUCTION(vsum4shs);
    s_table4_[0x680] = INSTRUCTION(vsubuws);
    s_table4_[0x686] = INSTRUCTION(vcmpgtuw_);
    s_table4_[0x688] = INSTRUCTION(vsum2sws);
    s_table4_[0x6C6] = INSTRUCTION(vcmpgtfp_);
    s_table4_[0x700] = INSTRUCTION(vsubsbs);
    s_table4_[0x706] = INSTRUCTION(vcmpgtsb_);
    s_table4_[0x708] = INSTRUCTION(vsum4sbs);
    s_table4_[0x740] = INSTRUCTION(vsubshs);
    s_table4_[0x746] = INSTRUCTION(vcmpgtsh_);
    s_table4_[0x780] = INSTRUCTION(vsubsws);
    s_table4_[0x786] = INSTRUCTION(vcmpgtsw_);
    s_table4_[0x788] = INSTRUCTION(vsumsws);
    s_table4_[0x7C6] = INSTRUCTION(vcmpbfp_);

    // Initialize Table 19
    for (auto& caller : s_table19) {
        caller = { ENTRY_INVALID };
    }
    s_table19[0x000] = INSTRUCTION(mcrf);
    s_table19[0x010] = INSTRUCTION(bclr);
    s_table19[0x021] = INSTRUCTION(crnor);
    s_table19[0x081] = INSTRUCTION(crandc);
    s_table19[0x096] = INSTRUCTION(isync);
    s_table19[0x0c1] = INSTRUCTION(crxor);
    s_table19[0x0e1] = INSTRUCTION(crnand);
    s_table19[0x101] = INSTRUCTION(crand);
    s_table19[0x121] = INSTRUCTION(creqv);
    s_table19[0x1a1] = INSTRUCTION(crorc);
    s_table19[0x1c1] = INSTRUCTION(cror);
    s_table19[0x210] = INSTRUCTION(bcctr);

    // Initialize Table 30
    for (auto& caller : s_table30) {
        caller = INSTRUCTION(unknown);
    }
    s_table30[0x0] = INSTRUCTION(rldiclx);
    s_table30[0x1] = INSTRUCTION(rldicrx);
    s_table30[0x2] = INSTRUCTION(rldicx);
    s_table30[0x3] = INSTRUCTION(rldimix);
    s_table30[0x4] = INSTRUCTION(rldc_lr);

    // Initialize Table 31
    for (auto& caller : s_table31) {
        caller = { ENTRY_INVALID };
    }
    s_table31[0x000] = INSTRUCTION(cmp);
    s_table31[0x004] = INSTRUCTION(tw);
    s_table31[0x006] = INSTRUCTION(lvsl);
    s_table31[0x007] = INSTRUCTION(lvebx);
    s_table31[0x008] = INSTRUCTION(subfc);
    s_table31[0x009] = INSTRUCTION(mulhdu);
    s_table31[0x00A] = INSTRUCTION(addc);
    s_table31[0x00B] = INSTRUCTION(mulhwu);
    s_table31[0x013] = INSTRUCTION(mfocrf);
    s_table31[0x014] = INSTRUCTION(lwarx);
    s_table31[0x015] = INSTRUCTION(ldx);
    s_table31[0x017] = INSTRUCTION(lwzx);
    s_table31[0x018] = INSTRUCTION(slw);
    s_table31[0x01A] = INSTRUCTION(cntlzw);
    s_table31[0x01B] = INSTRUCTION(sld);
    s_table31[0x01C] = INSTRUCTION(andx);
    s_table31[0x020] = INSTRUCTION(cmpl);
    s_table31[0x026] = INSTRUCTION(lvsr);
    s_table31[0x027] = INSTRUCTION(lvehx);
    s_table31[0x028] = INSTRUCTION(subf);
    s_table31[0x035] = INSTRUCTION(ldux);
    s_table31[0x036] = INSTRUCTION(dcbst);
    s_table31[0x037] = INSTRUCTION(lwzux);
    s_table31[0x03A] = INSTRUCTION(cntlzd);
    s_table31[0x03C] = INSTRUCTION(andc);
    s_table31[0x044] = INSTRUCTION(td);
    s_table31[0x047] = INSTRUCTION(lvewx);
    s_table31[0x049] = INSTRUCTION(mulhd);
    s_table31[0x04B] = INSTRUCTION(mulhw);
    s_table31[0x054] = INSTRUCTION(ldarx);
    s_table31[0x056] = INSTRUCTION(dcbf);
    s_table31[0x057] = INSTRUCTION(lbzx);
    s_table31[0x067] = INSTRUCTION(lvx);
    s_table31[0x068] = INSTRUCTION(neg);
    s_table31[0x077] = INSTRUCTION(lbzux);
    s_table31[0x07C] = INSTRUCTION(nor);
    s_table31[0x087] = INSTRUCTION(stvebx);
    s_table31[0x088] = INSTRUCTION(subfe);
    s_table31[0x08A] = INSTRUCTION(adde);
    s_table31[0x090] = INSTRUCTION(mtocrf);
    s_table31[0x095] = INSTRUCTION(stdx);
    s_table31[0x096] = INSTRUCTION(stwcx_);
    s_table31[0x097] = INSTRUCTION(stwx);
    s_table31[0x0A7] = INSTRUCTION(stvehx);
    s_table31[0x0B5] = INSTRUCTION(stdux);
    s_table31[0x0B7] = INSTRUCTION(stwux);
    s_table31[0x0C7] = INSTRUCTION(stvewx);
    s_table31[0x0C8] = INSTRUCTION(subfze);
    s_table31[0x0CA] = INSTRUCTION(addze);
    s_table31[0x0D6] = INSTRUCTION(stdcx_);
    s_table31[0x0D7] = INSTRUCTION(stbx);
    s_table31[0x0E7] = INSTRUCTION(stvx);
    s_table31[0x0E8] = INSTRUCTION(subfme);
    s_table31[0x0E9] = INSTRUCTION(mulld);
    s_table31[0x0EA] = INSTRUCTION(addme);
    s_table31[0x0EB] = INSTRUCTION(mullw);
    s_table31[0x0F6] = INSTRUCTION(dcbtst);
    s_table31[0x0F7] = INSTRUCTION(stbux);
    s_table31[0x10A] = INSTRUCTION(add);
    s_table31[0x116] = INSTRUCTION(dcbt);
    s_table31[0x117] = INSTRUCTION(lhzx);
    s_table31[0x11C] = INSTRUCTION(eqv);
    s_table31[0x136] = INSTRUCTION(eciwx);
    s_table31[0x137] = INSTRUCTION(lhzux);
    s_table31[0x13C] = INSTRUCTION(xorx);
    s_table31[0x153] = INSTRUCTION(mfspr);
    s_table31[0x155] = INSTRUCTION(lwax);
    s_table31[0x156] = INSTRUCTION(dst);
    s_table31[0x157] = INSTRUCTION(lhax);
    s_table31[0x167] = INSTRUCTION(lvxl);
    s_table31[0x173] = INSTRUCTION(mftb);
    s_table31[0x175] = INSTRUCTION(lwaux);
    s_table31[0x176] = INSTRUCTION(dstst);
    s_table31[0x177] = INSTRUCTION(lhaux);
    s_table31[0x197] = INSTRUCTION(sthx);
    s_table31[0x19C] = INSTRUCTION(orc);
    s_table31[0x1B6] = INSTRUCTION(ecowx);
    s_table31[0x1B7] = INSTRUCTION(sthux);
    s_table31[0x1BC] = INSTRUCTION(orx);
    s_table31[0x1C9] = INSTRUCTION(divdu);
    s_table31[0x1CB] = INSTRUCTION(divwu);
    s_table31[0x1D3] = INSTRUCTION(mtspr);
    s_table31[0x1DC] = INSTRUCTION(nand);
    s_table31[0x1E7] = INSTRUCTION(stvxl);
    s_table31[0x1E9] = INSTRUCTION(divd);
    s_table31[0x1EB] = INSTRUCTION(divw);
    s_table31[0x207] = INSTRUCTION(lvlx);
    s_table31[0x214] = INSTRUCTION(ldbrx);
    s_table31[0x215] = INSTRUCTION(lswx);
    s_table31[0x216] = INSTRUCTION(lwbrx);
    s_table31[0x217] = INSTRUCTION(lfsx);
    s_table31[0x218] = INSTRUCTION(srw);
    s_table31[0x21B] = INSTRUCTION(srd);
    s_table31[0x227] = INSTRUCTION(lvrx);
    s_table31[0x237] = INSTRUCTION(lfsux);
    s_table31[0x255] = INSTRUCTION(lswi);
    s_table31[0x256] = INSTRUCTION(sync);
    s_table31[0x257] = INSTRUCTION(lfdx);
    s_table31[0x277] = INSTRUCTION(lfdux);
    s_table31[0x287] = INSTRUCTION(stvlx);
    s_table31[0x295] = INSTRUCTION(stswx);
    s_table31[0x296] = INSTRUCTION(stwbrx);
    s_table31[0x297] = INSTRUCTION(stfsx);
    s_table31[0x2A7] = INSTRUCTION(stvrx);
    s_table31[0x2B7] = INSTRUCTION(stfsux);
    s_table31[0x2D5] = INSTRUCTION(stswi);
    s_table31[0x2D7] = INSTRUCTION(stfdx);
    s_table31[0x2F7] = INSTRUCTION(stfdux);
    s_table31[0x307] = INSTRUCTION(lvlxl);
    s_table31[0x316] = INSTRUCTION(lhbrx);
    s_table31[0x318] = INSTRUCTION(sraw);
    s_table31[0x31A] = INSTRUCTION(srad);
    s_table31[0x327] = INSTRUCTION(lvrxl);
    s_table31[0x336] = INSTRUCTION(dss);
    s_table31[0x338] = INSTRUCTION(srawi);
    s_table31[0x33A] = INSTRUCTION(sradi);
    s_table31[0x33B] = INSTRUCTION(sradi);
    s_table31[0x356] = INSTRUCTION(eieio);
    s_table31[0x387] = INSTRUCTION(stvlxl);
    s_table31[0x396] = INSTRUCTION(sthbrx);
    s_table31[0x39A] = INSTRUCTION(extsh);
    s_table31[0x387] = INSTRUCTION(stvrxl);
    s_table31[0x3BA] = INSTRUCTION(extsb);
    s_table31[0x3D7] = INSTRUCTION(stfiwx);
    s_table31[0x3DA] = INSTRUCTION(extsw);
    s_table31[0x3D6] = INSTRUCTION(icbi);
    s_table31[0x3F6] = INSTRUCTION(dcbz);

    // Initialize Table 58
    for (auto& caller : s_table58) {
        caller = { ENTRY_INVALID };
    }
    s_table58[0x0] = INSTRUCTION(ld);
    s_table58[0x1] = INSTRUCTION(ldu);
    s_table58[0x2] = INSTRUCTION(lwa);

    // Initialize Table 59
    for (auto& caller : s_table59) {
        caller = { ENTRY_INVALID };
    }
    s_table59[0x12] = INSTRUCTION(fdivs);
    s_table59[0x14] = INSTRUCTION(fsubs);
    s_table59[0x15] = INSTRUCTION(fadds);
    s_table59[0x16] = INSTRUCTION(fsqrts);
    s_table59[0x18] = INSTRUCTION(fres);
    s_table59[0x19] = INSTRUCTION(fmuls);
    s_table59[0x1C] = INSTRUCTION(fmsubs);
    s_table59[0x1D] = INSTRUCTION(fmadds);
    s_table59[0x1E] = INSTRUCTION(fnmsubs);
    s_table59[0x1F] = INSTRUCTION(fnmadds);

    // Initialize Table 62
    for (auto& caller : s_table62) {
        caller = { ENTRY_INVALID };
    }
    s_table62[0x0] = INSTRUCTION(std);
    s_table62[0x1] = INSTRUCTION(stdu);

    // Initialize Table 63
    for (auto& caller : s_table63) {
        caller = TABLE(get_table63_);
    }
    s_table63[0x17] = INSTRUCTION(fsel);
    s_table63[0x19] = INSTRUCTION(fmul);
    s_table63[0x1C] = INSTRUCTION(fmsub);
    s_table63[0x1D] = INSTRUCTION(fmadd);
    s_table63[0x1E] = INSTRUCTION(fnmsub);
    s_table63[0x1F] = INSTRUCTION(fnmadd);

    // Initialize Table 63 (Extended)
    for (auto& caller : s_table63_) {
        caller = { ENTRY_INVALID };
    }
    s_table63_[0x000] = INSTRUCTION(fcmpu);
    s_table63_[0x00C] = INSTRUCTION(frsp);
    s_table63_[0x00E] = INSTRUCTION(fctiw);
    s_table63_[0x00F] = INSTRUCTION(fctiwz);
    s_table63_[0x012] = INSTRUCTION(fdiv);
    s_table63_[0x014] = INSTRUCTION(fsub);
    s_table63_[0x015] = INSTRUCTION(fadd);
    s_table63_[0x016] = INSTRUCTION(fsqrt);
    s_table63_[0x01A] = INSTRUCTION(frsqrte);
    s_table63_[0x020] = INSTRUCTION(fcmpo);
    s_table63_[0x026] = INSTRUCTION(mtfsb1);
    s_table63_[0x028] = INSTRUCTION(fneg);
    s_table63_[0x040] = INSTRUCTION(mcrfs);
    s_table63_[0x046] = INSTRUCTION(mtfsb0);
    s_table63_[0x048] = INSTRUCTION(fmr);
    s_table63_[0x086] = INSTRUCTION(mtfsfi);
    s_table63_[0x088] = INSTRUCTION(fnabs);
    s_table63_[0x108] = INSTRUCTION(fabs);
    s_table63_[0x247] = INSTRUCTION(mffs);
    s_table63_[0x2C7] = INSTRUCTION(mtfsf);
    s_table63_[0x32E] = INSTRUCTION(fctid);
    s_table63_[0x32F] = INSTRUCTION(fctidz);
    s_table63_[0x34E] = INSTRUCTION(fcfid);
}

}  // namespace ppu
}  // namespace cpu
