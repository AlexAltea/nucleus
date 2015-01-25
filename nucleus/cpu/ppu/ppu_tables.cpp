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

/**
 * PPU tables:
 * Initialized as static constant data when Nucleus starts up.
 * Invalid entries are zero-filled, therefore matching the ENTRY_INVALID type.
 * NOTE: Use designated initializers instead as soon as they become available on C++.
 */

template <int N>
struct Table {
    Entry table[N];

    const Entry& operator[] (const u32 i) const {
        return table[i];
    }
};

// Primary Table
static const struct table_primary_t : Table<0x40> {
    table_primary_t() : Table() {
        table[0x004] = TABLE(get_table4);
        table[0x013] = TABLE(get_table19);
        table[0x01E] = TABLE(get_table30);
        table[0x01F] = TABLE(get_table31);
        table[0x03A] = TABLE(get_table58);
        table[0x03B] = TABLE(get_table59);
        table[0x03E] = TABLE(get_table62);
        table[0x03F] = TABLE(get_table63);
        table[0x002] = INSTRUCTION(tdi);
        table[0x003] = INSTRUCTION(twi);
        table[0x007] = INSTRUCTION(mulli);
        table[0x008] = INSTRUCTION(subfic);
        table[0x00A] = INSTRUCTION(cmpli);
        table[0x00B] = INSTRUCTION(cmpi);
        table[0x00C] = INSTRUCTION(addic);
        table[0x00D] = INSTRUCTION(addic_);
        table[0x00E] = INSTRUCTION(addi);
        table[0x00F] = INSTRUCTION(addis);
        table[0x010] = INSTRUCTION(bcx);
        table[0x011] = INSTRUCTION(sc);
        table[0x012] = INSTRUCTION(bx);
        table[0x014] = INSTRUCTION(rlwimix);
        table[0x015] = INSTRUCTION(rlwinmx);
        table[0x017] = INSTRUCTION(rlwnmx);
        table[0x018] = INSTRUCTION(ori);
        table[0x019] = INSTRUCTION(oris);
        table[0x01A] = INSTRUCTION(xori);
        table[0x01B] = INSTRUCTION(xoris);
        table[0x01C] = INSTRUCTION(andi_);
        table[0x01D] = INSTRUCTION(andis_);
        table[0x020] = INSTRUCTION(lwz);
        table[0x021] = INSTRUCTION(lwzu);
        table[0x022] = INSTRUCTION(lbz);
        table[0x023] = INSTRUCTION(lbzu);
        table[0x024] = INSTRUCTION(stw);
        table[0x025] = INSTRUCTION(stwu);
        table[0x026] = INSTRUCTION(stb);
        table[0x027] = INSTRUCTION(stbu);
        table[0x028] = INSTRUCTION(lhz);
        table[0x029] = INSTRUCTION(lhzu);
        table[0x02A] = INSTRUCTION(lha);
        table[0x02B] = INSTRUCTION(lhau);
        table[0x02C] = INSTRUCTION(sth);
        table[0x02D] = INSTRUCTION(sthu);
        table[0x02E] = INSTRUCTION(lmw);
        table[0x02F] = INSTRUCTION(stmw);
        table[0x030] = INSTRUCTION(lfs);
        table[0x031] = INSTRUCTION(lfsu);
        table[0x032] = INSTRUCTION(lfd);
        table[0x033] = INSTRUCTION(lfdu);
        table[0x034] = INSTRUCTION(stfs);
        table[0x035] = INSTRUCTION(stfsu);
        table[0x036] = INSTRUCTION(stfd);
        table[0x037] = INSTRUCTION(stfdu);
    }
} tablePrimary;

// Table 4
static const struct table4_t : Table<0x40> {
    table4_t() : Table() {
        table[0x020] = INSTRUCTION(vmhaddshs);
        table[0x021] = INSTRUCTION(vmhraddshs);
        table[0x022] = INSTRUCTION(vmladduhm);
        table[0x024] = INSTRUCTION(vmsumubm);
        table[0x025] = INSTRUCTION(vmsummbm);
        table[0x026] = INSTRUCTION(vmsumuhm);
        table[0x027] = INSTRUCTION(vmsumuhs);
        table[0x028] = INSTRUCTION(vmsumshm);
        table[0x029] = INSTRUCTION(vmsumshs);
        table[0x02A] = INSTRUCTION(vsel);
        table[0x02B] = INSTRUCTION(vperm);
        table[0x02C] = INSTRUCTION(vsldoi);
        table[0x02E] = INSTRUCTION(vmaddfp);
        table[0x02F] = INSTRUCTION(vnmsubfp);
    }
} table4;

