/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "spu_tables.h"

// Instruction entry
#define INSTRUCTION(name) { ENTRY_INSTRUCTION, nullptr, #name, &Translator::name }

// Table entry
# define TABLE(caller) { ENTRY_TABLE, caller, nullptr, nullptr }

namespace cpu {
namespace frontend {
namespace spu {

/**
 * SPU tables:
 * Initialized as static constant data when Nucleus starts up.
 * Invalid entries are zero-filled, therefore matching the ENTRY_INVALID type.
 * NOTE: Use designated initializers instead as soon as they become available on C++.
 */

template <int N>
struct Table {
    Entry table[N];

    const Entry& operator[] (const U32 i) const {
        return table[i];
    }
};

// Tables
static const struct table4_t : Table<(1 << 4)> {
    table4_t() : Table() {
        table[0x008] = INSTRUCTION(selb);
        table[0x00B] = INSTRUCTION(shufb);
        table[0x00C] = INSTRUCTION(mpya);
        table[0x00D] = INSTRUCTION(fnms);
        table[0x00E] = INSTRUCTION(fma);
        table[0x00F] = INSTRUCTION(fms);
    }
} table4;

static const struct table7_t : Table<(1 << 7)> {
    table7_t() : Table() {
        table[0x008] = INSTRUCTION(hbra);
        table[0x009] = INSTRUCTION(hbrr);
        table[0x021] = INSTRUCTION(ila);
    }
} table7;

static const struct table8_t : Table<(1 << 8)> {
    table8_t() : Table() {
        table[0x004] = INSTRUCTION(ori);
		table[0x005] = INSTRUCTION(orhi);
		table[0x006] = INSTRUCTION(orbi);
		table[0x00C] = INSTRUCTION(sfi);
		table[0x00D] = INSTRUCTION(sfhi);
		table[0x014] = INSTRUCTION(andi);
		table[0x015] = INSTRUCTION(andhi);
		table[0x016] = INSTRUCTION(andbi);
		table[0x01C] = INSTRUCTION(ai);
		table[0x01D] = INSTRUCTION(ahi);
		table[0x024] = INSTRUCTION(stqd);
		table[0x034] = INSTRUCTION(lqd);
		table[0x044] = INSTRUCTION(xori);
		table[0x045] = INSTRUCTION(xorhi);
		table[0x046] = INSTRUCTION(xorbi);
		table[0x04C] = INSTRUCTION(cgti);
		table[0x04D] = INSTRUCTION(cgthi);
		table[0x04E] = INSTRUCTION(cgtbi);
		table[0x04F] = INSTRUCTION(hgti);
		table[0x05C] = INSTRUCTION(clgti);
		table[0x05D] = INSTRUCTION(clgthi);
		table[0x05E] = INSTRUCTION(clgtbi);
		table[0x05F] = INSTRUCTION(hlgti);
		table[0x074] = INSTRUCTION(mpyi);
		table[0x075] = INSTRUCTION(mpyui);
		table[0x07C] = INSTRUCTION(ceqi);
		table[0x07D] = INSTRUCTION(ceqhi);
		table[0x07E] = INSTRUCTION(ceqbi);
		table[0x07F] = INSTRUCTION(heqi);
    }
} table8;

static const struct table9_t : Table<(1 << 9)> {
    table9_t() : Table() {
        table[0x040] = INSTRUCTION(brz);
		table[0x041] = INSTRUCTION(stqa);
		table[0x042] = INSTRUCTION(brnz);
		table[0x044] = INSTRUCTION(brhz);
		table[0x046] = INSTRUCTION(brhnz);
		table[0x047] = INSTRUCTION(stqr);
		table[0x060] = INSTRUCTION(bra);
		table[0x061] = INSTRUCTION(lqa);
		table[0x062] = INSTRUCTION(brasl);
		table[0x064] = INSTRUCTION(br);
		table[0x065] = INSTRUCTION(fsmbi);
		table[0x066] = INSTRUCTION(brsl);
		table[0x067] = INSTRUCTION(lqr);
		table[0x081] = INSTRUCTION(il);
		table[0x082] = INSTRUCTION(ilhu);
		table[0x083] = INSTRUCTION(ilh);
		table[0x0C1] = INSTRUCTION(iohl);
    }
} table9;

static const struct table10_t : Table<(1 << 10)> {
    table10_t() : Table() {
        table[0x1D8] = INSTRUCTION(cflts);
		table[0x1D9] = INSTRUCTION(cfltu);
		table[0x1DA] = INSTRUCTION(csflt);
		table[0x1DB] = INSTRUCTION(cuflt);
    }
} table10;

static const struct table11_t : Table<(1 << 11)> {
    table11_t() : Table() {
		table[0x000] = INSTRUCTION(stop);
		table[0x001] = INSTRUCTION(lnop);
		table[0x002] = INSTRUCTION(sync);
		table[0x003] = INSTRUCTION(dsync);
		table[0x00C] = INSTRUCTION(mfspr);
		table[0x00D] = INSTRUCTION(rdch);
		table[0x00F] = INSTRUCTION(rchcnt);
		table[0x040] = INSTRUCTION(sf);
		table[0x041] = INSTRUCTION(or_);
		table[0x042] = INSTRUCTION(bg);
		table[0x048] = INSTRUCTION(sfh);
		table[0x049] = INSTRUCTION(nor);
		table[0x053] = INSTRUCTION(absdb);
		table[0x058] = INSTRUCTION(rot);
		table[0x059] = INSTRUCTION(rotm);
		table[0x05A] = INSTRUCTION(rotma);
		table[0x05B] = INSTRUCTION(shl);
		table[0x05C] = INSTRUCTION(roth);
		table[0x05D] = INSTRUCTION(rothm);
		table[0x05E] = INSTRUCTION(rotmah);
		table[0x05F] = INSTRUCTION(shlh);
		table[0x078] = INSTRUCTION(roti);
		table[0x079] = INSTRUCTION(rotmi);
		table[0x07A] = INSTRUCTION(rotmai);
		table[0x07B] = INSTRUCTION(shli);
		table[0x07C] = INSTRUCTION(rothi);
		table[0x07D] = INSTRUCTION(rothmi);
		table[0x07E] = INSTRUCTION(rotmahi);
		table[0x07F] = INSTRUCTION(shlhi);
		table[0x0C0] = INSTRUCTION(a);
		table[0x0C1] = INSTRUCTION(and_);
		table[0x0C2] = INSTRUCTION(cg);
		table[0x0C8] = INSTRUCTION(ah);
		table[0x0C9] = INSTRUCTION(nand);
		table[0x0D3] = INSTRUCTION(avgb);
		table[0x10C] = INSTRUCTION(mtspr);
		table[0x10D] = INSTRUCTION(wrch);
		table[0x128] = INSTRUCTION(biz);
		table[0x129] = INSTRUCTION(binz);
		table[0x12A] = INSTRUCTION(bihz);
		table[0x12B] = INSTRUCTION(bihnz);
		table[0x140] = INSTRUCTION(stopd);
		table[0x144] = INSTRUCTION(stqx);
		table[0x1A8] = INSTRUCTION(bi);
		table[0x1A9] = INSTRUCTION(bisl);
		table[0x1AA] = INSTRUCTION(iret);
		table[0x1AB] = INSTRUCTION(bisled);
		table[0x1AC] = INSTRUCTION(hbr);
		table[0x1B0] = INSTRUCTION(gb);
		table[0x1B1] = INSTRUCTION(gbh);
		table[0x1B2] = INSTRUCTION(gbb);
		table[0x1B4] = INSTRUCTION(fsm);
		table[0x1B5] = INSTRUCTION(fsmh);
		table[0x1B6] = INSTRUCTION(fsmb);
		table[0x1B8] = INSTRUCTION(frest);
		table[0x1B9] = INSTRUCTION(frsqest);
		table[0x1C4] = INSTRUCTION(lqx);
		table[0x1CC] = INSTRUCTION(rotqbybi);
		table[0x1CD] = INSTRUCTION(rotqmbybi);
		table[0x1CF] = INSTRUCTION(shlqbybi);
		table[0x1D4] = INSTRUCTION(cbx);
		table[0x1D5] = INSTRUCTION(chx);
		table[0x1D6] = INSTRUCTION(cwx);
		table[0x1D7] = INSTRUCTION(cdx);
		table[0x1D8] = INSTRUCTION(rotqbi);
		table[0x1D9] = INSTRUCTION(rotqmbi);
		table[0x1DB] = INSTRUCTION(shlqbi);
		table[0x1DC] = INSTRUCTION(rotqby);
		table[0x1DD] = INSTRUCTION(rotqmby);
		table[0x1DF] = INSTRUCTION(shlqby);
		table[0x1F0] = INSTRUCTION(orx);
		table[0x1F4] = INSTRUCTION(cbd);
		table[0x1F5] = INSTRUCTION(chd);
		table[0x1F6] = INSTRUCTION(cwd);
		table[0x1F7] = INSTRUCTION(cdd);
		table[0x1F8] = INSTRUCTION(rotqbii);
		table[0x1F9] = INSTRUCTION(rotqmbii);
		table[0x1FB] = INSTRUCTION(shlqbii);
		table[0x1FC] = INSTRUCTION(rotqbyi);
		table[0x1FD] = INSTRUCTION(rotqmbyi);
		table[0x1FF] = INSTRUCTION(shlqbyi);
		table[0x201] = INSTRUCTION(nop);
		table[0x240] = INSTRUCTION(cgt);
		table[0x241] = INSTRUCTION(xor_);
		table[0x248] = INSTRUCTION(cgth);
		table[0x249] = INSTRUCTION(eqv);
		table[0x250] = INSTRUCTION(cgtb);
		table[0x253] = INSTRUCTION(sumb);
		table[0x258] = INSTRUCTION(hgt);
		table[0x2A5] = INSTRUCTION(clz);
		table[0x2A6] = INSTRUCTION(xswd);
		table[0x2AE] = INSTRUCTION(xshw);
		table[0x2B4] = INSTRUCTION(cntb);
		table[0x2B6] = INSTRUCTION(xsbh);
		table[0x2C0] = INSTRUCTION(clgt);
		table[0x2C1] = INSTRUCTION(andc);
		table[0x2C2] = INSTRUCTION(fcgt);
		table[0x2C3] = INSTRUCTION(dfcgt);
		table[0x2C4] = INSTRUCTION(fa);
		table[0x2C5] = INSTRUCTION(fs);
		table[0x2C6] = INSTRUCTION(fm);
		table[0x2C8] = INSTRUCTION(clgth);
		table[0x2C9] = INSTRUCTION(orc);
		table[0x2CA] = INSTRUCTION(fcmgt);
		table[0x2CB] = INSTRUCTION(dfcmgt);
		table[0x2CC] = INSTRUCTION(dfa);
		table[0x2CD] = INSTRUCTION(dfs);
		table[0x2CE] = INSTRUCTION(dfm);
		table[0x2D0] = INSTRUCTION(clgtb);
		table[0x2D8] = INSTRUCTION(hlgt);
		table[0x35C] = INSTRUCTION(dfma);
		table[0x35D] = INSTRUCTION(dfms);
		table[0x35E] = INSTRUCTION(dfnms);
		table[0x35F] = INSTRUCTION(dfnma);
		table[0x3C0] = INSTRUCTION(ceq);
		table[0x3CE] = INSTRUCTION(mpyhhu);
		table[0x340] = INSTRUCTION(addx);
		table[0x341] = INSTRUCTION(sfx);
		table[0x342] = INSTRUCTION(cgx);
		table[0x343] = INSTRUCTION(bgx);
		table[0x346] = INSTRUCTION(mpyhha);
		table[0x34E] = INSTRUCTION(mpyhhau);
		table[0x398] = INSTRUCTION(fscrrd);
		table[0x3B8] = INSTRUCTION(fesd);
		table[0x3B9] = INSTRUCTION(frds);
		table[0x3BA] = INSTRUCTION(fscrwr);
		table[0x3BF] = INSTRUCTION(dftsv);
		table[0x3C2] = INSTRUCTION(fceq);
		table[0x3C3] = INSTRUCTION(dfceq);
		table[0x3C4] = INSTRUCTION(mpy);
		table[0x3C5] = INSTRUCTION(mpyh);
		table[0x3C6] = INSTRUCTION(mpyhh);
		table[0x3C7] = INSTRUCTION(mpys);
		table[0x3C8] = INSTRUCTION(ceqh);
		table[0x3CA] = INSTRUCTION(fcmeq);
		table[0x3CB] = INSTRUCTION(dfcmeq);
		table[0x3CC] = INSTRUCTION(mpyu);
		table[0x3D0] = INSTRUCTION(ceqb);
		table[0x3D4] = INSTRUCTION(fi);
		table[0x3D8] = INSTRUCTION(heq);
    }
} table11;


/**
 * Return entries from tables
 */
const Entry& get_entry(Instruction code) {
    if (table4[code.opcode].type != ENTRY_INVALID) {
        return table4[code.opcode];
    }
    if (table7[code.opcode].type != ENTRY_INVALID) {
        return table7[code.opcode];
    }
    if (table8[code.opcode].type != ENTRY_INVALID) {
        return table8[code.opcode];
    }
    if (table9[code.opcode].type != ENTRY_INVALID) {
        return table9[code.opcode];
    }
    if (table10[code.opcode].type != ENTRY_INVALID) {
        return table10[code.opcode];
    }
    if (table11[code.opcode].type != ENTRY_INVALID) {
        return table11[code.opcode];
    }
    return Entry{};
}

}  // namespace spu
}  // namespace frontend
}  // namespace cpu
