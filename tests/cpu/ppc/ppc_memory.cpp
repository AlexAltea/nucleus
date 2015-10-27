/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "tests/cpu/test_ppc.h"

/**
 * PPC64 Instructions:
 *  - UISA: Load and Store Instructions (Section: 4.2.3)
 *  - UISA: Memory Synchronization Instructions (Section: 4.2.6)
 *  - VEA: Memory Synchronization Instructions (Section: 4.3.2)
 */

void PPCTestRunner::lbz() {
    // Load Byte and Zero
    TEST_INSTRUCTION(test_lbz, RAIndex, RA, D, RD, {
        state.r[RAIndex] = RA;
        state.r[2] = 0xFFFFFFFFFFFFFFFFULL;
        run(lbz(r2, RAIndex, D));
        expect(state.r[2] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lbz(r1, addr + 0x00, 0x0000, 0x0000000000000001ULL);
    test_lbz(r1, addr + 0x00, 0x0010, 0x00000000000000FFULL);
    test_lbz(r1, addr + 0x10, 0xFFFF, 0x0000000000000000ULL);
    test_lbz(r1, addr + 0x08, 0xFFFF, 0x00000000000000EFULL);
    memory->free(addr);
}

void PPCTestRunner::lbzu() {
    // Load Byte and Zero with Update
    TEST_INSTRUCTION(test_lbzu, RAIndex, oldRA, D, RD, newRA, {
        state.r[RAIndex] = oldRA;
        state.r[2] = 0xFFFFFFFFFFFFFFFFULL;
        run(lbzu(r2, RAIndex, D));
        expect(state.r[RAIndex] == newRA);
        expect(state.r[2] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lbzu(r1, addr + 0x00, 0x0000, 0x0000000000000001ULL, addr + 0x00);
    test_lbzu(r1, addr + 0x00, 0x0010, 0x00000000000000FFULL, addr + 0x10);
    test_lbzu(r1, addr + 0x10, 0xFFFF, 0x0000000000000000ULL, addr + 0x0F);
    test_lbzu(r1, addr + 0x08, 0xFFFF, 0x00000000000000EFULL, addr + 0x07);
    memory->free(addr);
}

void PPCTestRunner::lbzux() {
    // Load Byte and Zero with Update Indexed
    TEST_INSTRUCTION(test_lbzux, RAIndex, oldRA, RB, RD, newRA, {
        state.r[RAIndex] = oldRA;
        state.r[2] = RB;
        state.r[3] = 0xFFFFFFFFFFFFFFFFULL;
        run(lbzux(r3, RAIndex, r2));
        expect(state.r[RAIndex] == newRA);
        expect(state.r[3] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lbzux(r1, addr + 0x00, 0x0000000000000000ULL, 0x0000000000000001ULL, addr + 0x00);
    test_lbzux(r1, addr + 0x00, 0x0000000000000010ULL, 0x00000000000000FFULL, addr + 0x10);
    test_lbzux(r1, addr + 0x10, 0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL, addr + 0x0F);
    test_lbzux(r1, addr + 0x08, 0xFFFFFFFFFFFFFFFFULL, 0x00000000000000EFULL, addr + 0x07);
    memory->free(addr);
}

void PPCTestRunner::lbzx() {
    // Load Byte and Zero Indexed
    TEST_INSTRUCTION(test_lbzx, RAIndex, RA, RB, RD, {
        state.r[RAIndex] = RA;
        state.r[2] = RB;
        state.r[3] = 0xFFFFFFFFFFFFFFFFULL;
        run(lbzx(r3, RAIndex, r2));
        expect(state.r[3] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lbzx(r1, addr + 0x00, 0x0000000000000000ULL, 0x0000000000000001ULL);
    test_lbzx(r1, addr + 0x00, 0x0000000000000010ULL, 0x00000000000000FFULL);
    test_lbzx(r1, addr + 0x10, 0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL);
    test_lbzx(r1, addr + 0x08, 0xFFFFFFFFFFFFFFFFULL, 0x00000000000000EFULL);
    test_lbzx(r0, 0x8000000000000000ULL, addr + 0x00, 0x0000000000000001ULL);
    test_lbzx(r0, 0x8000000000000000ULL, addr + 0x10, 0x00000000000000FFULL);
    test_lbzx(r0, 0x8000000000000000ULL, addr + 0x0F, 0x0000000000000000ULL);
    test_lbzx(r0, 0x8000000000000000ULL, addr + 0x07, 0x00000000000000EFULL);
    memory->free(addr);
}

void PPCTestRunner::ld() {
    // Load Doubleword
    TEST_INSTRUCTION(test_ld, RAIndex, RA, D, RD, {
        state.r[RAIndex] = RA;
        run(ld(r2, RAIndex, D));
        expect(state.r[2] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_ld(r1, addr + 0x00, 0x0000, 0x0123456789ABCDEFULL);
    test_ld(r1, addr + 0x00, 0x0010, 0xFFFFFFFFFFFFFFFFULL);
    test_ld(r1, addr + 0x10, 0xFFF8, 0x0000000000000000ULL);
    test_ld(r1, addr + 0x18, 0x0004, 0xFFFFFFFF0000FFFFULL);
    memory->free(addr);
}

void PPCTestRunner::ldbrx() {
}

void PPCTestRunner::ldu() {
    // Load Doubleword with Update
    TEST_INSTRUCTION(test_ldu, RAIndex, oldRA, D, RD, newRA, {
        state.r[RAIndex] = oldRA;
        run(ldu(r2, RAIndex, D));
        expect(state.r[RAIndex] == newRA);
        expect(state.r[2] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_ldu(r1, addr + 0x00, 0x0000, 0x0123456789ABCDEFULL, addr + 0x00);
    test_ldu(r1, addr + 0x00, 0x0010, 0xFFFFFFFFFFFFFFFFULL, addr + 0x10);
    test_ldu(r1, addr + 0x10, 0xFFF8, 0x0000000000000000ULL, addr + 0x08);
    test_ldu(r1, addr + 0x18, 0x0004, 0xFFFFFFFF0000FFFFULL, addr + 0x1C);
    memory->free(addr);
}

void PPCTestRunner::ldux() {
    // Load Doubleword with Update Indexed
    TEST_INSTRUCTION(test_ldux, RAIndex, oldRA, RB, RD, newRA, {
        state.r[RAIndex] = oldRA;
        state.r[2] = RB;
        run(ldux(r3, RAIndex, r2));
        expect(state.r[RAIndex] == newRA);
        expect(state.r[3] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_ldux(r1, addr + 0x00, 0x0000000000000000, 0x0123456789ABCDEFULL, addr + 0x00);
    test_ldux(r1, addr + 0x00, 0x0000000000000010, 0xFFFFFFFFFFFFFFFFULL, addr + 0x10);
    test_ldux(r1, addr + 0x10, 0xFFFFFFFFFFFFFFF8, 0x0000000000000000ULL, addr + 0x08);
    test_ldux(r1, addr + 0x18, 0x0000000000000004, 0xFFFFFFFF0000FFFFULL, addr + 0x1C);
    memory->free(addr);
}

void PPCTestRunner::ldx() {
    // Load Doubleword Indexed
    TEST_INSTRUCTION(test_ldx, RAIndex, RA, RB, RD, {
        state.r[RAIndex] = RA;
        state.r[2] = RB;
        run(ldx(r3, RAIndex, r2));
        expect(state.r[3] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_ldx(r1, addr + 0x00, 0x0000000000000000, 0x0123456789ABCDEFULL);
    test_ldx(r1, addr + 0x00, 0x0000000000000010, 0xFFFFFFFFFFFFFFFFULL);
    test_ldx(r1, addr + 0x10, 0xFFFFFFFFFFFFFFF8, 0x0000000000000000ULL);
    test_ldx(r1, addr + 0x18, 0x0000000000000004, 0xFFFFFFFF0000FFFFULL);
    test_ldx(r0, 0x8000000000000000, addr + 0x00, 0x0123456789ABCDEFULL);
    test_ldx(r0, 0x8000000000000000, addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    test_ldx(r0, 0x8000000000000000, addr + 0x08, 0x0000000000000000ULL);
    test_ldx(r0, 0x8000000000000000, addr + 0x1C, 0xFFFFFFFF0000FFFFULL);
    memory->free(addr);
}

void PPCTestRunner::lfd() {
    // Load Floating-Point Double
    TEST_INSTRUCTION(test_lfd, RAIndex, RA, D, FRD, {
        state.r[RAIndex] = RA;
        run(lfd(f0, RAIndex, D));
        expect(reinterpret_cast<U64&>(state.f[0]) == FRD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lfd(r1, addr + 0x00, 0x0000, 0x0123456789ABCDEFULL);
    test_lfd(r1, addr + 0x00, 0x0010, 0xFFFFFFFFFFFFFFFFULL);
    test_lfd(r1, addr + 0x10, 0xFFF8, 0x0000000000000000ULL);
    test_lfd(r1, addr + 0x18, 0x0004, 0xFFFFFFFF0000FFFFULL);
    memory->free(addr);
}

void PPCTestRunner::lfdu() {
    // Load Floating-Point Double with Update
    TEST_INSTRUCTION(test_lfdu, RAIndex, oldRA, D, FRD, newRA, {
        state.r[RAIndex] = oldRA;
        run(lfdu(f0, RAIndex, D));
        expect(state.r[RAIndex] == newRA);
        expect(reinterpret_cast<U64&>(state.f[0]) == FRD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lfdu(r1, addr + 0x00, 0x0000, 0x0123456789ABCDEFULL, addr + 0x00);
    test_lfdu(r1, addr + 0x00, 0x0010, 0xFFFFFFFFFFFFFFFFULL, addr + 0x10);
    test_lfdu(r1, addr + 0x10, 0xFFF8, 0x0000000000000000ULL, addr + 0x08);
    test_lfdu(r1, addr + 0x18, 0x0004, 0xFFFFFFFF0000FFFFULL, addr + 0x1C);
    memory->free(addr);
}

void PPCTestRunner::lfdux() {
    // Load Floating-Point Double with Update Indexed
    TEST_INSTRUCTION(test_lfdux, RAIndex, oldRA, RB, FRD, newRA, {
        state.r[RAIndex] = oldRA;
        state.r[2] = RB;
        run(lfdux(f0, RAIndex, r2));
        expect(state.r[RAIndex] == newRA);
        expect(reinterpret_cast<U64&>(state.f[0]) == FRD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lfdux(r1, addr + 0x00, 0x0000000000000000, 0x0123456789ABCDEFULL, addr + 0x00);
    test_lfdux(r1, addr + 0x00, 0x0000000000000010, 0xFFFFFFFFFFFFFFFFULL, addr + 0x10);
    test_lfdux(r1, addr + 0x10, 0xFFFFFFFFFFFFFFF8, 0x0000000000000000ULL, addr + 0x08);
    test_lfdux(r1, addr + 0x18, 0x0000000000000004, 0xFFFFFFFF0000FFFFULL, addr + 0x1C);
    memory->free(addr);
}

void PPCTestRunner::lfdx() {
    // Load Floating-Point Double Indexed
    TEST_INSTRUCTION(test_lfdx, RAIndex, RA, RB, FRD, {
        state.r[RAIndex] = RA;
        state.r[2] = RB;
        run(lfdx(f0, RAIndex, r2));
        expect(reinterpret_cast<U64&>(state.f[0]) == FRD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lfdx(r1, addr + 0x00, 0x0000000000000000, 0x0123456789ABCDEFULL);
    test_lfdx(r1, addr + 0x00, 0x0000000000000010, 0xFFFFFFFFFFFFFFFFULL);
    test_lfdx(r1, addr + 0x10, 0xFFFFFFFFFFFFFFF8, 0x0000000000000000ULL);
    test_lfdx(r1, addr + 0x18, 0x0000000000000004, 0xFFFFFFFF0000FFFFULL);
    test_lfdx(r0, 0x8000000000000000, addr + 0x00, 0x0123456789ABCDEFULL);
    test_lfdx(r0, 0x8000000000000000, addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    test_lfdx(r0, 0x8000000000000000, addr + 0x08, 0x0000000000000000ULL);
    test_lfdx(r0, 0x8000000000000000, addr + 0x1C, 0xFFFFFFFF0000FFFFULL);
    memory->free(addr);
}

void PPCTestRunner::lfs() {
}

void PPCTestRunner::lfsu() {
}

void PPCTestRunner::lfsux() {
}

void PPCTestRunner::lfsx() {
}

void PPCTestRunner::lha() {
    // Load Halfword Algebraic
    TEST_INSTRUCTION(test_lha, RAIndex, RA, D, RD, {
        state.r[RAIndex] = RA;
        state.r[2] = 0xFFFFFFFFFFFFFFFFULL;
        run(lha(r2, RAIndex, D));
        expect(state.r[2] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lha(r1, addr + 0x00, 0x0000, 0x0000000000000123ULL);
    test_lha(r1, addr + 0x00, 0x0010, 0xFFFFFFFFFFFFFFFFULL);
    test_lha(r1, addr + 0x10, 0xFFFE, 0x0000000000000000ULL);
    test_lha(r1, addr + 0x08, 0xFFFF, 0xFFFFFFFFFFFFEF00ULL);
    memory->free(addr);
}

void PPCTestRunner::lhau() {
    // Load Halfword Algebraic with Update
    TEST_INSTRUCTION(test_lhau, RAIndex, oldRA, D, RD, newRA, {
        state.r[RAIndex] = oldRA;
        state.r[2] = 0xFFFFFFFFFFFFFFFFULL;
        run(lhau(r2, RAIndex, D));
        expect(state.r[RAIndex] == newRA);
        expect(state.r[2] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lhau(r1, addr + 0x00, 0x0000, 0x0000000000000123ULL, addr + 0x00);
    test_lhau(r1, addr + 0x00, 0x0010, 0xFFFFFFFFFFFFFFFFULL, addr + 0x10);
    test_lhau(r1, addr + 0x10, 0xFFFE, 0x0000000000000000ULL, addr + 0x0E);
    test_lhau(r1, addr + 0x08, 0xFFFF, 0xFFFFFFFFFFFFEF00ULL, addr + 0x07);
    memory->free(addr);
}

void PPCTestRunner::lhaux() {
    // Load Halfword Algebraic with Update Indexed
    TEST_INSTRUCTION(test_lhaux, RAIndex, oldRA, RB, RD, newRA, {
        state.r[RAIndex] = oldRA;
        state.r[2] = RB;
        state.r[3] = 0xFFFFFFFFFFFFFFFFULL;
        run(lhaux(r3, RAIndex, r2));
        expect(state.r[RAIndex] == newRA);
        expect(state.r[3] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lhaux(r1, addr + 0x00, 0x0000000000000000ULL, 0x0000000000000123ULL, addr + 0x00);
    test_lhaux(r1, addr + 0x00, 0x0000000000000010ULL, 0xFFFFFFFFFFFFFFFFULL, addr + 0x10);
    test_lhaux(r1, addr + 0x10, 0xFFFFFFFFFFFFFFFEULL, 0x0000000000000000ULL, addr + 0x0E);
    test_lhaux(r1, addr + 0x08, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFEF00ULL, addr + 0x07);
    memory->free(addr);
}

void PPCTestRunner::lhax() {
    // Load Halfword Algebraic Indexed
    TEST_INSTRUCTION(test_lhax, RAIndex, RA, RB, RD, {
        state.r[RAIndex] = RA;
        state.r[2] = RB;
        state.r[3] = 0xFFFFFFFFFFFFFFFFULL;
        run(lhax(r3, RAIndex, r2));
        expect(state.r[3] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lhax(r1, addr + 0x00, 0x0000000000000000ULL, 0x0000000000000123ULL);
    test_lhax(r1, addr + 0x00, 0x0000000000000010ULL, 0xFFFFFFFFFFFFFFFFULL);
    test_lhax(r1, addr + 0x10, 0xFFFFFFFFFFFFFFFEULL, 0x0000000000000000ULL);
    test_lhax(r1, addr + 0x08, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFEF00ULL);
    test_lhax(r0, 0x8000000000000000ULL, addr + 0x00, 0x0000000000000123ULL);
    test_lhax(r0, 0x8000000000000000ULL, addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    test_lhax(r0, 0x8000000000000000ULL, addr + 0x0E, 0x0000000000000000ULL);
    test_lhax(r0, 0x8000000000000000ULL, addr + 0x07, 0xFFFFFFFFFFFFEF00ULL);
    memory->free(addr);
}

void PPCTestRunner::lhbrx() {
}

void PPCTestRunner::lhz() {
    // Load Halfword and Zero
    TEST_INSTRUCTION(test_lhz, RAIndex, RA, D, RD, {
        state.r[RAIndex] = RA;
        state.r[2] = 0xFFFFFFFFFFFFFFFFULL;
        run(lhz(r2, RAIndex, D));
        expect(state.r[2] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lhz(r1, addr + 0x00, 0x0000, 0x0000000000000123ULL);
    test_lhz(r1, addr + 0x00, 0x0010, 0x000000000000FFFFULL);
    test_lhz(r1, addr + 0x10, 0xFFFE, 0x0000000000000000ULL);
    test_lhz(r1, addr + 0x08, 0xFFFF, 0x000000000000EF00ULL);
    memory->free(addr);
}

void PPCTestRunner::lhzu() {
    // Load Halfword and Zero with Update
    TEST_INSTRUCTION(test_lhzu, RAIndex, oldRA, D, RD, newRA, {
        state.r[RAIndex] = oldRA;
        state.r[2] = 0xFFFFFFFFFFFFFFFFULL;
        run(lhzu(r2, RAIndex, D));
        expect(state.r[RAIndex] == newRA);
        expect(state.r[2] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lhzu(r1, addr + 0x00, 0x0000, 0x0000000000000123ULL, addr + 0x00);
    test_lhzu(r1, addr + 0x00, 0x0010, 0x000000000000FFFFULL, addr + 0x10);
    test_lhzu(r1, addr + 0x10, 0xFFFE, 0x0000000000000000ULL, addr + 0x0E);
    test_lhzu(r1, addr + 0x08, 0xFFFF, 0x000000000000EF00ULL, addr + 0x07);
    memory->free(addr);
}

void PPCTestRunner::lhzux() {
    // Load Halfword and Zero with Update Indexed
    TEST_INSTRUCTION(test_lhzux, RAIndex, oldRA, RB, RD, newRA, {
        state.r[RAIndex] = oldRA;
        state.r[2] = RB;
        state.r[3] = 0xFFFFFFFFFFFFFFFFULL;
        run(lhzux(r3, RAIndex, r2));
        expect(state.r[RAIndex] == newRA);
        expect(state.r[3] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lhzux(r1, addr + 0x00, 0x0000000000000000ULL, 0x0000000000000123ULL, addr + 0x00);
    test_lhzux(r1, addr + 0x00, 0x0000000000000010ULL, 0x000000000000FFFFULL, addr + 0x10);
    test_lhzux(r1, addr + 0x10, 0xFFFFFFFFFFFFFFFEULL, 0x0000000000000000ULL, addr + 0x0E);
    test_lhzux(r1, addr + 0x08, 0xFFFFFFFFFFFFFFFFULL, 0x000000000000EF00ULL, addr + 0x07);
    memory->free(addr);
}

void PPCTestRunner::lhzx() {
    // Load Halfword and Zero Indexed
    TEST_INSTRUCTION(test_lhzx, RAIndex, RA, RB, RD, {
        state.r[RAIndex] = RA;
        state.r[2] = RB;
        state.r[3] = 0xFFFFFFFFFFFFFFFFULL;
        run(lhzx(r3, RAIndex, r2));
        expect(state.r[3] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lhzx(r1, addr + 0x00, 0x0000000000000000ULL, 0x0000000000000123ULL);
    test_lhzx(r1, addr + 0x00, 0x0000000000000010ULL, 0x000000000000FFFFULL);
    test_lhzx(r1, addr + 0x10, 0xFFFFFFFFFFFFFFFEULL, 0x0000000000000000ULL);
    test_lhzx(r1, addr + 0x08, 0xFFFFFFFFFFFFFFFFULL, 0x000000000000EF00ULL);
    test_lhzx(r0, 0x8000000000000000ULL, addr + 0x00, 0x0000000000000123ULL);
    test_lhzx(r0, 0x8000000000000000ULL, addr + 0x10, 0x000000000000FFFFULL);
    test_lhzx(r0, 0x8000000000000000ULL, addr + 0x0E, 0x0000000000000000ULL);
    test_lhzx(r0, 0x8000000000000000ULL, addr + 0x07, 0x000000000000EF00ULL);
    memory->free(addr);
}

void PPCTestRunner::lmw() {
}

void PPCTestRunner::lswi() {
}

void PPCTestRunner::lswx() {
}

void PPCTestRunner::lwa() {
    // Load Word Algebraic
    TEST_INSTRUCTION(test_lwa, RAIndex, RA, D, RD, {
        state.r[RAIndex] = RA;
        state.r[2] = 0xFFFFFFFFFFFFFFFFULL;
        run(lwa(r2, RAIndex, D));
        expect(state.r[2] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lwa(r1, addr + 0x00, 0x0000, 0x0000000001234567ULL);
    test_lwa(r1, addr + 0x00, 0x0010, 0xFFFFFFFFFFFFFFFFULL);
    test_lwa(r1, addr + 0x10, 0xFFFC, 0x0000000000000000ULL);
    test_lwa(r1, addr + 0x07, 0x0000, 0xFFFFFFFFEF000000ULL);
    memory->free(addr);
}

void PPCTestRunner::lwaux() {
    // Load Word Algebraic with Update Indexed
    TEST_INSTRUCTION(test_lwaux, RAIndex, oldRA, RB, RD, newRA, {
        state.r[RAIndex] = oldRA;
        state.r[2] = RB;
        state.r[3] = 0xFFFFFFFFFFFFFFFFULL;
        run(lwaux(r3, RAIndex, r2));
        expect(state.r[RAIndex] == newRA);
        expect(state.r[3] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lwaux(r1, addr + 0x00, 0x0000000000000000ULL, 0x0000000001234567ULL, addr + 0x00);
    test_lwaux(r1, addr + 0x00, 0x0000000000000010ULL, 0xFFFFFFFFFFFFFFFFULL, addr + 0x10);
    test_lwaux(r1, addr + 0x10, 0xFFFFFFFFFFFFFFFCULL, 0x0000000000000000ULL, addr + 0x0C);
    test_lwaux(r1, addr + 0x07, 0x0000000000000000ULL, 0xFFFFFFFFEF000000ULL, addr + 0x07);
    memory->free(addr);
}

void PPCTestRunner::lwax() {
    // Load Word Algebraic Indexed
    TEST_INSTRUCTION(test_lwax, RAIndex, RA, RB, RD, {
        state.r[RAIndex] = RA;
        state.r[2] = RB;
        state.r[3] = 0xFFFFFFFFFFFFFFFFULL;
        run(lwax(r3, RAIndex, r2));
        expect(state.r[3] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lwax(r1, addr + 0x00, 0x0000000000000000ULL, 0x0000000001234567ULL);
    test_lwax(r1, addr + 0x00, 0x0000000000000010ULL, 0xFFFFFFFFFFFFFFFFULL);
    test_lwax(r1, addr + 0x10, 0xFFFFFFFFFFFFFFFCULL, 0x0000000000000000ULL);
    test_lwax(r1, addr + 0x07, 0x0000000000000000ULL, 0xFFFFFFFFEF000000ULL);
    test_lwax(r0, 0x8000000000000000ULL, addr + 0x00, 0x0000000001234567ULL);
    test_lwax(r0, 0x8000000000000000ULL, addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    test_lwax(r0, 0x8000000000000000ULL, addr + 0x0C, 0x0000000000000000ULL);
    test_lwax(r0, 0x8000000000000000ULL, addr + 0x07, 0xFFFFFFFFEF000000ULL);
    memory->free(addr);
}

void PPCTestRunner::lwbrx() {
}

void PPCTestRunner::lwz() {
    // Load Word and Zero
    TEST_INSTRUCTION(test_lwz, RAIndex, RA, D, RD, {
        state.r[RAIndex] = RA;
        state.r[2] = 0xFFFFFFFFFFFFFFFFULL;
        run(lwz(r2, RAIndex, D));
        expect(state.r[2] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lwz(r1, addr + 0x00, 0x0000, 0x0000000001234567ULL);
    test_lwz(r1, addr + 0x00, 0x0010, 0x00000000FFFFFFFFULL);
    test_lwz(r1, addr + 0x10, 0xFFFC, 0x0000000000000000ULL);
    test_lwz(r1, addr + 0x07, 0x0000, 0x00000000EF000000ULL);
    memory->free(addr);
}

void PPCTestRunner::lwzu() {
    // Load Word and Zero with Update
    TEST_INSTRUCTION(test_lwzu, RAIndex, oldRA, D, RD, newRA, {
        state.r[RAIndex] = oldRA;
        state.r[2] = 0xFFFFFFFFFFFFFFFFULL;
        run(lwzu(r2, RAIndex, D));
        expect(state.r[RAIndex] == newRA);
        expect(state.r[2] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lwzu(r1, addr + 0x00, 0x0000, 0x0000000001234567ULL, addr + 0x00);
    test_lwzu(r1, addr + 0x00, 0x0010, 0x00000000FFFFFFFFULL, addr + 0x10);
    test_lwzu(r1, addr + 0x10, 0xFFFC, 0x0000000000000000ULL, addr + 0x0C);
    test_lwzu(r1, addr + 0x07, 0x0000, 0x00000000EF000000ULL, addr + 0x07);
    memory->free(addr);
}

void PPCTestRunner::lwzux() {
    // Load Word and Zero with Update Indexed
    TEST_INSTRUCTION(test_lwzux, RAIndex, oldRA, RB, RD, newRA, {
        state.r[RAIndex] = oldRA;
        state.r[2] = RB;
        state.r[3] = 0xFFFFFFFFFFFFFFFFULL;
        run(lwzux(r3, RAIndex, r2));
        expect(state.r[RAIndex] == newRA);
        expect(state.r[3] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lwzux(r1, addr + 0x00, 0x0000000000000000ULL, 0x0000000001234567ULL, addr + 0x00);
    test_lwzux(r1, addr + 0x00, 0x0000000000000010ULL, 0x00000000FFFFFFFFULL, addr + 0x10);
    test_lwzux(r1, addr + 0x10, 0xFFFFFFFFFFFFFFFCULL, 0x0000000000000000ULL, addr + 0x0C);
    test_lwzux(r1, addr + 0x07, 0x0000000000000000ULL, 0x00000000EF000000ULL, addr + 0x07);
    memory->free(addr);
}

void PPCTestRunner::lwzx() {
    // Load Word and Zero Indexed
    TEST_INSTRUCTION(test_lwzx, RAIndex, RA, RB, RD, {
        state.r[RAIndex] = RA;
        state.r[2] = RB;
        state.r[3] = 0xFFFFFFFFFFFFFFFFULL;
        run(lwzx(r3, RAIndex, r2));
        expect(state.r[3] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lwzx(r1, addr + 0x00, 0x0000000000000000ULL, 0x0000000001234567ULL);
    test_lwzx(r1, addr + 0x00, 0x0000000000000010ULL, 0x00000000FFFFFFFFULL);
    test_lwzx(r1, addr + 0x10, 0xFFFFFFFFFFFFFFFCULL, 0x0000000000000000ULL);
    test_lwzx(r1, addr + 0x07, 0x0000000000000000ULL, 0x00000000EF000000ULL);
    test_lwzx(r0, 0x8000000000000000ULL, addr + 0x00, 0x0000000001234567ULL);
    test_lwzx(r0, 0x8000000000000000ULL, addr + 0x10, 0x00000000FFFFFFFFULL);
    test_lwzx(r0, 0x8000000000000000ULL, addr + 0x0C, 0x0000000000000000ULL);
    test_lwzx(r0, 0x8000000000000000ULL, addr + 0x07, 0x00000000EF000000ULL);
    memory->free(addr);
}

void PPCTestRunner::stb() {
        // Load Byte and Zero
    TEST_INSTRUCTION(test_lbz, RAIndex, RA, D, RD, {
        state.r[RAIndex] = RA;
        state.r[2] = 0xFFFFFFFFFFFFFFFFULL;
        run(lbz(r2, RAIndex, D));
        expect(state.r[2] == RD);
        expect(!state.cr.field[0].lt);
        expect(!state.cr.field[0].gt);
        expect(!state.cr.field[0].eq);
        expect(!state.cr.field[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    const U32 addr = memory->alloc(0x1000);
    memory->write64(addr + 0x00, 0x0123456789ABCDEFULL);
    memory->write64(addr + 0x08, 0x0000000000000000ULL);
    memory->write64(addr + 0x10, 0xFFFFFFFFFFFFFFFFULL);
    memory->write64(addr + 0x18, 0x00000000FFFFFFFFULL);
    memory->write64(addr + 0x20, 0x0000FFFF00FF0000ULL);

    test_lbz(r1, addr + 0x00, 0x0000, 0x0000000000000001ULL);
    test_lbz(r1, addr + 0x00, 0x0010, 0x00000000000000FFULL);
    test_lbz(r1, addr + 0x10, 0xFFFF, 0x0000000000000000ULL);
    test_lbz(r1, addr + 0x08, 0xFFFF, 0x00000000000000EFULL);
    memory->free(addr);
}

void PPCTestRunner::stbu() {
}

void PPCTestRunner::stbux() {
}

void PPCTestRunner::stbx() {
}

void PPCTestRunner::std() {
}

void PPCTestRunner::stdu() {
}

void PPCTestRunner::stdux() {
}

void PPCTestRunner::stdx() {
}

void PPCTestRunner::stfd() {
}

void PPCTestRunner::stfdu() {
}

void PPCTestRunner::stfdux() {
}

void PPCTestRunner::stfdx() {
}

void PPCTestRunner::stfiwx() {
}

void PPCTestRunner::stfs() {
}

void PPCTestRunner::stfsu() {
}

void PPCTestRunner::stfsux() {
}

void PPCTestRunner::stfsx() {
}

void PPCTestRunner::sth() {
}

void PPCTestRunner::sthbrx() {
}

void PPCTestRunner::sthu() {
}

void PPCTestRunner::sthux() {
}

void PPCTestRunner::sthx() {
}

void PPCTestRunner::stmw() {
}

void PPCTestRunner::stswi() {
}

void PPCTestRunner::stswx() {
}

void PPCTestRunner::stw() {
}

void PPCTestRunner::stwbrx() {
}

void PPCTestRunner::stwu() {
}

void PPCTestRunner::stwux() {
}

void PPCTestRunner::stwx() {
}

void PPCTestRunner::ldarx() {
}

void PPCTestRunner::lwarx() {
}

void PPCTestRunner::stdcx_() {
}

void PPCTestRunner::stwcx_() {
}

void PPCTestRunner::sync() {
}

void PPCTestRunner::eieio() {
}

void PPCTestRunner::isync() {
}