// Table 4 (Extended)
static const struct table4_ext_t : Table<0x800> {
    table4_ext_t() : Table() {
        table[0x000] = INSTRUCTION(vaddubm);
        table[0x002] = INSTRUCTION(vmaxub);
        table[0x004] = INSTRUCTION(vrlb);
        table[0x006] = INSTRUCTION(vcmpequb);
        table[0x008] = INSTRUCTION(vmuloub);
        table[0x00A] = INSTRUCTION(vaddfp);
        table[0x00C] = INSTRUCTION(vmrghb);
        table[0x00E] = INSTRUCTION(vpkuhum);
        table[0x040] = INSTRUCTION(vadduhm);
        table[0x042] = INSTRUCTION(vmaxuh);
        table[0x044] = INSTRUCTION(vrlh);
        table[0x046] = INSTRUCTION(vcmpequh);
        table[0x048] = INSTRUCTION(vmulouh);
        table[0x04A] = INSTRUCTION(vsubfp);
        table[0x04C] = INSTRUCTION(vmrghh);
        table[0x04E] = INSTRUCTION(vpkuwum);
        table[0x080] = INSTRUCTION(vadduwm);
        table[0x082] = INSTRUCTION(vmaxuw);
        table[0x084] = INSTRUCTION(vrlw);
        table[0x086] = INSTRUCTION(vcmpequw);
        table[0x08C] = INSTRUCTION(vmrghw);
        table[0x08E] = INSTRUCTION(vpkuhus);
        table[0x0C6] = INSTRUCTION(vcmpeqfp);
        table[0x0CE] = INSTRUCTION(vpkuwus);
        table[0x102] = INSTRUCTION(vmaxsb);
        table[0x104] = INSTRUCTION(vslb);
        table[0x108] = INSTRUCTION(vmulosb);
        table[0x10A] = INSTRUCTION(vrefp);
        table[0x10C] = INSTRUCTION(vmrglb);
        table[0x10E] = INSTRUCTION(vpkshus);
        table[0x142] = INSTRUCTION(vmaxsh);
        table[0x144] = INSTRUCTION(vslh);
        table[0x148] = INSTRUCTION(vmulosh);
        table[0x14A] = INSTRUCTION(vrsqrtefp);
        table[0x14C] = INSTRUCTION(vmrglh);
        table[0x14E] = INSTRUCTION(vpkswus);
        table[0x180] = INSTRUCTION(vaddcuw);
        table[0x182] = INSTRUCTION(vmaxsw);
        table[0x184] = INSTRUCTION(vslw);
        table[0x18A] = INSTRUCTION(vexptefp);
        table[0x18C] = INSTRUCTION(vmrglw);
        table[0x18E] = INSTRUCTION(vpkshss);
        table[0x1C4] = INSTRUCTION(vsl);
        table[0x1C6] = INSTRUCTION(vcmpgefp);
        table[0x1CA] = INSTRUCTION(vlogefp);
        table[0x1CE] = INSTRUCTION(vpkswss);
        table[0x200] = INSTRUCTION(vaddubs);
        table[0x202] = INSTRUCTION(vminub);
        table[0x204] = INSTRUCTION(vsrb);
        table[0x206] = INSTRUCTION(vcmpgtub);
        table[0x208] = INSTRUCTION(vmuleub);
        table[0x20A] = INSTRUCTION(vrfin);
        table[0x20C] = INSTRUCTION(vspltb);
        table[0x20E] = INSTRUCTION(vupkhsb);
        table[0x240] = INSTRUCTION(vadduhs);
        table[0x242] = INSTRUCTION(vminuh);
        table[0x244] = INSTRUCTION(vsrh);
        table[0x246] = INSTRUCTION(vcmpgtuh);
        table[0x248] = INSTRUCTION(vmuleuh);
        table[0x24A] = INSTRUCTION(vrfiz);
        table[0x24C] = INSTRUCTION(vsplth);
        table[0x24E] = INSTRUCTION(vupkhsh);
        table[0x280] = INSTRUCTION(vadduws);
        table[0x282] = INSTRUCTION(vminuw);
        table[0x284] = INSTRUCTION(vsrw);
        table[0x286] = INSTRUCTION(vcmpgtuw);
        table[0x28A] = INSTRUCTION(vrfip);
        table[0x28C] = INSTRUCTION(vspltw);
        table[0x28E] = INSTRUCTION(vupklsb);
        table[0x2C4] = INSTRUCTION(vsr);
        table[0x2C6] = INSTRUCTION(vcmpgtfp);
        table[0x2CA] = INSTRUCTION(vrfim);
        table[0x2CE] = INSTRUCTION(vupklsh);
        table[0x300] = INSTRUCTION(vaddsbs);
        table[0x302] = INSTRUCTION(vminsb);
        table[0x304] = INSTRUCTION(vsrab);
        table[0x306] = INSTRUCTION(vcmpgtsb);
        table[0x308] = INSTRUCTION(vmulesb);
        table[0x30A] = INSTRUCTION(vcfux);
        table[0x30C] = INSTRUCTION(vspltisb);
        table[0x30E] = INSTRUCTION(vpkpx);
        table[0x340] = INSTRUCTION(vaddshs);
        table[0x342] = INSTRUCTION(vminsh);
        table[0x344] = INSTRUCTION(vsrah);
        table[0x346] = INSTRUCTION(vcmpgtsh);
        table[0x348] = INSTRUCTION(vmulesh);
        table[0x34A] = INSTRUCTION(vcfsx);
        table[0x34C] = INSTRUCTION(vspltish);
        table[0x34E] = INSTRUCTION(vupkhpx);
        table[0x380] = INSTRUCTION(vaddsws);
        table[0x382] = INSTRUCTION(vminsw);
        table[0x384] = INSTRUCTION(vsraw);
        table[0x386] = INSTRUCTION(vcmpgtsw);
        table[0x38A] = INSTRUCTION(vctuxs);
        table[0x38C] = INSTRUCTION(vspltisw);
        table[0x3C6] = INSTRUCTION(vcmpbfp);
        table[0x3CA] = INSTRUCTION(vctsxs);
        table[0x3CE] = INSTRUCTION(vupklpx);
        table[0x400] = INSTRUCTION(vsububm);
        table[0x402] = INSTRUCTION(vavgub);
        table[0x404] = INSTRUCTION(vand);
        table[0x406] = INSTRUCTION(vcmpequb_);
        table[0x40A] = INSTRUCTION(vmaxfp);
        table[0x40C] = INSTRUCTION(vslo);
        table[0x440] = INSTRUCTION(vsubuhm);
        table[0x442] = INSTRUCTION(vavguh);
        table[0x444] = INSTRUCTION(vandc);
        table[0x446] = INSTRUCTION(vcmpequh_);
        table[0x44A] = INSTRUCTION(vminfp);
        table[0x44C] = INSTRUCTION(vsro);
        table[0x480] = INSTRUCTION(vsubuwm);
        table[0x482] = INSTRUCTION(vavguw);
        table[0x484] = INSTRUCTION(vor);
        table[0x486] = INSTRUCTION(vcmpequw_);
        table[0x4C4] = INSTRUCTION(vxor);
        table[0x4C6] = INSTRUCTION(vcmpeqfp_);
        table[0x502] = INSTRUCTION(vavgsb);
        table[0x504] = INSTRUCTION(vnor);
        table[0x542] = INSTRUCTION(vavgsh);
        table[0x580] = INSTRUCTION(vsubcuw);
        table[0x582] = INSTRUCTION(vavgsw);
        table[0x5C6] = INSTRUCTION(vcmpgefp_);
        table[0x600] = INSTRUCTION(vsububs);
        table[0x604] = INSTRUCTION(mfvscr);
        table[0x606] = INSTRUCTION(vcmpgtub_);
        table[0x608] = INSTRUCTION(vsum4ubs);
        table[0x640] = INSTRUCTION(vsubuhs);
        table[0x644] = INSTRUCTION(mtvscr);
        table[0x646] = INSTRUCTION(vcmpgtuh_);
        table[0x648] = INSTRUCTION(vsum4shs);
        table[0x680] = INSTRUCTION(vsubuws);
        table[0x686] = INSTRUCTION(vcmpgtuw_);
        table[0x688] = INSTRUCTION(vsum2sws);
        table[0x6C6] = INSTRUCTION(vcmpgtfp_);
        table[0x700] = INSTRUCTION(vsubsbs);
        table[0x706] = INSTRUCTION(vcmpgtsb_);
        table[0x708] = INSTRUCTION(vsum4sbs);
        table[0x740] = INSTRUCTION(vsubshs);
        table[0x746] = INSTRUCTION(vcmpgtsh_);
        table[0x780] = INSTRUCTION(vsubsws);
        table[0x786] = INSTRUCTION(vcmpgtsw_);
        table[0x788] = INSTRUCTION(vsumsws);
        table[0x7C6] = INSTRUCTION(vcmpbfp_);
    }
} table4_;

