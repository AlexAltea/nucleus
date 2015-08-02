/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/builder.h"
#include "nucleus/cpu/hir/value.h"
#include "nucleus/cpu/frontend/frontend_recompiler.h"
#include "nucleus/cpu/frontend/spu/spu_instruction.h"

namespace cpu {
namespace spu {

class Recompiler : public frontend::IRecompiler<U32> {
public:
    /**
     * SPU Register access
     */
    template <typename T>
    hir::Value<T, 128 / T::size> getGPR(int index);

    template <typename T>
    void setGPR(int index, hir::Value<T, 128 / T::size> value);

    /**
     * SPU Instructions:
     * Organized according to chapter 3 to 11 of the Synergistic Processor Unit
     * Instruction Set Architecture (Version 1.2 / January 27, 2007).
     */

    // Memory-Load/Store Instructions (Chapter 3)
    void cbd(Instruction code);
    void cbx(Instruction code);
    void cdd(Instruction code);
    void cdx(Instruction code);
    void chd(Instruction code);
    void chx(Instruction code);
    void cwd(Instruction code);
    void cwx(Instruction code);
    void lqa(Instruction code);
    void lqd(Instruction code);
    void lqr(Instruction code);
    void lqx(Instruction code);
    void stqa(Instruction code);
    void stqd(Instruction code);
    void stqr(Instruction code);
    void stqx(Instruction code);

    // Constant-Formation Instructions (Chapter 4)
    void fsmbi(Instruction code);
    void il(Instruction code);
    void ila(Instruction code);
    void ilh(Instruction code);
    void ilhu(Instruction code);
    void iohl(Instruction code);

    // Integer and Logical Instructions (Chapter 5)
    void a(Instruction code);
    void absdb(Instruction code);
    void addx(Instruction code);
    void ah(Instruction code);
    void ahi(Instruction code);
    void ai(Instruction code);
    void and(Instruction code);
    void andc(Instruction code);
    void andbi(Instruction code);
    void andhi(Instruction code);
    void andi(Instruction code);
    void avgb(Instruction code);
    void bg(Instruction code);
    void bgx(Instruction code);
    void cg(Instruction code);
    void cgx(Instruction code);
    void clz(Instruction code);
    void cntb(Instruction code);
    void eqv(Instruction code);
    void fsm(Instruction code);
    void fsmb(Instruction code);
    void fsmh(Instruction code);
    void gb(Instruction code);
    void gbb(Instruction code);
    void gbh(Instruction code);
    void mpy(Instruction code);
    void mpya(Instruction code);
    void mpyh(Instruction code);
    void mpyhh(Instruction code);
    void mpyhha(Instruction code);
    void mpyhhau(Instruction code);
    void mpyhhu(Instruction code);
    void mpyi(Instruction code);
    void mpys(Instruction code);
    void mpyu(Instruction code);
    void mpyui(Instruction code);
    void nand(Instruction code);
    void nor(Instruction code);
    void or(Instruction code);
    void orbi(Instruction code);
    void orc(Instruction code);
    void orhi(Instruction code);
    void ori(Instruction code);
    void orx(Instruction code);
    void selb(Instruction code);
    void sf(Instruction code);
    void sfh(Instruction code);
    void sfhi(Instruction code);
    void sfi(Instruction code);
    void sfx(Instruction code);
    void shufb(Instruction code);
    void sumb(Instruction code);
    void xor(Instruction code);
    void xorbi(Instruction code);
    void xorhi(Instruction code);
    void xori(Instruction code);
    void xsbh(Instruction code);
    void xshw(Instruction code);
    void xswd(Instruction code);

