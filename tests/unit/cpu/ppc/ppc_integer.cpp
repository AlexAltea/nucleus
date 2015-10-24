/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "tests/unit/cpu/test_ppu.h"

/**
 * PPC64 Instructions:
 *  - UISA: Integer instructions (Section: 4.2.1)
 */

void PPCTestRunner::addx() {
    // Add
    TEST_INSTRUCTION(test_add, R1, R2, R3, {
        state.r[1] = R1;
        state.r[2] = R2;
        run(add(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_add(0x000000010000FFFFULL, 0x0000000200000001ULL, 0x0000000300010000ULL);
    test_add(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, 0x0000000000000000ULL);
    
    // Add (with condition)
    TEST_INSTRUCTION(test_add_, R1, R2, R3, LT, GT, EQ, SO, {
        state.r[1] = R1;
        state.r[2] = R2;
        run(add_(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_add_(0x000000010000FFFFULL, 0x0000000200000001ULL, 0x0000000300010000ULL, 0,1,0,0);
    test_add_(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, 0x0000000000000000ULL, 0,0,1,0);
    test_add_(0x0000000000000001ULL, 0x7FFFFFFFFFFFFFFFULL, 0x8000000000000000ULL, 1,0,0,0);
}

void PPCTestRunner::addcx() {
    // Add Carrying
    TEST_INSTRUCTION(test_addc, R1, R2, oldCA, R3, newCA, {
        state.r[1] = R1;
        state.r[2] = R2;
        state.xer.ca = oldCA;
        run(addc(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(state.xer.ca == newCA);
    });

    test_addc(0x000000010000FFFFULL, 0x0000000200000001ULL, false, 0x0000000300010000ULL, false);
    test_addc(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, false, 0x0000000000000000ULL, true);
    test_addc(0xFFFFFFFFFFFFFFF0ULL, 0x000000000000000FULL, true,  0xFFFFFFFFFFFFFFFFULL, false);
    test_addc(0x000000000000FFFFULL, 0xFFFFFFFFFFFF0011ULL, true,  0x0000000000000010ULL, true);

    // Add Carrying (with condition)
    TEST_INSTRUCTION(test_addc_, R1, R2, oldCA, R3, newCA, LT, GT, EQ, SO, {
        state.r[1] = R1;
        state.r[2] = R2;
        state.xer.ca = oldCA;
        run(addc_(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(state.xer.ca == newCA);
    });

    test_addc_(0x000000010000FFFFULL, 0x0000000200000001ULL, false, 0x0000000300010000ULL, false, 0,1,0,0);
    test_addc_(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, false, 0x0000000000000000ULL, true,  0,0,1,0);
    test_addc_(0xFFFFFFFFFFFFFFF0ULL, 0x000000000000000FULL, true,  0xFFFFFFFFFFFFFFFFULL, false, 1,0,0,0);
    test_addc_(0x000000000000FFFFULL, 0xFFFFFFFFFFFF0011ULL, true,  0x0000000000000010ULL, true,  0,1,0,0);
}

void PPCTestRunner::addex() {
    // Add Extended
    TEST_INSTRUCTION(test_adde, R1, R2, oldCA, R3, newCA, {
        state.r[1] = R1;
        state.r[2] = R2;
        state.xer.ca = oldCA;
        run(adde(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(state.xer.ca == newCA);
    });

    test_adde(0x000000010000FFFFULL, 0x0000000200000001ULL, false, 0x0000000300010000ULL, false);
    test_adde(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, false, 0x0000000000000000ULL, true);
    test_adde(0x000000000000FFFFULL, 0x0000000000000000ULL, true,  0x0000000000010000ULL, false);
    test_adde(0xFFFFFFFFFFFFFFF0ULL, 0x000000000000000FULL, true,  0x0000000000000000ULL, true);
        
    // Add Extended (with condition)
    TEST_INSTRUCTION(test_adde_, R1, R2, oldCA, R3, newCA, LT, GT, EQ, SO, {
        state.r[1] = R1;
        state.r[2] = R2;
        state.xer.ca = oldCA;
        run(adde_(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(state.xer.ca == newCA);
    });

    test_adde_(0x000000010000FFFFULL, 0x0000000200000001ULL, false, 0x0000000300010000ULL, false, 0,1,0,0);
    test_adde_(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, false, 0x0000000000000000ULL, true,  0,0,1,0);
    test_adde_(0x000000000000FFFFULL, 0x0000000000000000ULL, true,  0x0000000000010000ULL, false, 0,1,0,0);
    test_adde_(0xFFFFFFFFFFFFFFFFULL, 0x8000000000000000ULL, true,  0x8000000000000000ULL, true,  1,0,0,0);
}

void PPCTestRunner::addi() {
    // Add Immediate
    TEST_INSTRUCTION(test_addi, RAIndex, RA, SIMM, RD, {
        state.r[RAIndex] = RA;
        run(addi(r2, RAIndex, SIMM));
        expect(state.r[2] == RD);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_addi(r1, 0x000000010000FFFFULL, 0x0001, 0x0000000100010000ULL);
    test_addi(r1, 0x00000000FFFFFFFFULL, 0x0001, 0x0000000100000000ULL);
    test_addi(r1, 0x00000000FFFF0001ULL, 0xFFFF, 0x00000000FFFF0000ULL);
    test_addi(r1, 0xFFFFFFFFFFFFFFFFULL, 0x0001, 0x0000000000000000ULL);
    test_addi(r0, 0x000000010000FFFFULL, 0x0001, 0x0000000000000001ULL);
    test_addi(r0, 0x00000000FFFFFFFFULL, 0xFFFF, 0xFFFFFFFFFFFFFFFFULL);
    test_addi(r0, 0x00000000FFFF0001ULL, 0x0000, 0x0000000000000000ULL);
}

void PPCTestRunner::addic() {
    // Add Immediate Carrying
    TEST_INSTRUCTION(test_addic, R1, SIMM, R2, CA, {
        state.r[1] = R1;
        run(addic(r2, r1, SIMM));
        expect(state.r[2] == R2);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(state.xer.ca == CA);
    });

    test_addic(0x000000010000FFFFULL, 0x0001, 0x0000000100010000ULL, false);
    test_addic(0x00000000FFFFFFFFULL, 0x0001, 0x0000000100000000ULL, false);
    test_addic(0x00000000FFFF0001ULL, 0xFFFF, 0x00000000FFFF0000ULL, false);
    test_addic(0xFFFFFFFFFFFFFFFFULL, 0x0001, 0x0000000000000000ULL, true);
}

void PPCTestRunner::addic_() {
    // Add Immediate Carrying and Record
    TEST_INSTRUCTION(test_addic_, R1, SIMM, R2, CA, LT, GT, EQ, SO, {
        state.r[1] = R1;
        run(addic_(r2, r1, SIMM));
        expect(state.r[2] == R2);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(state.xer.ca == CA);
    });

    test_addic_(0x000000010000FFFFULL, 0x0001, 0x0000000100010000ULL, false, 0,1,0,0);
    test_addic_(0x00000000FFFFFFFFULL, 0x0001, 0x0000000100000000ULL, false, 0,1,0,0);
    test_addic_(0x00000000FFFF0001ULL, 0xFFFF, 0x00000000FFFF0000ULL, false, 0,1,0,0);
    test_addic_(0xFFFFFFFFFFFFFFFFULL, 0xFFFF, 0xFFFFFFFFFFFFFFFEULL, false, 1,0,0,0);
    test_addic_(0xFFFFFFFFFFFFFFFFULL, 0x0001, 0x0000000000000000ULL, true,  0,0,1,0);
}

void PPCTestRunner::addis() {
    // Add Immediate Shifted
    TEST_INSTRUCTION(test_addis, RAIndex, RA, SIMM, RD, {
        state.r[RAIndex] = RA;
        run(addis(r2, RAIndex, SIMM));
        expect(state.r[2] == RD);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_addis(r1, 0x00010000FFFF0000ULL, 0x0001, 0x0001000100000000ULL);
    test_addis(r1, 0x0000FFFFFFFF0000ULL, 0x0001, 0x0001000000000000ULL);
    test_addis(r1, 0x0000FFFF00010000ULL, 0xFFFF, 0x0000FFFF00000000ULL);
    test_addis(r1, 0xFFFFFFFFFFFF0000ULL, 0x0001, 0x0000000000000000ULL);
    test_addis(r0, 0x00010000FFFF0000ULL, 0x0001, 0x0000000000010000ULL);
    test_addis(r0, 0x0000FFFFFFFF0000ULL, 0xFFFF, 0xFFFFFFFFFFFF0000ULL);
    test_addis(r0, 0x0000FFFF00010000ULL, 0x0000, 0x0000000000000000ULL);
}

void PPCTestRunner::addmex() {
}

void PPCTestRunner::addzex() {
    // Add to Zero Extended
    TEST_INSTRUCTION(test_addze, RA, oldCA, RD, newCA, {
        state.r[1] = RA;
        state.xer.ca = oldCA;
        run(addze(r2, r1));
        expect(state.r[2] == RD);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(state.xer.ca == newCA);
    });

    test_addze(0x1111111111111111, false, 0x1111111111111111, false);
    test_addze(0x1111111111111111, true,  0x1111111111111112, false);
    test_addze(0x7fffffffffffffff, false, 0x7fffffffffffffff, false);
    test_addze(0x7fffffffffffffff, true,  0x8000000000000000, false);
    test_addze(0xffffffffffffffff, false, 0xffffffffffffffff, false);
    test_addze(0xffffffffffffffff, true,  0x0000000000000000, true);

    // Add to Zero Extended (with condition) 
    TEST_INSTRUCTION(test_addze_, RA, oldCA, RD, newCA, LT, GT, EQ, SO, {
        state.r[1] = RA;
        state.xer.ca = oldCA;
        run(addze_(r2, r1));
        expect(state.r[2] == RD);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(state.xer.ca == newCA);
    });

    test_addze_(0x1111111111111111, false, 0x1111111111111111, false, 0,1,0,0);
    test_addze_(0x1111111111111111, true,  0x1111111111111112, false, 0,1,0,0);
    test_addze_(0x7fffffffffffffff, false, 0x7fffffffffffffff, false, 0,1,0,0);
    test_addze_(0x7fffffffffffffff, true,  0x8000000000000000, false, 1,0,0,0);
    test_addze_(0xffffffffffffffff, false, 0xffffffffffffffff, false, 1,0,0,0);
    test_addze_(0xffffffffffffffff, true,  0x0000000000000000, true,  0,0,1,0);
}

void PPCTestRunner::andx() {
    // AND
    TEST_INSTRUCTION(test_and_, R1, R2, R3, {
        state.r[1] = R1;
        state.r[2] = R2;
        run(and_(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_and_(0x0000111100001111ULL, 0x1111111100000000ULL, 0x0000111100000000ULL);
    test_and_(0x00000000000000FFULL, 0x000000000000F0F0ULL, 0x00000000000000F0ULL);

    // AND (with condition)
    TEST_INSTRUCTION(test_and__, R1, R2, R3, LT, GT, EQ, SO, {
        state.r[1] = R1;
        state.r[2] = R2;
        run(and__(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_and__(0x00000000FFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0x00000000FFFFFFFFULL, 0,1,0,0);
    test_and__(0xFFFFFFFF00000000ULL, 0x800000000000FFFFULL, 0x8000000000000000ULL, 1,0,0,0);
    test_and__(0xFFFFFFFF00000000ULL, 0x00000000FFFFFFFFULL, 0x0000000000000000ULL, 0,0,1,0);
}

void PPCTestRunner::andcx() {
    // And with Complement
    TEST_INSTRUCTION(test_andc, R1, R2, R3, {
        state.r[1] = R1;
        state.r[2] = R2;
        run(andc(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_andc(0x0000111100001111ULL, 0x1111111100000000ULL, 0x0000000000001111ULL);
    test_andc(0x00000000000000FFULL, 0x000000000000F0F0ULL, 0x000000000000000FULL);

    // And with Complement (with condition)
    TEST_INSTRUCTION(test_andc_, R1, R2, R3, LT, GT, EQ, SO, {
        state.r[1] = R1;
        state.r[2] = R2;
        run(andc_(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_andc_(0x00000000FFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL, 0,0,1,0);
    test_andc_(0xFFFFFFFF00005555ULL, 0xFFFFFFFFFFFF0000ULL, 0x0000000000005555ULL, 0,1,0,0);
    test_andc_(0xFFFFFFFF00000000ULL, 0x00000000FFFFFFFFULL, 0xFFFFFFFF00000000ULL, 1,0,0,0);
}

void PPCTestRunner::andi_() {
    // AND Immediate
    TEST_INSTRUCTION(test_andi_, R1, UIMM, R2, LT, GT, EQ, SO, {
        state.r[1] = R1;
        run(andi_(r2, r1, UIMM));
        expect(state.r[2] == R2);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_andi_(0x0000FFFFFFFFFFF0ULL, 0x000F, 0x0000000000000000ULL, 0,0,1,0);
    test_andi_(0x0000FFFFFFFFFFFFULL, 0xFFFF, 0x000000000000FFFFULL, 0,1,0,0);
}

void PPCTestRunner::andis_() {
    // AND Immediate Shifted
    TEST_INSTRUCTION(test_andis_, R1, UIMM, R2, LT, GT, EQ, SO, {
        state.r[1] = R1;
        run(andis_(r2, r1, UIMM));
        expect(state.r[2] == R2);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_andis_(0x0000FFFFFFF0FFFFULL, 0x000F, 0x0000000000000000ULL, 0,0,1,0);
    test_andis_(0x0000FFFFFFFFFFFFULL, 0xFFFF, 0x00000000FFFF0000ULL, 0,1,0,0);
}

void PPCTestRunner::cmp() {
}

void PPCTestRunner::cmpi() {
}

void PPCTestRunner::cmpl() {
}

void PPCTestRunner::cmpli() {
}

void PPCTestRunner::cntlzdx() {
}

void PPCTestRunner::cntlzwx() {
}

void PPCTestRunner::divdx() {
}

void PPCTestRunner::divdux() {
}

void PPCTestRunner::divwx() {
}

void PPCTestRunner::divwux() {
}

void PPCTestRunner::eqvx() {
}

void PPCTestRunner::extsbx() {
    // Extend Sign Byte
    TEST_INSTRUCTION(test_extsb, RS, RA, {
        state.r[1] = RS;
        run(extsb(r2, r1));
        expect(state.r[2] == RA);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_extsb(0x00FF00FF00FF0020ULL, 0x0000000000000020ULL);
    test_extsb(0xFF00FF00000000FFULL, 0xFFFFFFFFFFFFFFFFULL);

    // Extend Sign Byte (with condition)
    TEST_INSTRUCTION(test_extsb_, RS, RA, LT, GT, EQ, SO, {
        state.r[1] = RS;
        run(extsb_(r2, r1));
        expect(state.r[2] == RA);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_extsb_(0x00FF00FF00FF0000ULL, 0x0000000000000000ULL, 0,0,1,0);
    test_extsb_(0x0000000000000080ULL, 0xFFFFFFFFFFFFFF80ULL, 1,0,0,0);
    test_extsb_(0xFFFFFFFFFFFFFF7FULL, 0x000000000000007FULL, 0,1,0,0);
}

void PPCTestRunner::extshx() {
    // Extend Sign Halfword
    TEST_INSTRUCTION(test_extsh, RS, RA, {
        state.r[1] = RS;
        run(extsh(r2, r1));
        expect(state.r[2] == RA);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_extsh(0x00FF00FF00FF2000ULL, 0x0000000000002000ULL);
    test_extsh(0xFF00FF000000FFFFULL, 0xFFFFFFFFFFFFFFFFULL);

    // Extend Sign Halfword (with condition)
    TEST_INSTRUCTION(test_extsh_, RS, RA, LT, GT, EQ, SO, {
        state.r[1] = RS;
        run(extsh_(r2, r1));
        expect(state.r[2] == RA);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_extsh_(0x00FF00FF00FF0000ULL, 0x0000000000000000ULL, 0,0,1,0);
    test_extsh_(0x0000000000008000ULL, 0xFFFFFFFFFFFF8000ULL, 1,0,0,0);
    test_extsh_(0xFFFFFFFFFFFF7FFFULL, 0x0000000000007FFFULL, 0,1,0,0);
}

void PPCTestRunner::extswx() {
    // Extend Sign Word
    TEST_INSTRUCTION(test_extsw, RS, RA, {
        state.r[1] = RS;
        run(extsw(r2, r1));
        expect(state.r[2] == RA);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_extsw(0x00FF00FF20000000ULL, 0x0000000020000000ULL);
    test_extsw(0xFF00FF00FFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL);

    // Extend Sign Word (with condition)
    TEST_INSTRUCTION(test_extsw_, RS, RA, LT, GT, EQ, SO, {
        state.r[1] = RS;
        run(extsw_(r2, r1));
        expect(state.r[2] == RA);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_extsw_(0x00FF00FF00000000ULL, 0x0000000000000000ULL, 0,0,1,0);
    test_extsw_(0x0000000080000000ULL, 0xFFFFFFFF80000000ULL, 1,0,0,0);
    test_extsw_(0xFFFFFFFF7FFFFFFFULL, 0x000000007FFFFFFFULL, 0,1,0,0);
}

void PPCTestRunner::mulhdx() {
}

void PPCTestRunner::mulhdux() {
}

void PPCTestRunner::mulhwx() {
}

void PPCTestRunner::mulhwux() {
}

void PPCTestRunner::mulldx() {
}

void PPCTestRunner::mulli() {
}

void PPCTestRunner::mullwx() {
}

void PPCTestRunner::nandx() {
    // NAND
    TEST_INSTRUCTION(test_nand, RS, RB, RA, {
        state.r[1] = RS;
        state.r[2] = RB;
        run(nand(r3, r1, r2));
        expect(state.r[3] == RA);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_nand(0x0000FFFF0000FFFFULL, 0xFFFFFFFF00000000ULL, 0xFFFF0000FFFFFFFFULL);
    test_nand(0xF000000000000000ULL, 0xF000000000000000ULL, 0x0FFFFFFFFFFFFFFFULL);
    test_nand(0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL);

    // NAND (with condition)
    TEST_INSTRUCTION(test_nand_, RS, RB, RA, LT, GT, EQ, SO, {
        state.r[1] = RS;
        state.r[2] = RB;
        run(nand_(r3, r1, r2));
        expect(state.r[3] == RA);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_nand_(0x0000FFFF0000FFFFULL, 0xFFFFFFFF00000000ULL, 0xFFFF0000FFFFFFFFULL, 1,0,0,0);
    test_nand_(0xF000000000000000ULL, 0xF000000000000000ULL, 0x0FFFFFFFFFFFFFFFULL, 0,1,0,0);
    test_nand_(0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL, 0,0,1,0);
}

void PPCTestRunner::negx() {
    // Negate
    TEST_INSTRUCTION(test_neg, RA, RD, {
        state.r[1] = RA;
        run(neg(r2, r1));
        expect(state.r[2] == RD);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_neg(0xFFFF0000FFFF0000ULL, 0x0000FFFF00010000ULL);
    test_neg(0x0000000000000001ULL, 0xFFFFFFFFFFFFFFFFULL);
    test_neg(0x0000000000000000ULL, 0x0000000000000000ULL);

    // Negate (with condition)
    TEST_INSTRUCTION(test_neg_, RA, RD, LT, GT, EQ, SO, {
        state.r[1] = RA;
        run(neg_(r2, r1));
        expect(state.r[2] == RD);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_neg_(0xFFFF0000FFFF0000ULL, 0x0000FFFF00010000ULL, 0,1,0,0);
    test_neg_(0x0000000000000001ULL, 0xFFFFFFFFFFFFFFFFULL, 1,0,0,0);
    test_neg_(0x0000000000000000ULL, 0x0000000000000000ULL, 0,0,1,0);
}

void PPCTestRunner::norx() {
    // NOR
    TEST_INSTRUCTION(test_nor, RS, RB, RA, {
        state.r[1] = RS;
        state.r[2] = RB;
        run(nor(r3, r1, r2));
        expect(state.r[3] == RA);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_nor(0x0000FFFF0000FFFFULL, 0xFFFFFFFF00000000ULL, 0x00000000FFFF0000ULL);
    test_nor(0x000000000000FF00ULL, 0x000000000000F0F0ULL, 0xFFFFFFFFFFFF000FULL);
    test_nor(0xFFFFFFFF00000000ULL, 0x00000000FFFFFFFFULL, 0x0000000000000000ULL);

    // NOR (with condition)
    TEST_INSTRUCTION(test_nor_, RS, RB, RA, LT, GT, EQ, SO, {
        state.r[1] = RS;
        state.r[2] = RB;
        run(nor_(r3, r1, r2));
        expect(state.r[3] == RA);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_nor_(0x0000FFFF0000FFFFULL, 0xFFFFFFFF00000000ULL, 0x00000000FFFF0000ULL, 0,1,0,0);
    test_nor_(0x000000000000FF00ULL, 0x000000000000F0F0ULL, 0xFFFFFFFFFFFF000FULL, 1,0,0,0);
    test_nor_(0xFFFFFFFF00000000ULL, 0x00000000FFFFFFFFULL, 0x0000000000000000ULL, 0,0,1,0);
}

void PPCTestRunner::orx() {
    // OR
    TEST_INSTRUCTION(test_or_, R1, R2, R3, {
        state.r[1] = R1;
        state.r[2] = R2;
        run(or_(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_or_(0x0000111100001111ULL, 0x1111111100000000ULL, 0x1111111100001111ULL);
    test_or_(0x00000000000000FFULL, 0x000000000000F0F0ULL, 0x000000000000F0FFULL);

    // OR (with condition)
    TEST_INSTRUCTION(test_or__, R1, R2, R3, LT, GT, EQ, SO, {
        state.r[1] = R1;
        state.r[2] = R2;
        run(or__(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_or__(0x0000FF00FF00FFFFULL, 0x000000FF00FFFFFFULL, 0x0000FFFFFFFFFFFFULL, 0,1,0,0);
    test_or__(0x000000000000FFFFULL, 0x80000000000000FFULL, 0x800000000000FFFFULL, 1,0,0,0);
    test_or__(0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0,0,1,0);
}

void PPCTestRunner::orcx() {
    // OR with Complement
    TEST_INSTRUCTION(test_orc, R1, R2, R3, {
        state.r[1] = R1;
        state.r[2] = R2;
        run(orc(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_orc(0x0000FFFF0000FFFFULL, 0x00000000FFFFFFFFULL, 0xFFFFFFFF0000FFFFULL);
    test_orc(0x00000000000000FFULL, 0xFFFFFFFFFFFF0F0FULL, 0x000000000000F0FFULL);

    // OR with Complement (with condition)
    TEST_INSTRUCTION(test_orc_, R1, R2, R3, LT, GT, EQ, SO, {
        state.r[1] = R1;
        state.r[2] = R2;
        run(orc_(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_orc_(0x0000FF00FF00FFFFULL, 0xFFFFFF00FF000000ULL, 0x0000FFFFFFFFFFFFULL, 0,1,0,0);
    test_orc_(0x000000000000FFFFULL, 0x0FFFFFFFFFFFFF00ULL, 0xF00000000000FFFFULL, 1,0,0,0);
    test_orc_(0x0000000000000000ULL, 0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL, 0,0,1,0);
}

void PPCTestRunner::ori() {
    // OR Immediate
    TEST_INSTRUCTION(test_ori, R1, UIMM, R2,{
        state.r[1] = R1;
        run(ori(r2, r1, UIMM));
        expect(state.r[2] == R2);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_ori(0x000000000000FF00ULL, 0x0F0F, 0x000000000000FF0FULL);
    test_ori(0x0000FFFFFFFFFFFFULL, 0xFFFF, 0x0000FFFFFFFFFFFFULL);
}

void PPCTestRunner::oris() {
    // OR Immediate Shifted
    TEST_INSTRUCTION(test_oris, R1, UIMM, R2, {
        state.r[1] = R1;
        run(oris(r2, r1, UIMM));
        expect(state.r[2] == R2);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_oris(0x00000000FF0000FFULL, 0x0F0F, 0x00000000FF0F00FFULL);
    test_oris(0x0000FFFFFFFFFF00ULL, 0xFFFF, 0x0000FFFFFFFFFF00ULL);
}

void PPCTestRunner::rldc_lr() {
}

void PPCTestRunner::rldicx() {
}

void PPCTestRunner::rldiclx() {
}

void PPCTestRunner::rldicrx() {
}

void PPCTestRunner::rldimix() {
}

void PPCTestRunner::rlwimix() {
}

void PPCTestRunner::rlwinmx() {
}

void PPCTestRunner::rlwnmx() {
}

void PPCTestRunner::sldx() {
}

void PPCTestRunner::slwx() {
}

void PPCTestRunner::sradx() {
}

void PPCTestRunner::sradix() {
}

void PPCTestRunner::srawx() {
}

void PPCTestRunner::srawix() {
}

void PPCTestRunner::srdx() {
}

void PPCTestRunner::srwx() {
}

void PPCTestRunner::subfx() {
    // Subtract From
    TEST_INSTRUCTION(test_subf, R1, R2, R3, {
        state.r[1] = R1;
        state.r[2] = R2;
        run(subf(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_subf(0x0000000300010000ULL, 0x0000000200000001ULL, 0x000000010000FFFFULL);
    test_subf(0x0000000000000000ULL, 0x0000000000000001ULL, 0xFFFFFFFFFFFFFFFFULL);
    
    // Subtract From (with condition)
    TEST_INSTRUCTION(test_subf_, R1, R2, R3, LT, GT, EQ, SO, {
        state.r[1] = R1;
        state.r[2] = R2;
        run(subf_(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_subf_(0x0000000300010000ULL, 0x000000010000FFFFULL, 0x0000000200000001ULL, 0,1,0,0);
    test_subf_(0x0000000000000001ULL, 0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL, 0,0,1,0);
    test_subf_(0x8000000000000001ULL, 0xFFFFFFFFFFFFFFFFULL, 0x8000000000000000ULL, 1,0,0,0);
}

void PPCTestRunner::subfcx() {
    // Subtract from Carrying
    TEST_INSTRUCTION(test_subfc, RA, RB, oldCA, RD, newCA, {
        state.r[1] = RA;
        state.r[2] = RB;
        state.xer.ca = oldCA;
        run(subfc(r3, r1, r2));
        expect(state.r[3] == RD);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(state.xer.ca == newCA);
    });

    test_subfc(0x000000010000FFFFULL, 0x0000000300010000ULL, false, 0x0000000200000001ULL, false);
    test_subfc(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL, false, 0x0000000000000001ULL, true);
    test_subfc(0xFFFFFFFFFFFFFFF0ULL, 0xFFFFFFFFFFFFFFFFULL, true,  0x000000000000000FULL, false);
    test_subfc(0x000000000000FFFFULL, 0x0000000000000010ULL, true,  0xFFFFFFFFFFFF0011ULL, true);

    // Subtract from Carrying (with condition)
    TEST_INSTRUCTION(test_subfc_, RA, RB, oldCA, RD, newCA, LT, GT, EQ, SO, {
        state.r[1] = RA;
        state.r[2] = RB;
        state.xer.ca = oldCA;
        run(subfc_(r3, r1, r2));
        expect(state.r[3] == RD);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(state.xer.ca == newCA);
    });

    test_subfc_(0x000000010000FFFFULL, 0x0000000200000001ULL, false, 0x0000000300010000ULL, false, 0,1,0,0);
    test_subfc_(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, false, 0x0000000000000000ULL, true,  0,0,1,0);
    test_subfc_(0xFFFFFFFFFFFFFFF0ULL, 0x000000000000000FULL, true,  0xFFFFFFFFFFFFFFFFULL, false, 1,0,0,0);
    test_subfc_(0x000000000000FFFFULL, 0xFFFFFFFFFFFF0011ULL, true,  0x0000000000000010ULL, true,  0,1,0,0);
}

void PPCTestRunner::subfex() {
    // Subtract from Extended
    TEST_INSTRUCTION(test_subfe, R1, R2, oldCA, R3, newCA, {
        state.r[1] = R1;
        state.r[2] = R2;
        state.xer.ca = oldCA;
        run(adde(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(state.xer.ca == newCA);
    });

    test_subfe(0x000000010000FFFFULL, 0x0000000200000001ULL, false, 0x0000000300010000ULL, false);
    test_subfe(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, false, 0x0000000000000000ULL, true);
    test_subfe(0x000000000000FFFFULL, 0x0000000000000000ULL, true,  0x0000000000010000ULL, false);
    test_subfe(0xFFFFFFFFFFFFFFF0ULL, 0x000000000000000FULL, true,  0x0000000000000000ULL, true);
        
    // Subtract from Extended (with condition)
    TEST_INSTRUCTION(test_subfe_, R1, R2, oldCA, R3, newCA, LT, GT, EQ, SO, {
        state.r[1] = R1;
        state.r[2] = R2;
        state.xer.ca = oldCA;
        run(subfe_(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(state.xer.ca == newCA);
    });

    test_subfe_(0x000000010000FFFFULL, 0x0000000200000001ULL, false, 0x0000000300010000ULL, false, 0,1,0,0);
    test_subfe_(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, false, 0x0000000000000000ULL, true,  0,0,1,0);
    test_subfe_(0x000000000000FFFFULL, 0x0000000000000000ULL, true,  0x0000000000010000ULL, false, 0,1,0,0);
    test_subfe_(0xFFFFFFFFFFFFFFFFULL, 0x8000000000000000ULL, true,  0x8000000000000000ULL, true,  1,0,0,0);
}

void PPCTestRunner::subfic() {
    // Subtract from Immediate Carrying
    TEST_INSTRUCTION(test_subfic, R1, SIMM, R2, CA, {
        state.r[1] = R1;
        run(subfic(r2, r1, SIMM));
        expect(state.r[2] == R2);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(state.xer.ca == CA);
    });

    test_subfic(0x000000010000FFFFULL, 0x0001, 0x0000000100010000ULL, false);
    test_subfic(0x00000000FFFFFFFFULL, 0x0001, 0x0000000100000000ULL, false);
    test_subfic(0x00000000FFFF0001ULL, 0xFFFF, 0x00000000FFFF0000ULL, false);
    test_subfic(0xFFFFFFFFFFFFFFFFULL, 0x0001, 0x0000000000000000ULL, true);
}

void PPCTestRunner::subfmex() {
}

void PPCTestRunner::subfzex() {
    // Subtract from Zero Extended
    TEST_INSTRUCTION(test_subfze, RA, oldCA, RD, newCA, {
        state.r[1] = RA;
        state.xer.ca = oldCA;
        run(addze(r2, r1));
        expect(state.r[2] == RD);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(state.xer.ca == newCA);
    });

    test_subfze(0x1111111111111111, false, 0x1111111111111111, false);
    test_subfze(0x1111111111111111, true,  0x1111111111111112, false);
    test_subfze(0x7fffffffffffffff, false, 0x7fffffffffffffff, false);
    test_subfze(0x7fffffffffffffff, true,  0x8000000000000000, false);
    test_subfze(0xffffffffffffffff, false, 0xffffffffffffffff, false);
    test_subfze(0xffffffffffffffff, true,  0x0000000000000000, true);

    // Subtract from Zero Extended (with condition) 
    TEST_INSTRUCTION(test_subfze_, RA, oldCA, RD, newCA, LT, GT, EQ, SO, {
        state.r[1] = RA;
        state.xer.ca = oldCA;
        run(addze_(r2, r1));
        expect(state.r[2] == RD);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(state.xer.ca == newCA);
    });

    test_subfze_(0x1111111111111111, false, 0x1111111111111111, false, 0,1,0,0);
    test_subfze_(0x1111111111111111, true,  0x1111111111111112, false, 0,1,0,0);
    test_subfze_(0x7fffffffffffffff, false, 0x7fffffffffffffff, false, 0,1,0,0);
    test_subfze_(0x7fffffffffffffff, true,  0x8000000000000000, false, 1,0,0,0);
    test_subfze_(0xffffffffffffffff, false, 0xffffffffffffffff, false, 1,0,0,0);
    test_subfze_(0xffffffffffffffff, true,  0x0000000000000000, true,  0,0,1,0);
}

void PPCTestRunner::xorx() {
    // XOR
    TEST_INSTRUCTION(test_xor_, R1, R2, R3, {
        state.r[1] = R1;
        state.r[2] = R2;
        run(xor_(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_xor_(0x0000111100001111ULL, 0x1111111100000000ULL, 0x1111000000001111ULL);
    test_xor_(0x00000000000000FFULL, 0x000000000000F0F0ULL, 0x000000000000F00FULL);

    // XOR (with condition)
    TEST_INSTRUCTION(test_xor__, R1, R2, R3, LT, GT, EQ, SO, {
        state.r[1] = R1;
        state.r[2] = R2;
        run(xor__(r3, r1, r2));
        expect(state.r[3] == R3);
        expect(state.cr[0].lt == LT);
        expect(state.cr[0].gt == GT);
        expect(state.cr[0].eq == EQ);
        expect(state.cr[0].so == SO);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_xor__(0x0000FF00FF00FFFFULL, 0x000000FF00FFFFFFULL, 0x0000FFFFFFFF0000ULL, 0,1,0,0);
    test_xor__(0x000000000000FFFFULL, 0x80000000000000FFULL, 0x800000000000FF00ULL, 1,0,0,0);
    test_xor__(0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL, 0,0,1,0);
}

void PPCTestRunner::xori() {
    // XOR Immediate
    TEST_INSTRUCTION(test_xori, R1, UIMM, R2,{
        state.r[1] = R1;
        run(xori(r2, r1, UIMM));
        expect(state.r[2] == R2);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_xori(0x000000000000FF00ULL, 0x0F0F, 0x000000000000F00FULL);
    test_xori(0x0000FFFFFFFFFFFFULL, 0xFFFF, 0x0000FFFFFFFF0000ULL);
}

void PPCTestRunner::xoris() {
    // XOR Immediate Shifted
    TEST_INSTRUCTION(test_xoris, R1, UIMM, R2, {
        state.r[1] = R1;
        run(xoris(r2, r1, UIMM));
        expect(state.r[2] == R2);
        expect(!state.cr[0].lt);
        expect(!state.cr[0].gt);
        expect(!state.cr[0].eq);
        expect(!state.cr[0].so);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_xoris(0x00000000FF0000FFULL, 0x0F0F, 0x00000000F00F00FFULL);
    test_xoris(0x0000FFFFFFFFFF00ULL, 0xFFFF, 0x0000FFFF0000FF00ULL);
}