// Table 19
static const struct table19_t : Table<0x400> {
    table19_t() : Table() {
        table[0x000] = INSTRUCTION(mcrf);
        table[0x010] = INSTRUCTION(bclrx);
        table[0x021] = INSTRUCTION(crnor);
        table[0x081] = INSTRUCTION(crandc);
        table[0x096] = INSTRUCTION(isync);
        table[0x0C1] = INSTRUCTION(crxor);
        table[0x0E1] = INSTRUCTION(crnand);
        table[0x101] = INSTRUCTION(crand);
        table[0x121] = INSTRUCTION(creqv);
        table[0x1A1] = INSTRUCTION(crorc);
        table[0x1C1] = INSTRUCTION(cror);
        table[0x210] = INSTRUCTION(bcctrx);
    }
} table19;

// Table 30
static const struct table30_t : Table<0x8> {
    table30_t() : Table() {
        table[0x000] = INSTRUCTION(rldiclx);
        table[0x001] = INSTRUCTION(rldicrx);
        table[0x002] = INSTRUCTION(rldicx);
        table[0x003] = INSTRUCTION(rldimix);
        table[0x004] = INSTRUCTION(rldc_lr);
    }
} table30;

// Table 31
static const struct table31_t : Table<0x400> {
    table31_t() : Table() {
        table[0x000] = INSTRUCTION(cmp);
        table[0x004] = INSTRUCTION(tw);
        table[0x006] = INSTRUCTION(lvsl);
        table[0x007] = INSTRUCTION(lvebx);
        table[0x008] = INSTRUCTION(subfcx);
        table[0x009] = INSTRUCTION(mulhdux);
        table[0x00A] = INSTRUCTION(addcx);
        table[0x00B] = INSTRUCTION(mulhwux);
        table[0x013] = INSTRUCTION(mfocrf);
        table[0x014] = INSTRUCTION(lwarx);
        table[0x015] = INSTRUCTION(ldx);
        table[0x017] = INSTRUCTION(lwzx);
        table[0x018] = INSTRUCTION(slwx);
        table[0x01A] = INSTRUCTION(cntlzwx);
        table[0x01B] = INSTRUCTION(sldx);
        table[0x01C] = INSTRUCTION(andx);
        table[0x020] = INSTRUCTION(cmpl);
        table[0x026] = INSTRUCTION(lvsr);
        table[0x027] = INSTRUCTION(lvehx);
        table[0x028] = INSTRUCTION(subfx);
        table[0x035] = INSTRUCTION(ldux);
        table[0x036] = INSTRUCTION(dcbst);
        table[0x037] = INSTRUCTION(lwzux);
        table[0x03A] = INSTRUCTION(cntlzdx);
        table[0x03C] = INSTRUCTION(andcx);
        table[0x044] = INSTRUCTION(td);
        table[0x047] = INSTRUCTION(lvewx);
        table[0x049] = INSTRUCTION(mulhdx);
        table[0x04B] = INSTRUCTION(mulhwx);
        table[0x054] = INSTRUCTION(ldarx);
        table[0x056] = INSTRUCTION(dcbf);
        table[0x057] = INSTRUCTION(lbzx);
        table[0x067] = INSTRUCTION(lvx);
        table[0x068] = INSTRUCTION(negx);
        table[0x077] = INSTRUCTION(lbzux);
        table[0x07C] = INSTRUCTION(norx);
        table[0x087] = INSTRUCTION(stvebx);
        table[0x088] = INSTRUCTION(subfex);
        table[0x08A] = INSTRUCTION(addex);
        table[0x090] = INSTRUCTION(mtocrf);
        table[0x095] = INSTRUCTION(stdx);
        table[0x096] = INSTRUCTION(stwcx_);
        table[0x097] = INSTRUCTION(stwx);
        table[0x0A7] = INSTRUCTION(stvehx);
        table[0x0B5] = INSTRUCTION(stdux);
        table[0x0B7] = INSTRUCTION(stwux);
        table[0x0C7] = INSTRUCTION(stvewx);
        table[0x0C8] = INSTRUCTION(subfzex);
        table[0x0CA] = INSTRUCTION(addzex);
        table[0x0D6] = INSTRUCTION(stdcx_);
        table[0x0D7] = INSTRUCTION(stbx);
        table[0x0E7] = INSTRUCTION(stvx);
        table[0x0E8] = INSTRUCTION(subfmex);
        table[0x0E9] = INSTRUCTION(mulldx);
        table[0x0EA] = INSTRUCTION(addmex);
        table[0x0EB] = INSTRUCTION(mullwx);
        table[0x0F6] = INSTRUCTION(dcbtst);
        table[0x0F7] = INSTRUCTION(stbux);
        table[0x10A] = INSTRUCTION(addx);
        table[0x116] = INSTRUCTION(dcbt);
        table[0x117] = INSTRUCTION(lhzx);
        table[0x11C] = INSTRUCTION(eqvx);
        table[0x136] = INSTRUCTION(eciwx);
        table[0x137] = INSTRUCTION(lhzux);
        table[0x13C] = INSTRUCTION(xorx);
        table[0x153] = INSTRUCTION(mfspr);
        table[0x155] = INSTRUCTION(lwax);
        table[0x156] = INSTRUCTION(dst);
        table[0x157] = INSTRUCTION(lhax);
        table[0x167] = INSTRUCTION(lvxl);
        table[0x173] = INSTRUCTION(mftb);
        table[0x175] = INSTRUCTION(lwaux);
        table[0x176] = INSTRUCTION(dstst);
        table[0x177] = INSTRUCTION(lhaux);
        table[0x197] = INSTRUCTION(sthx);
        table[0x19C] = INSTRUCTION(orcx);
        table[0x1B6] = INSTRUCTION(ecowx);
        table[0x1B7] = INSTRUCTION(sthux);
        table[0x1BC] = INSTRUCTION(orx);
        table[0x1C9] = INSTRUCTION(divdux);
        table[0x1CB] = INSTRUCTION(divwux);
        table[0x1D3] = INSTRUCTION(mtspr);
        table[0x1DC] = INSTRUCTION(nandx);
        table[0x1E7] = INSTRUCTION(stvxl);
        table[0x1E9] = INSTRUCTION(divdx);
        table[0x1EB] = INSTRUCTION(divwx);
        table[0x207] = INSTRUCTION(lvlx);
        table[0x214] = INSTRUCTION(ldbrx);
        table[0x215] = INSTRUCTION(lswx);
        table[0x216] = INSTRUCTION(lwbrx);
        table[0x217] = INSTRUCTION(lfsx);
        table[0x218] = INSTRUCTION(srwx);
        table[0x21B] = INSTRUCTION(srdx);
        table[0x227] = INSTRUCTION(lvrx);
        table[0x237] = INSTRUCTION(lfsux);
        table[0x255] = INSTRUCTION(lswi);
        table[0x256] = INSTRUCTION(sync);
        table[0x257] = INSTRUCTION(lfdx);
        table[0x277] = INSTRUCTION(lfdux);
        table[0x287] = INSTRUCTION(stvlx);
        table[0x295] = INSTRUCTION(stswx);
        table[0x296] = INSTRUCTION(stwbrx);
        table[0x297] = INSTRUCTION(stfsx);
        table[0x2A7] = INSTRUCTION(stvrx);
        table[0x2B7] = INSTRUCTION(stfsux);
        table[0x2D5] = INSTRUCTION(stswi);
        table[0x2D7] = INSTRUCTION(stfdx);
        table[0x2F7] = INSTRUCTION(stfdux);
        table[0x307] = INSTRUCTION(lvlxl);
        table[0x316] = INSTRUCTION(lhbrx);
        table[0x318] = INSTRUCTION(srawx);
        table[0x31A] = INSTRUCTION(sradx);
        table[0x327] = INSTRUCTION(lvrxl);
        table[0x336] = INSTRUCTION(dss);
        table[0x338] = INSTRUCTION(srawix);
        table[0x33A] = INSTRUCTION(sradix);
        table[0x33B] = INSTRUCTION(sradix);
        table[0x356] = INSTRUCTION(eieio);
        table[0x387] = INSTRUCTION(stvlxl);
        table[0x396] = INSTRUCTION(sthbrx);
        table[0x39A] = INSTRUCTION(extshx);
        table[0x387] = INSTRUCTION(stvrxl);
        table[0x3BA] = INSTRUCTION(extsbx);
        table[0x3D7] = INSTRUCTION(stfiwx);
        table[0x3DA] = INSTRUCTION(extswx);
        table[0x3D6] = INSTRUCTION(icbi);
        table[0x3F6] = INSTRUCTION(dcbz);
    }
} table31;

