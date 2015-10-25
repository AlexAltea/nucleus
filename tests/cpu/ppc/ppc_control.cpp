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
    // Move from XER register
    TEST_INSTRUCTION(test_mfxer, SO, OV, CA, BC, RD, {
        state.xer.so = SO;
        state.xer.ov = OV;
        state.xer.ca = CA;
        state.xer.bc = BC;
        run(mfxer(r1));
        expect(state.r[1] == RD);
    });

    test_mfxer(0,0,0, 0x00, 0x0000000000000000ULL);
    test_mfxer(1,0,0, 0x00, 0x0000000080000000ULL);
    test_mfxer(0,1,0, 0x00, 0x0000000040000000ULL);
    test_mfxer(0,0,1, 0x00, 0x0000000020000000ULL);
    test_mfxer(0,0,0, 0x01, 0x0000000000000001ULL);
    test_mfxer(0,0,0, 0x7F, 0x000000000000007FULL);

    // Move from LR register
    TEST_INSTRUCTION(test_mflr, LR, RD, {
        state.lr = LR;
        run(mflr(r1));
        expect(state.r[1] == RD);
    });

    test_mflr(0x0000000000000000ULL, 0x0000000000000000ULL);
    test_mflr(0x0000000000000001ULL, 0x0000000000000001ULL);
    test_mflr(0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL);

    // Move from CTR register
    TEST_INSTRUCTION(test_mfctr, CTR, RD, {
        state.ctr = CTR;
        run(mfctr(r1));
        expect(state.r[1] == RD);
    });

    test_mfctr(0x0000000000000000ULL, 0x0000000000000000ULL);
    test_mfctr(0x0000000000000001ULL, 0x0000000000000001ULL);
    test_mfctr(0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL);
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
    // Move to XER register
    TEST_INSTRUCTION(test_mtxer, RS, SO, OV, CA, BC, {
        state.r[1] = RS;
        run(mtxer(r1));
        expect(state.xer.so == SO);
        expect(state.xer.ov == OV);
        expect(state.xer.ca == CA);
        expect(state.xer.bc == BC);
    });

    test_mtxer(0x0000000000000000ULL, 0,0,0, 0x00);
    test_mtxer(0x0000000080000000ULL, 1,0,0, 0x00);
    test_mtxer(0x0000000040000000ULL, 0,1,0, 0x00);
    test_mtxer(0x0000000020000000ULL, 0,0,1, 0x00);
    test_mtxer(0x0000000000000001ULL, 0,0,0, 0x01);
    test_mtxer(0x000000000000007FULL, 0,0,0, 0x7F);

    // Move to LR register
    TEST_INSTRUCTION(test_mtlr, RS, LR, {
        state.r[1] = RS;
        run(mtlr(r1));
        expect(state.lr == LR);
    });

    test_mtlr(0x0000000000000000ULL, 0x0000000000000000ULL);
    test_mtlr(0x0000000000000001ULL, 0x0000000000000001ULL);
    test_mtlr(0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL);

    // Move to CTR register
    TEST_INSTRUCTION(test_mtctr, RS, CTR, {
        state.r[1] = RS;
        run(mtctr(r1));
        expect(state.ctr == CTR);
    });

    test_mtctr(0x0000000000000000ULL, 0x0000000000000000ULL);
    test_mtctr(0x0000000000000001ULL, 0x0000000000000001ULL);
    test_mtctr(0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL);
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
