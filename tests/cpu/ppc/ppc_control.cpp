/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "tests/cpu/test_ppc.h"

/**
 * PPC64 Instructions:
 *  - UISA: Processor Control Instructions (Section: 4.2.5)
 *  - VEA: Processor Control Instructions (Section: 4.3.1)
 *  - VEA: Memory Control Instructions (Section: 4.3.3)
 *  - VEA: External Control Instructions (Section: 4.3.4)
 */

void PPCTestRunner::mfocrf() {
    // Move from One Condition Register Field
    TEST_INSTRUCTION(test_mfocrf, CR, CRM, RD, RDDefined, {
        state.setCR(CR);
        run(mfocrf(r1, CRM));
        expect((state.r[1] & RDDefined) == RD);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_mfocrf(0x12481248, 0x01, 0x0000000000000008ULL, 0x000000000000000FULL);
    test_mfocrf(0x12481248, 0x80, 0x0000000010000000ULL, 0x00000000F0000000ULL);
    test_mfocrf(0x12481248, 0x81, 0x0000000000000000ULL, 0x0000000000000000ULL);
    test_mfocrf(0x12481248, 0xFF, 0x0000000000000000ULL, 0x0000000000000000ULL);

    // Move from Condition Register
    TEST_INSTRUCTION(test_mfcr, oldRD, CR, newRD, {
        state.r[1] = oldRD;
        state.setCR(CR);
        run(mfcr(r1));
        expect(state.r[1] == newRD);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_mfcr(0x0000000000000000ULL, 0x12481248, 0x0000000012481248ULL);
    test_mfcr(0xFFFFFFFFFFFFFFFFULL, 0x12481248, 0x0000000012481248ULL);
}

void PPCTestRunner::mfspr() {
}

void PPCTestRunner::mtocrf() {
    // Move to One Condition Register Field
    TEST_INSTRUCTION(test_mtocrf, RS, CRM, CR, CRDefined, {
        state.r[1] = RS;
        run(mtocrf(CRM, r1));
        expect((state.getCR() & CRDefined) == CR);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_mtocrf(0x0000000012481248, 0x01, 0x00000008, 0xFFFFFFFF);
    test_mtocrf(0x0000000012481248, 0x80, 0x10000000, 0xFFFFFFFF);
    test_mtocrf(0x0000000012481248, 0x81, 0x00000000, 0x00000000);
    test_mtocrf(0x0000000012481248, 0xFF, 0x00000000, 0x00000000);

    // Move to Condition Register Field
    TEST_INSTRUCTION(test_mtcrf, RS, CRM, CR, {
        state.r[1] = RS;
        run(mtcrf(CRM, r1));
        expect(state.getCR() == CR);
        expect(!state.xer.so);
        expect(!state.xer.ov);
        expect(!state.xer.ca);
    });

    test_mtcrf(0x0000000012481248, 0x01, 0x00000008);
    test_mtcrf(0x0000000012481248, 0x80, 0x10000000);
    test_mtcrf(0x0000000012481248, 0x81, 0x10000008);
    test_mtcrf(0x0000000012481248, 0xFF, 0x12481248);
}

void PPCTestRunner::mtspr() {
}

void PPCTestRunner::mftb() {
}

void PPCTestRunner::dcbf() {
}

void PPCTestRunner::dcbst() {
}

void PPCTestRunner::dcbt() {
}

void PPCTestRunner::dcbtst() {
}

void PPCTestRunner::dcbz() {
}

void PPCTestRunner::icbi() {
}

void PPCTestRunner::eciwx() {
}

void PPCTestRunner::ecowx() {
}