// Table 58
static const struct table58_t : Table<0x4> {
    table58_t() : Table() {
        table[0x000] = INSTRUCTION(ld);
        table[0x001] = INSTRUCTION(ldu);
        table[0x002] = INSTRUCTION(lwa);
    }
} table58;

// Table 59
static const struct table59_t : Table<0x20> {
    table59_t() : Table() {
        table[0x012] = INSTRUCTION(fdivsx);
        table[0x014] = INSTRUCTION(fsubsx);
        table[0x015] = INSTRUCTION(faddsx);
        table[0x016] = INSTRUCTION(fsqrtsx);
        table[0x018] = INSTRUCTION(fresx);
        table[0x019] = INSTRUCTION(fmulsx);
        table[0x01C] = INSTRUCTION(fmsubsx);
        table[0x01D] = INSTRUCTION(fmaddsx);
        table[0x01E] = INSTRUCTION(fnmsubsx);
        table[0x01F] = INSTRUCTION(fnmaddsx);
    }
} table59;

// Table 62
static const struct table62_t : Table<0x4> {
    table62_t() : Table() {
        table[0x000] = INSTRUCTION(std);
        table[0x001] = INSTRUCTION(stdu);
    }
} table62;

// Table 63
static const struct table63_t : Table<0x20> {
    table63_t() : Table() {
        table[0x017] = INSTRUCTION(fselx);
        table[0x019] = INSTRUCTION(fmulx);
        table[0x01C] = INSTRUCTION(fmsubx);
        table[0x01D] = INSTRUCTION(fmaddx);
        table[0x01E] = INSTRUCTION(fnmsubx);
        table[0x01F] = INSTRUCTION(fnmaddx);
    }
} table63;

