/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "tests/cpu/test_ppc.h"

/**
 * PPC64 Instructions:
 *  - UISA: Branch and Flow Control Instructions (Section: 4.2.4)
 */

void PPCTestRunner::bx() {
}

void PPCTestRunner::bcx() {
    // Add
    TEST_INSTRUCTION(test_bc, CR, BO, BI, DidBranch, {
        state.setCR(CR);
        run({
            a.li(r0, 1);
            a.bc(BO, BI, 8);
            a.li(r0, 0);
            a.nop();
        });
        expect(state.r[0] == U64(DidBranch));
    });
                             
    /**
     * Using CR := 0x84210953, therefore:
     *       CR == [ LT | GT | EQ | SO |    |LTSO|GTSO|EQSO]
     *       CR == [1000,0100,0010,0001,0000,1001,0101,0011]
     */
    test_bc(0x84219055, 12,  0, true);
    test_bc(0x84219055, 12,  1, false);
    test_bc(0x84219055, 12,  2, false);
    test_bc(0x84219055, 12,  3, false);
    test_bc(0x84219055, 12,  4, false);
    test_bc(0x84219055, 12,  5, true);
    test_bc(0x84219055, 12,  6, false);
    test_bc(0x84219055, 12,  7, false);
    test_bc(0x84219055, 12,  8, false);
    test_bc(0x84219055, 12,  9, false);
    test_bc(0x84219055, 12, 10, true);
    test_bc(0x84219055, 12, 11, false);
    test_bc(0x84219055, 12, 12, false);
    test_bc(0x84219055, 12, 13, false);
    test_bc(0x84219055, 12, 14, false);
    test_bc(0x84219055, 12, 15, true);
    test_bc(0x84219055, 12, 16, false);
    test_bc(0x84219055, 12, 17, false);
    test_bc(0x84219055, 12, 18, false);
    test_bc(0x84219055, 12, 19, false);
    test_bc(0x84219055, 12, 20, true);
    test_bc(0x84219055, 12, 21, false);
    test_bc(0x84219055, 12, 22, false);
    test_bc(0x84219055, 12, 23, true);
    test_bc(0x84219055, 12, 24, false);
    test_bc(0x84219055, 12, 25, true);
    test_bc(0x84219055, 12, 26, false);
    test_bc(0x84219055, 12, 27, true);
    test_bc(0x84219055, 12, 28, false);
    test_bc(0x84219055, 12, 29, false);
    test_bc(0x84219055, 12, 30, true);
    test_bc(0x84219055, 12, 31, true);
}

void PPCTestRunner::bcctrx() {
}

void PPCTestRunner::bclrx() {
}

void PPCTestRunner::crand() {
}

void PPCTestRunner::crandc() {
}

void PPCTestRunner::creqv() {
}

void PPCTestRunner::crnand() {
}

void PPCTestRunner::crnor() {
}

void PPCTestRunner::cror() {
}

void PPCTestRunner::crorc() {
}

void PPCTestRunner::crxor() {
}

void PPCTestRunner::mcrf() {
}

void PPCTestRunner::sc() {
}

void PPCTestRunner::td() {
}

void PPCTestRunner::tdi() {
}

void PPCTestRunner::tw() {
}

void PPCTestRunner::twi() {
}