    // Shift and Rotate Instructions (Chapter 6)
    void shl(Instruction code);
    void shlh(Instruction code);
    void shlhi(Instruction code);
    void shli(Instruction code);
    void shlqbi(Instruction code);
    void shlqbii(Instruction code);
    void shlqby(Instruction code);
    void shlqbybi(Instruction code);
    void shlqbyi(Instruction code);
    void rot(Instruction code);
    void roth(Instruction code);
    void rothi(Instruction code);
    void rothm(Instruction code);
    void rothmi(Instruction code);
    void roti(Instruction code);
    void rotm(Instruction code);
    void rotma(Instruction code);
    void rotmah(Instruction code);
    void rotmahi(Instruction code);
    void rotmai(Instruction code);
    void rotmi(Instruction code);
    void rotqbi(Instruction code);
    void rotqbii(Instruction code);
    void rotqby(Instruction code);
    void rotqbybi(Instruction code);
    void rotqbyi(Instruction code);
    void rotqmbi(Instruction code);
    void rotqmbii(Instruction code);
    void rotqmby(Instruction code);
    void rotqmbybi(Instruction code);
    void rotqmbyi(Instruction code);

    // Compare, Branch and Halt Instructions (Chapter 7)
    void bi(Instruction code);
    void bihnz(Instruction code);
    void bihz(Instruction code);
    void binz(Instruction code);
    void bisl(Instruction code);
    void bisled(Instruction code);
    void biz(Instruction code);
    void br(Instruction code);
    void bra(Instruction code);
    void brasl(Instruction code);
    void brhnz(Instruction code);
    void brhz(Instruction code);
    void brnz(Instruction code);
    void brsl(Instruction code);
    void brz(Instruction code);
    void ceq(Instruction code);
    void ceqb(Instruction code);
    void ceqbi(Instruction code);
    void ceqh(Instruction code);
    void ceqhi(Instruction code);
    void ceqi(Instruction code);
    void cgt(Instruction code);
    void cgtb(Instruction code);
    void cgtbi(Instruction code);
    void cgth(Instruction code);
    void cgthi(Instruction code);
    void cgti(Instruction code);
    void clgt(Instruction code);
    void clgtb(Instruction code);
    void clgtbi(Instruction code);
    void clgth(Instruction code);
    void clgthi(Instruction code);
    void clgti(Instruction code);
    void heq(Instruction code);
    void heqi(Instruction code);
    void hgt(Instruction code);
    void hgti(Instruction code);
    void hlgt(Instruction code);
    void hlgti(Instruction code);
    void iret(Instruction code);

    // Hint-for-Branch Instructions (Chapter 8)
    void hbr(Instruction code);
    void hbra(Instruction code);
    void hbrr(Instruction code);

    // Floating-Point Instructions (Chapter 9)
    void cflts(Instruction code);
    void cfltu(Instruction code);
    void csflt(Instruction code);
    void cuflt(Instruction code);
    void dfa(Instruction code);
    void dfceq(Instruction code);
    void dfcgt(Instruction code);
    void dfcmeq(Instruction code);
    void dfcmgt(Instruction code);
    void dfm(Instruction code);
    void dfma(Instruction code);
    void dfms(Instruction code);
    void dfnma(Instruction code);
    void dfnms(Instruction code);
    void dfs(Instruction code);
    void dftsv(Instruction code);
    void fa(Instruction code);
    void fceq(Instruction code);
    void fcgt(Instruction code);
    void fcmeq(Instruction code);
    void fcmgt(Instruction code);
    void fesd(Instruction code);
    void fi(Instruction code);
    void fm(Instruction code);
    void fma(Instruction code);
    void fms(Instruction code);
    void fnms(Instruction code);
    void frds(Instruction code);
    void frest(Instruction code);
    void frsqest(Instruction code);
    void fs(Instruction code);
    void fscrrd(Instruction code);
    void fscrwr(Instruction code);

    // Control Instructions (Chapter 10)
    void dsync(Instruction code);
    void lnop(Instruction code);
    void mfspr(Instruction code);
    void mtspr(Instruction code);
    void nop(Instruction code);
    void stop(Instruction code);
    void stopd(Instruction code);
    void sync(Instruction code);

    // Channel Instructions (Chapter 11)
    void rchcnt(Instruction code);
    void rdch(Instruction code);
    void wrch(Instruction code);
};

}  // namespace spu
}  // namespace cpu