// Table 63 (Extended)
static const struct table63_ext_t : Table<0x400> {
    table63_ext_t() : Table() {
        table[0x000] = INSTRUCTION(fcmpu);
        table[0x00C] = INSTRUCTION(frspx);
        table[0x00E] = INSTRUCTION(fctiwx);
        table[0x00F] = INSTRUCTION(fctiwzx);
        table[0x012] = INSTRUCTION(fdivx);
        table[0x014] = INSTRUCTION(fsubx);
        table[0x015] = INSTRUCTION(faddx);
        table[0x016] = INSTRUCTION(fsqrtx);
        table[0x01A] = INSTRUCTION(frsqrtex);
        table[0x020] = INSTRUCTION(fcmpo);
        table[0x026] = INSTRUCTION(mtfsb1x);
        table[0x028] = INSTRUCTION(fnegx);
        table[0x040] = INSTRUCTION(mcrfs);
        table[0x046] = INSTRUCTION(mtfsb0x);
        table[0x048] = INSTRUCTION(fmrx);
        table[0x086] = INSTRUCTION(mtfsfix);
        table[0x088] = INSTRUCTION(fnabsx);
        table[0x108] = INSTRUCTION(fabsx);
        table[0x247] = INSTRUCTION(mffsx);
        table[0x2C7] = INSTRUCTION(mtfsfx);
        table[0x32E] = INSTRUCTION(fctidx);
        table[0x32F] = INSTRUCTION(fctidzx);
        table[0x34E] = INSTRUCTION(fcfidx);
    }
} table63_;

