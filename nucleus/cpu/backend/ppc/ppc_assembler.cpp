/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppc_assembler.h"

namespace cpu {
namespace backend {
namespace ppc {

void PPCAssembler::emit(U32 instruction) {
    auto* addr = static_cast<BE<U32>*>(curAddr);
    *addr++ = instruction;
}

// Emit instruction form
void PPCAssembler::emitFormXO(U32 instruction, Operand d, Operand a, Operand b) {
    const U32 rd = (d & 0x1F) << 21;
    const U32 ra = (a & 0x1F) << 16;
    const U32 rb = (b & 0x1F) << 11;
    emit(instruction | rd | ra | rb);
}

// PPC instructions
void PPCAssembler::add(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000214, rd, ra, rb); }
void PPCAssembler::add_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000215, rd, ra, rb); }
void PPCAssembler::addc(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000014, rd, ra, rb); }
void PPCAssembler::addc_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000015, rd, ra, rb); }
void PPCAssembler::addco(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000414, rd, ra, rb); }
void PPCAssembler::addco_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000415, rd, ra, rb); }
void PPCAssembler::adde(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000114, rd, ra, rb); }
void PPCAssembler::adde_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000115, rd, ra, rb); }
void PPCAssembler::addeo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000514, rd, ra, rb); }
void PPCAssembler::addeo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000515, rd, ra, rb); }
void PPCAssembler::addme(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0001D4, rd, ra, 0); }
void PPCAssembler::addme_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0001D5, rd, ra, 0); }
void PPCAssembler::addmeo(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0005D4, rd, ra, 0); }
void PPCAssembler::addmeo_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0005D5, rd, ra, 0); }
void PPCAssembler::addo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000614, rd, ra, rb); }
void PPCAssembler::addo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000615, rd, ra, rb); }
void PPCAssembler::addze(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000194, rd, ra, 0); }
void PPCAssembler::addze_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000195, rd, ra, 0); }
void PPCAssembler::addzeo(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000594, rd, ra, 0); }
void PPCAssembler::addzeo_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000595, rd, ra, 0); }
void PPCAssembler::divd(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0003D2, rd, ra, rb); }
void PPCAssembler::divd_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0003D3, rd, ra, rb); }
void PPCAssembler::divdo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0007D2, rd, ra, rb); }
void PPCAssembler::divdo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0007D3, rd, ra, rb); }
void PPCAssembler::divdu(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000392, rd, ra, rb); }
void PPCAssembler::divdu_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000393, rd, ra, rb); }
void PPCAssembler::divduo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000792, rd, ra, rb); }
void PPCAssembler::divduo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000793, rd, ra, rb); }
void PPCAssembler::divw(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0003D6, rd, ra, rb); }
void PPCAssembler::divw_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0003D7, rd, ra, rb); }
void PPCAssembler::divwo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0007D6, rd, ra, rb); }
void PPCAssembler::divwo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0007D7, rd, ra, rb); }
void PPCAssembler::divwu(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000396, rd, ra, rb); }
void PPCAssembler::divwu_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000397, rd, ra, rb); }
void PPCAssembler::divwuo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000796, rd, ra, rb); }
void PPCAssembler::divwuo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000797, rd, ra, rb); }
void PPCAssembler::lbz(RegGPR rd, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::lbzu(RegGPR rd, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::lbzux(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lbzx(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::ld(RegGPR rd, RegGPR ra, U16 ds = 0) {  }
void PPCAssembler::ldarx(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::ldbrx() {  }
void PPCAssembler::ldu(RegGPR rd, RegGPR ra, U16 ds = 0) {  }
void PPCAssembler::ldux(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::ldx(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lfd(RegFPR frd, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::lfdu(RegFPR frd, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::lfdux(RegFPR frd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lfdx(RegFPR frd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lfs(RegFPR frd, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::lfsu(RegFPR frd, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::lfsux(RegFPR frd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lfsx(RegFPR frd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lha(RegGPR rd, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::lhau(RegGPR rd, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::lhaux(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lhax(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lhbrx(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lhz(RegGPR rd, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::lhzu(RegGPR rd, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::lhzux(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lhzx(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::li(RegGPR rd, S16 simm) {  }
void PPCAssembler::lis(RegGPR rd, S16 simm) {  }
void PPCAssembler::lmw(RegGPR rd, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::lswi() {  }
void PPCAssembler::lswx(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lwa(RegGPR rd, RegGPR ra, U16 ds = 0) {  }
void PPCAssembler::lwarx(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lwaux(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lwax(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lwbrx(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lwz(RegGPR rd, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::lwzu(RegGPR rd, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::lwzux(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::lwzx(RegGPR rd, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::mcrf(RegCR crfd, RegCR crfs) {  }
void PPCAssembler::mcrfs(RegCR crfd, RegCR crfs) {  }
void PPCAssembler::mffs() {  }
void PPCAssembler::mffs_() {  }
void PPCAssembler::mfocrf() {  }
void PPCAssembler::mfspr() {  }
void PPCAssembler::mftb() {  }
void PPCAssembler::mr(RegGPR ra, RegGPR rs) {  }
void PPCAssembler::mtfsb0x() {  }
void PPCAssembler::mtfsb1x() {  }
void PPCAssembler::mtfsfix() {  }
void PPCAssembler::mtfsfx() {  }
void PPCAssembler::mtocrf() {  }
void PPCAssembler::mtspr() {  }
void PPCAssembler::mulhd(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000092, rd, ra, rb); }
void PPCAssembler::mulhd_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000093, rd, ra, rb); }
void PPCAssembler::mulhdu(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000012, rd, ra, rb); }
void PPCAssembler::mulhdu_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000013, rd, ra, rb); }
void PPCAssembler::mulhw(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000096, rd, ra, rb); }
void PPCAssembler::mulhw_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000097, rd, ra, rb); }
void PPCAssembler::mulhwu(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000016, rd, ra, rb); }
void PPCAssembler::mulhwu_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000017, rd, ra, rb); }
void PPCAssembler::mulld(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0001D2, rd, ra, rb); }
void PPCAssembler::mulld_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0001D3, rd, ra, rb); }
void PPCAssembler::mulldo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0005D2, rd, ra, rb); }
void PPCAssembler::mulldo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0005D3, rd, ra, rb); }
void PPCAssembler::mulli(RegGPR rd, RegGPR ra, S16 simm) {  }
void PPCAssembler::mullw(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0001D6, rd, ra, rb); }
void PPCAssembler::mullw_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0001D7, rd, ra, rb); }
void PPCAssembler::mullwo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0005D6, rd, ra, rb); }
void PPCAssembler::mullwo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C0005D7, rd, ra, rb); }
void PPCAssembler::nand(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::nand_(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::neg(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0000D0, rd, ra, 0); }
void PPCAssembler::neg_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0000D1, rd, ra, 0); }
void PPCAssembler::nego(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0004D0, rd, ra, 0); }
void PPCAssembler::nego_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0004D1, rd, ra, 0); }
void PPCAssembler::nop() {  }
void PPCAssembler::nor(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::nor_(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::not_(RegGPR rd, RegGPR rs ) {  }
void PPCAssembler::or_(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::or__(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::orc(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::orc_(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::ori(RegGPR ra, RegGPR rs, U16 uimm) {  }
void PPCAssembler::oris(RegGPR ra, RegGPR rs, U16 uimm) {  }
void PPCAssembler::rldcl(RegGPR ra, RegGPR rs, RegGPR rb, U8 mb) {  }
void PPCAssembler::rldcl_(RegGPR ra, RegGPR rs, RegGPR rb, U8 mb) {  }
void PPCAssembler::rldcr(RegGPR ra, RegGPR rs, RegGPR rb, U8 me) {  }
void PPCAssembler::rldcr_(RegGPR ra, RegGPR rs, RegGPR rb, U8 me) {  }
void PPCAssembler::rldic(RegGPR ra, RegGPR rs, U8 sh, U8 mb) {  }
void PPCAssembler::rldic_(RegGPR ra, RegGPR rs, U8 sh, U8 mb) {  }
void PPCAssembler::rldicl(RegGPR ra, RegGPR rs, U8 sh, U8 mb) {  }
void PPCAssembler::rldicl_(RegGPR ra, RegGPR rs, U8 sh, U8 mb) {  }
void PPCAssembler::rldicr(RegGPR ra, RegGPR rs, U8 sh, U8 me) {  }
void PPCAssembler::rldicr_(RegGPR ra, RegGPR rs, U8 sh, U8 me) {  }
void PPCAssembler::rldimi(RegGPR ra, RegGPR rs, U8 sh, U8 mb) {  }
void PPCAssembler::rldimi_(RegGPR ra, RegGPR rs, U8 sh, U8 mb) {  }
void PPCAssembler::rlwimi(RegGPR ra, RegGPR rs, U8 sh, U8 mb, U8 me) {  }
void PPCAssembler::rlwimi_(RegGPR ra, RegGPR rs, U8 sh, U8 mb, U8 me) {  }
void PPCAssembler::rlwinm(RegGPR ra, RegGPR rs, U8 sh, U8 mb, U8 me) {  }
void PPCAssembler::rlwinm_(RegGPR ra, RegGPR rs, U8 sh, U8 mb, U8 me) {  }
void PPCAssembler::rlwnm(RegGPR ra, RegGPR rs, RegGPR rb, U8 mb, U8 me) {  }
void PPCAssembler::rlwnm_(RegGPR ra, RegGPR rs, RegGPR rb, U8 mb, U8 me) {  }
void PPCAssembler::sc() {  }
void PPCAssembler::sld(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::sld_(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::slw(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::slw_(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::srad(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::srad_(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::sradi(RegGPR ra, RegGPR rs, U8 sh) {  }
void PPCAssembler::sradi_(RegGPR ra, RegGPR rs, U8 sh) {  }
void PPCAssembler::sraw(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::sraw_(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::srawi(RegGPR ra, RegGPR rs, U8 sh) {  }
void PPCAssembler::srawi_(RegGPR ra, RegGPR rs, U8 sh) {  }
void PPCAssembler::srd(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::srd_(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::srw(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::srw_(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::stb(RegGPR rs, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::stbu(RegGPR rs, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::stbux(RegGPR rs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::stbx(RegGPR rs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::std(RegGPR rs, RegGPR ra, U16 ds = 0) {  }
void PPCAssembler::stdcx_(RegGPR rs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::stdu(RegGPR rs, RegGPR ra, U16 ds = 0) {  }
void PPCAssembler::stdux(RegGPR rs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::stdx(RegGPR rs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::stfd(RegFPR rs, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::stfdu(RegFPR frs, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::stfdux(RegFPR frs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::stfdx(RegFPR frs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::stfiwx(RegFPR frs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::stfs(RegFPR frs, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::stfsu(RegFPR frs, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::stfsux(RegFPR frs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::stfsx(RegFPR frs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::sth(RegGPR rs, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::sthbrx(RegGPR rs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::sthu(RegGPR rs, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::sthux(RegGPR rs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::sthx(RegGPR rs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::stmw(RegGPR rs, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::stswi() {  }
void PPCAssembler::stswx(RegGPR rs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::stw(RegGPR rs, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::stwbrx(RegGPR rs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::stwcx_(RegGPR rs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::stwu(RegGPR rs, RegGPR ra, U16 d = 0) {  }
void PPCAssembler::stwux(RegGPR rs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::stwx(RegGPR rs, RegGPR ra, RegGPR rb) {  }
void PPCAssembler::subf(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000050, rd, ra, rb); }
void PPCAssembler::subf_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000051, rd, ra, rb); }
void PPCAssembler::subfc(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000010, rd, ra, rb); }
void PPCAssembler::subfc_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000011, rd, ra, rb); }
void PPCAssembler::subfco(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000410, rd, ra, rb); }
void PPCAssembler::subfco_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000411, rd, ra, rb); }
void PPCAssembler::subfe(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000110, rd, ra, rb); }
void PPCAssembler::subfe_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000111, rd, ra, rb); }
void PPCAssembler::subfeo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000510, rd, ra, rb); }
void PPCAssembler::subfeo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000511, rd, ra, rb); }
void PPCAssembler::subfic(RegGPR rd, RegGPR ra, S16 simm) {  }
void PPCAssembler::subfme(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0001D0, rd, ra, 0); }
void PPCAssembler::subfme_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0001D1, rd, ra, 0); }
void PPCAssembler::subfmeo(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0005D0, rd, ra, 0); }
void PPCAssembler::subfmeo_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C0005D1, rd, ra, 0); }
void PPCAssembler::subfo(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000450, rd, ra, rb); }
void PPCAssembler::subfo_(RegGPR rd, RegGPR ra, RegGPR rb) { emitFormXO(0x7C000451, rd, ra, rb); }
void PPCAssembler::subfze(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000190, rd, ra, 0); }
void PPCAssembler::subfze_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000191, rd, ra, 0); }
void PPCAssembler::subfzeo(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000590, rd, ra, 0); }
void PPCAssembler::subfzeo_(RegGPR rd, RegGPR ra) { emitFormXO(0x7C000591, rd, ra, 0); }
void PPCAssembler::sync() {  }
void PPCAssembler::td() {  }
void PPCAssembler::tdi() {  }
void PPCAssembler::tw() {  }
void PPCAssembler::twi() {  }
void PPCAssembler::xor_(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::xor__(RegGPR ra, RegGPR rs, RegGPR rb) {  }
void PPCAssembler::xori(RegGPR ra, RegGPR rs, U16 uimm) {  }
void PPCAssembler::xoris(RegGPR ra, RegGPR rs, U16 uimm) {  }

}  // namespace ppc
}  // namespace backend
}  // namespace cpu