/**
 * Return entries from tables
 */
const Entry& get_entry(Instruction code)
{
    if (tablePrimary[code.opcode].type == ENTRY_TABLE) {
        return tablePrimary[code.opcode].caller(code);
    }
    return tablePrimary[code.opcode];
}

const Entry& get_table4(Instruction code)
{
    if (table4[code.op4].type == ENTRY_INVALID) {
        return get_table4_(code);
    }
    return table4[code.op4];
}

const Entry& get_table63(Instruction code)
{
    if (table63[code.op63].type == ENTRY_INVALID) {
        return get_table63_(code);
    }
    return table63[code.op63];
}

const Entry& get_table4_ (Instruction code) { return table4_[code.op4_]; }
const Entry& get_table19 (Instruction code) { return table19[code.op19]; }
const Entry& get_table30 (Instruction code) { return table30[code.op30]; }
const Entry& get_table31 (Instruction code) { return table31[code.op31]; }
const Entry& get_table58 (Instruction code) { return table58[code.op58]; }
const Entry& get_table59 (Instruction code) { return table59[code.op59]; }
const Entry& get_table62 (Instruction code) { return table62[code.op62]; }
const Entry& get_table63_(Instruction code) { return table63_[code.op63_]; }

}  // namespace ppu
}  // namespace cpu
