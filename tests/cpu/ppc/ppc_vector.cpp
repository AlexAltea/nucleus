/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "tests/cpu/test_ppc.h"

/**
 * PPC64 Vector/SIMD Instructions (aka AltiVec):
 *  - Vector UISA Instructions (Section: 4.2.x)
 *  - Vector VEA Instructions (Section: 4.3.x)
 */

void PPCTestRunner::dss() {
}

void PPCTestRunner::dst() {
}

void PPCTestRunner::dstst() {
}

void PPCTestRunner::lvebx() {
}

void PPCTestRunner::lvehx() {
}

void PPCTestRunner::lvewx() {
}

void PPCTestRunner::lvlx() {
}

void PPCTestRunner::lvlxl() {
}

void PPCTestRunner::lvrx() {
}

void PPCTestRunner::lvrxl() {
}

void PPCTestRunner::lvsl() {
}

void PPCTestRunner::lvsr() {
}

void PPCTestRunner::lvx() {
}

void PPCTestRunner::lvxl() {
}

void PPCTestRunner::mfvscr() {
}

void PPCTestRunner::mtvscr() {
}

void PPCTestRunner::stvebx() {
}

void PPCTestRunner::stvehx() {
}

void PPCTestRunner::stvewx() {
}

void PPCTestRunner::stvlx() {
}

void PPCTestRunner::stvlxl() {
}

void PPCTestRunner::stvrx() {
}

void PPCTestRunner::stvrxl() {
}

void PPCTestRunner::stvx() {
}

void PPCTestRunner::stvxl() {
}

void PPCTestRunner::vaddcuw() {
}

void PPCTestRunner::vaddfp() {
}

void PPCTestRunner::vaddsbs() {
}

void PPCTestRunner::vaddshs() {
}

void PPCTestRunner::vaddsws() {
}

void PPCTestRunner::vaddubm() {
}

void PPCTestRunner::vaddubs() {
}

void PPCTestRunner::vadduhm() {
}

void PPCTestRunner::vadduhs() {
}

void PPCTestRunner::vadduwm() {
}

void PPCTestRunner::vadduws() {
}

void PPCTestRunner::vand() {
}

void PPCTestRunner::vandc() {
}

void PPCTestRunner::vavgsb() {
}

void PPCTestRunner::vavgsh() {
}

void PPCTestRunner::vavgsw() {
}

void PPCTestRunner::vavgub() {
}

void PPCTestRunner::vavguh() {
}

void PPCTestRunner::vavguw() {
}

void PPCTestRunner::vcfsx() {
}

void PPCTestRunner::vcfux() {
}

void PPCTestRunner::vcmpbfpx() {
}

void PPCTestRunner::vcmpeqfpx() {
}

void PPCTestRunner::vcmpequbx() {
}

void PPCTestRunner::vcmpequhx() {
}

void PPCTestRunner::vcmpequwx() {
}

void PPCTestRunner::vcmpgefpx() {
}

void PPCTestRunner::vcmpgtfpx() {
}

void PPCTestRunner::vcmpgtsbx() {
}

void PPCTestRunner::vcmpgtshx() {
}

void PPCTestRunner::vcmpgtswx() {
}

void PPCTestRunner::vcmpgtubx() {
}

void PPCTestRunner::vcmpgtuhx() {
}

void PPCTestRunner::vcmpgtuwx() {
}

void PPCTestRunner::vctsxs() {
}

void PPCTestRunner::vctuxs() {
}

void PPCTestRunner::vexptefp() {
    TEST_INSTRUCTION(test_vexptefp, V1, V2, {
        state.v[1] = V1;
        run({ a.vexptefp(v2, v1); });
        expect(state.v[2] == V2);
    });

    test_vexptefp(
        V128::from_u32(0x40000000, 0x40400000, 0x40800000, 0x40A00000),
        V128::from_u32(0x40800000, 0x41000000, 0x41800000, 0x42000000));
}

void PPCTestRunner::vlogefp() {
    TEST_INSTRUCTION(test_vlogefp, V1, V2, {
        state.v[1] = V1;
        run({ a.vlogefp(v2, v1); });
        expect(state.v[2] == V2);
    });

    // TODO
}

void PPCTestRunner::vmaddfp() {
    TEST_INSTRUCTION(test_vmaddfp, V1, V2, V3, V4, {
        state.v[1] = V1;
        state.v[2] = V2;
        state.v[3] = V3;
        run({ a.vmaddfp(v4, v1, v2, v3); });
        expect(state.v[4] == V4);
    });

    test_vmaddfp(
        V128::from(+1.0f, +1.0f, +2.0f, +0.0f),
        V128::from(+0.0f, +1.0f, +3.0f, +1.0f),
        V128::from(+1.0f, -1.0f, -1.0f, -7.0f),
        V128::from(+1.0f, +0.0f, +5.0f, -7.0f));
}

void PPCTestRunner::vmaxfp() {
    TEST_INSTRUCTION(test_vmaxfp, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmaxfp(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vmaxfp(
        V128::from(+10.0f, -10.0f, +15.0f, -15.0f),
        V128::from(-10.0f, +20.0f, -20.0f, +30.0f),
        V128::from(+10.0f, +20.0f, +15.0f, +30.0f));
}

void PPCTestRunner::vmaxsb() {
    TEST_INSTRUCTION(test_vmaxsb, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmaxsb(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vmaxsh() {
    TEST_INSTRUCTION(test_vmaxsh, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmaxsh(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vmaxsh(
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x80060007),
        V128::from_u32(0x00080009, 0x000A000B, 0x000C000D, 0x000E000F),
        V128::from_u32(0x00080009, 0x000A000B, 0x000C000D, 0x000E000F));
    test_vmaxsh(
        V128::from_u32(0x00000009, 0x0002000B, 0x0004000D, 0x0006000F),
        V128::from_u32(0x00080001, 0x000A0003, 0x000C0005, 0x000E0007),
        V128::from_u32(0x00080009, 0x000A000B, 0x000C000D, 0x000E000F));
}

void PPCTestRunner::vmaxsw() {
    TEST_INSTRUCTION(test_vmaxsw, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmaxsw(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vmaxub() {
    TEST_INSTRUCTION(test_vmaxub, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmaxub(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vmaxuh() {
    TEST_INSTRUCTION(test_vmaxuh, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmaxuh(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vmaxuh(
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007),
        V128::from_u32(0x00080009, 0x000A000B, 0x000C000D, 0x800E000F),
        V128::from_u32(0x00080009, 0x000A000B, 0x000C000D, 0x800E000F));
    test_vmaxuh(
        V128::from_u32(0x00000009, 0x0002000B, 0x0004000D, 0x0006000F),
        V128::from_u32(0x00080001, 0x000A0003, 0x000C0005, 0x000E0007),
        V128::from_u32(0x00080009, 0x000A000B, 0x000C000D, 0x000E000F));
}

void PPCTestRunner::vmaxuw() {
    TEST_INSTRUCTION(test_vmaxuw, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmaxuw(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vmhaddshs() {
    TEST_INSTRUCTION(test_vmhaddshs, V1, V2, V3, V4, {
        state.v[1] = V1;
        state.v[2] = V2;
        state.v[3] = V3;
        run({ a.vmhaddshs(v4, v1, v2, v3); });
        expect(state.v[4] == V4);
    });

    // TODO
}

void PPCTestRunner::vmhraddshs() {
    TEST_INSTRUCTION(test_vmhaddshs, V1, V2, V3, V4, {
        state.v[1] = V1;
        state.v[2] = V2;
        state.v[3] = V3;
        run({ a.vmhaddshs(v4, v1, v2, v3); });
        expect(state.v[4] == V4);
    });

    // TODO
}

void PPCTestRunner::vminfp() {
    TEST_INSTRUCTION(test_vminfp, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vminfp(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vminfp(
        V128::from(+10.0f, -10.0f, +15.0f, -15.0f),
        V128::from(-10.0f, +20.0f, -20.0f, +30.0f),
        V128::from(-10.0f, -10.0f, -20.0f, -15.0f));
}

void PPCTestRunner::vminsb() {
    TEST_INSTRUCTION(test_vminsb, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vminsb(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vminsh() {
    TEST_INSTRUCTION(test_vminsh, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vminsh(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vminsh(
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007),
        V128::from_u32(0x00080009, 0x000A000B, 0x000C000D, 0x000E000F),
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007));
    test_vminsh(
        V128::from_u32(0x00000009, 0x0002000B, 0x0004000D, 0x0006000F),
        V128::from_u32(0x00080001, 0x000A0003, 0x000C0005, 0x000E0007),
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007));
}

void PPCTestRunner::vminsw() {
    TEST_INSTRUCTION(test_vminsw, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vminsw(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vminub() {
    TEST_INSTRUCTION(test_vminub, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vminub(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vminuh() {
    TEST_INSTRUCTION(test_vminuh, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vminuh(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vminuh(
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007),
        V128::from_u32(0x00080009, 0x000A000B, 0x000C000D, 0x000E000F),
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007));
    test_vminuh(
        V128::from_u32(0x00000009, 0x0002000B, 0x0004000D, 0x0006000F),
        V128::from_u32(0x00080001, 0x000A0003, 0x000C0005, 0x000E0007),
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007));
}

void PPCTestRunner::vminuw() {
    TEST_INSTRUCTION(test_vminuw, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vminuw(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vmladduhm() {
    TEST_INSTRUCTION(test_vmladduhm, V1, V2, V3, V4, {
        state.v[1] = V1;
        state.v[2] = V2;
        state.v[3] = V3;
        run({ a.vmladduhm(v4, v1, v2, v3); });
        expect(state.v[4] == V4);
    });

    // TODO
}

void PPCTestRunner::vmrghb() {
    TEST_INSTRUCTION(test_vmrghb, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmrghb(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vmrghb(
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x00100111, 0x02120313, 0x04140515, 0x06160717));
    test_vmrghb(
        V128::from_u32(0xF0F1F2F3, 0xF4F5F6F7, 0xF8F9FAFB, 0xFCFDFEFF),
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0xF000F101, 0xF202F303, 0xF404F505, 0xF606F707));
}

void PPCTestRunner::vmrghh() {
    TEST_INSTRUCTION(test_vmrghh, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmrghh(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vmrghh(
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x00011011, 0x02031213, 0x04051415, 0x06071617));
    test_vmrghh(
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007),
        V128::from_u32(0x00080009, 0x000A000B, 0x000C000D, 0x000E000F),
        V128::from_u32(0x00000008, 0x00010009, 0x0002000A, 0x0003000B));
    test_vmrghh(
        V128::from_u32(0xFFF8FFF9, 0xFFFAFFFB, 0xFFFCFFFD, 0xFFFEFFFF),
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007),
        V128::from_u32(0xFFF80000, 0xFFF90001, 0xFFFA0002, 0xFFFB0003));
}

void PPCTestRunner::vmrghw() {
    TEST_INSTRUCTION(test_vmrghw, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmrghw(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vmrghw(
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x00010203, 0x10111213, 0x04050607, 0x14151617));
    test_vmrghw(
        V128::from_u32(0x00000000, 0x00000001, 0x00000002, 0x00000003),
        V128::from_u32(0x00000004, 0x00000005, 0x00000006, 0x00000007),
        V128::from_u32(0x00000000, 0x00000004, 0x00000001, 0x00000005));
    test_vmrghw(
        V128::from_u32(0xC0800000, 0xC0400000, 0xC0000000, 0xBF800000),
        V128::from_u32(0x00000000, 0x3F800000, 0x40000000, 0x40400000),
        V128::from_u32(0xC0800000, 0x00000000, 0xC0400000, 0x3F800000));
    test_vmrghw(
        V128::from_u32(0xFFFFFFFC, 0xFFFFFFFD, 0xFFFFFFFE, 0xFFFFFFFF),
        V128::from_u32(0x00000000, 0x00000001, 0x00000002, 0x00000003),
        V128::from_u32(0xFFFFFFFC, 0x00000000, 0xFFFFFFFD, 0x00000001));
}

void PPCTestRunner::vmrglb() {
    TEST_INSTRUCTION(test_vmrglb, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmrglb(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vmrglb(
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x08180919, 0x0A1A0B1B, 0x0C1C0D1D, 0x0E1E0F1F));
    test_vmrglb(
        V128::from_u32(0xF0F1F2F3, 0xF4F5F6F7, 0xF8F9FAFB, 0xFCFDFEFF),
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0xF808F909, 0xFA0AFB0B, 0xFC0CFD0D, 0xFE0EFF0F));
}

void PPCTestRunner::vmrglh() {
    TEST_INSTRUCTION(test_vmrglh, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmrglh(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vmrglh(
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x08091819, 0x0A0B1A1B, 0x0C0D1C1D, 0x0E0F1E1F));
    test_vmrglh(
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007),
        V128::from_u32(0x00080009, 0x000A000B, 0x000C000D, 0x000E000F),
        V128::from_u32(0x0004000C, 0x0005000D, 0x0006000E, 0x0007000F));
    test_vmrglh(
        V128::from_u32(0xFFF8FFF9, 0xFFFAFFFB, 0xFFFCFFFD, 0xFFFEFFFF),
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007),
        V128::from_u32(0xFFFC0004, 0xFFFD0005, 0xFFFE0006, 0xFFFF0007));
}

void PPCTestRunner::vmrglw() {
    TEST_INSTRUCTION(test_vmrglw, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmrglw(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vmrglw(
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x08090A0B, 0x18191A1B, 0x0C0D0E0F, 0x1C1D1E1F));
    test_vmrglw(
        V128::from_u32(0x00000000, 0x00000001, 0x00000002, 0x00000003),
        V128::from_u32(0x00000004, 0x00000005, 0x00000006, 0x00000007),
        V128::from_u32(0x00000002, 0x00000006, 0x00000003, 0x00000007));
    test_vmrglw(
        V128::from_u32(0xC0800000, 0xC0400000, 0xC0000000, 0xBF800000),
        V128::from_u32(0x00000000, 0x3F800000, 0x40000000, 0x40400000),
        V128::from_u32(0xC0000000, 0x40000000, 0xBF800000, 0x40400000));
    test_vmrglw(
        V128::from_u32(0xFFFFFFFC, 0xFFFFFFFD, 0xFFFFFFFE, 0xFFFFFFFF),
        V128::from_u32(0x00000000, 0x00000001, 0x00000002, 0x00000003),
        V128::from_u32(0xFFFFFFFE, 0x00000002, 0xFFFFFFFF, 0x00000003));
}

void PPCTestRunner::vmsummbm() {
    TEST_INSTRUCTION(test_vmsummbm, V1, V2, V3, V4, {
        state.v[1] = V1;
        state.v[2] = V2;
        state.v[3] = V3;
        run({ a.vmsummbm(v4, v1, v2, v3); });
        expect(state.v[4] == V4);
    });

    // TODO
}

void PPCTestRunner::vmsumshm() {
    TEST_INSTRUCTION(test_vmsumshm, V1, V2, V3, V4, {
        state.v[1] = V1;
        state.v[2] = V2;
        state.v[3] = V3;
        run({ a.vmsumshm(v4, v1, v2, v3); });
        expect(state.v[4] == V4);
    });

    // TODO
}

void PPCTestRunner::vmsumshs() {
    TEST_INSTRUCTION(test_vmsumshs, V1, V2, V3, V4, {
        state.v[1] = V1;
        state.v[2] = V2;
        state.v[3] = V3;
        run({ a.vmsumshs(v4, v1, v2, v3); });
        expect(state.v[4] == V4);
    });

    // TODO
}

void PPCTestRunner::vmsumubm() {
    TEST_INSTRUCTION(test_vmsumubm, V1, V2, V3, V4, {
        state.v[1] = V1;
        state.v[2] = V2;
        state.v[3] = V3;
        run({ a.vmsumubm(v4, v1, v2, v3); });
        expect(state.v[4] == V4);
    });

    // TODO
}

void PPCTestRunner::vmsumuhm() {
    TEST_INSTRUCTION(test_vmsumuhm, V1, V2, V3, V4, {
        state.v[1] = V1;
        state.v[2] = V2;
        state.v[3] = V3;
        run({ a.vmsumuhm(v4, v1, v2, v3); });
        expect(state.v[4] == V4);
    });

    // TODO
}

void PPCTestRunner::vmsumuhs() {
    TEST_INSTRUCTION(test_vmsumuhs, V1, V2, V3, V4, {
        state.v[1] = V1;
        state.v[2] = V2;
        state.v[3] = V3;
        run({ a.vmsumuhs(v4, v1, v2, v3); });
        expect(state.v[4] == V4);
    });

    // TODO
}

void PPCTestRunner::vmulesb() {
    TEST_INSTRUCTION(test_vmulesb, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmulesb(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vmulesh() {
    TEST_INSTRUCTION(test_vmulesh, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmulesh(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vmuleub() {
    TEST_INSTRUCTION(test_vmuleub, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmuleub(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vmuleuh() {
    TEST_INSTRUCTION(test_vmuleuh, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmuleuh(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vmulosb() {
    TEST_INSTRUCTION(test_vmulosb, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmulosb(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vmulosh() {
    TEST_INSTRUCTION(test_vmulosh, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmulosh(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vmuloub() {
    TEST_INSTRUCTION(test_vmuloub, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmuloub(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vmulouh() {
    TEST_INSTRUCTION(test_vmulouh, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vmulouh(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    // TODO
}

void PPCTestRunner::vnmsubfp() {
    // TODO
}

void PPCTestRunner::vnor() {
    // TODO
}

void PPCTestRunner::vor() {
    TEST_INSTRUCTION(test_vor, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vor(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vor(
        V128::from_u32(0xFFFF0101, 0x7070FFFF, 0xFFFFFFFF, 0x00000000),
        V128::from_u32(0x80081010, 0x808F0000, 0x00000000, 0x8FFFFFFF),
        V128::from_u32(0xFFFF1111, 0xF0FFFFFF, 0xFFFFFFFF, 0x8FFFFFFF));
}

void PPCTestRunner::vperm() {
    TEST_INSTRUCTION(test_vperm, V1, V2, V3, V4, {
        state.v[1] = V1;
        state.v[2] = V2;
        state.v[3] = V3;
        run({ a.vperm(v4, v1, v2, v3); });
        expect(state.v[4] == V4);
    });

    test_vperm(
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000),
        V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000));
    test_vperm(
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x01010101, 0x01010101, 0x01010101, 0x01010101),
        V128::from_u32(0x01010101, 0x01010101, 0x01010101, 0x01010101));
    test_vperm(
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x11111111, 0x11111111, 0x11111111, 0x11111111),
        V128::from_u32(0x11111111, 0x11111111, 0x11111111, 0x11111111));
    test_vperm(
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x21212121, 0x21212121, 0x21212121, 0x21212121),
        V128::from_u32(0x01010101, 0x01010101, 0x01010101, 0x01010101));
}

void PPCTestRunner::vpkpx() {
    TEST_INSTRUCTION(test_vpkpx, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vpkpx(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vpkpx(
        V128::from_u32(0x00101820, 0x01283038, 0x00404850, 0x01586068),
        V128::from_u32(0x01707880, 0x00889098, 0x01A0A8B0, 0x00B8C0C8),
        V128::from_u32(0x086494C7, 0x212AAD8D, 0xB9F04653, 0xD2B65F19));
}

void PPCTestRunner::vpkshss() {
    TEST_INSTRUCTION(test_vpkshss, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vpkshss(v3, v1, v2); });
        expect(state.v[3] == V3);
        // TODO: Test SAT bit
    });

    test_vpkshss(
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007),
        V128::from_u32(0x00080009, 0x000A000B, 0x000C000D, 0x000E000F),
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F));
    test_vpkshss(
        V128::from_u32(0x7FFF8000, 0x00020003, 0x00040005, 0x00060007),
        V128::from_u32(0x7FFF8000, 0x000A000B, 0x000C000D, 0x000E000F),
        V128::from_u32(0x7F800203, 0x04050607, 0x7F800A0B, 0x0C0D0E0F));
    test_vpkshss(
        V128::from_u32(0xFFFFFF80, 0x0000007F, 0xFFFEFF7F, 0x00010080),
        V128::from_u32(0xFFFDFF7E, 0x00020081, 0xFFFCFF7D, 0x00030082),
        V128::from_u32(0xFF80007F, 0xFE80017F, 0xFD80027F, 0xFC80037F));
}

void PPCTestRunner::vpkshus() {
    TEST_INSTRUCTION(test_vpkshus, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vpkshus(v3, v1, v2); });
        expect(state.v[3] == V3);
        // TODO: Test SAT bit
    });

    test_vpkshus(
        V128::from_u32(0xFFFFFF80, 0x0000007F, 0xFFFEFF7F, 0x00010080),
        V128::from_u32(0xFFFDFF7E, 0x00020081, 0xFFFCFF7D, 0x00030082),
        V128::from_u32(0x0000007F, 0x00000180, 0x00000281, 0x00000382));
}

void PPCTestRunner::vpkswss() {
    TEST_INSTRUCTION(test_vpkswss, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vpkswss(v3, v1, v2); });
        expect(state.v[3] == V3);
        // TODO: Test SAT bit
    });

    test_vpkswss(
        V128::from_u32(0x00000001, 0x00000002, 0x00000003, 0x00000004),
        V128::from_u32(0x00000005, 0x00000006, 0x00000007, 0x00000008),
        V128::from_u32(0x00010002, 0x00030004, 0x00050006, 0x00070008));
    test_vpkswss(
        V128::from_u32(0x7FFFFFFF, 0x80000000, 0x00000000, 0x00000004),
        V128::from_u32(0x7FFFFFFF, 0x80000000, 0x00000000, 0x00000008),
        V128::from_u32(0x7FFF8000, 0x00000004, 0x7FFF8000, 0x00000008));
    test_vpkswss(
        V128::from_u32(0xFFFFFFFF, 0xFFFF8000, 0x00000000, 0x00007FFF),
        V128::from_u32(0xFFFFFFFE, 0xFFFF7FFF, 0x00000001, 0x00008000),
        V128::from_u32(0xFFFF8000, 0x00007FFF, 0xFFFE8000, 0x00017FFF));
}

void PPCTestRunner::vpkswus() {
    TEST_INSTRUCTION(test_vpkswus, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vpkswus(v3, v1, v2); });
        expect(state.v[3] == V3);
        // TODO: Test SAT bit
    });

    test_vpkswus(
        V128::from_u32(0xFFFFFFFF, 0xFFFF8000, 0x00000000, 0x00007FFF),
        V128::from_u32(0xFFFFFFFE, 0xFFFF7FFF, 0x00000001, 0x00008000),
        V128::from_u32(0x00000000, 0x00007FFF, 0x00000000, 0x00018000));
}

void PPCTestRunner::vpkuhum() {
    TEST_INSTRUCTION(test_vpkuhum, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vpkuhum(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vpkuhum(
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007),
        V128::from_u32(0x00080009, 0x000A000B, 0x000C000D, 0x000E000F),
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F));
    test_vpkuhum(
        V128::from_u32(0xFFF8FFF9, 0xFFFAFFFB, 0xFFFCFFFD, 0xFFFEFFFF),
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007),
        V128::from_u32(0xF8F9FAFB, 0xFCFDFEFF, 0x00010203, 0x04050607));
    test_vpkuhum(
        V128::from_u32(0x0000FFFF, 0xFFFF0000, 0x00000000, 0xFFFF0000),
        V128::from_u32(0xFFFF0000, 0x0000FFFF, 0xFFFFFFFF, 0x0000FFFF),
        V128::from_u32(0x00FFFF00, 0x0000FF00, 0xFF0000FF, 0xFFFF00FF));
}

void PPCTestRunner::vpkuhus() {
    TEST_INSTRUCTION(test_vpkuhus, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vpkuhus(v3, v1, v2); });
        expect(state.v[3] == V3);
        // TODO: Test SAT bit
    });

    test_vpkuhus(
        V128::from_u32(0x00000100, 0x00010101, 0x00020102, 0x00030103),
        V128::from_u32(0x00040104, 0x00050105, 0x00060106, 0x00070107),
        V128::from_u32(0x00FF01FF, 0x02FF03FF, 0x04FF05FF, 0x06FF07FF));
}

void PPCTestRunner::vpkuwum() {
    TEST_INSTRUCTION(test_vpkuwum, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vpkuwum(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vpkuwum(
        V128::from_u32(0x00000000, 0x00000001, 0x00000002, 0x00000003),
        V128::from_u32(0x00000004, 0x00000005, 0x00000006, 0x00000007),
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007));
    test_vpkuwum(
        V128::from_u32(0xFFFFFFFC, 0xFFFFFFFD, 0xFFFFFFFE, 0xFFFFFFFF),
        V128::from_u32(0x00000000, 0x00000001, 0x00000002, 0x00000003),
        V128::from_u32(0xFFFCFFFD, 0xFFFEFFFF, 0x00000001, 0x00020003));
    test_vpkuwum(
        V128::from_u32(0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000),
        V128::from_u32(0x0000FFFF, 0xFFFFFFFF, 0xFFFF0000, 0x00000000));
}

void PPCTestRunner::vpkuwus() {
    TEST_INSTRUCTION(test_vpkuwus, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vpkuwus(v3, v1, v2); });
        expect(state.v[3] == V3);
        // TODO: Test SAT bit
    });

    test_vpkuwus(
        V128::from_u32(0x00000000, 0x00010000, 0x00000001, 0x00010001),
        V128::from_u32(0x00000002, 0x00010002, 0x00000003, 0x00010003),
        V128::from_u32(0x0000FFFF, 0x0001FFFF, 0x0002FFFF, 0x0003FFFF));
    test_vpkuwus(
        V128::from_u32(0x80000000, 0x7FFFFFFF, 0x00000002, 0x00000003),
        V128::from_u32(0xFFFFFFFF, 0x00010002, 0xFFFFFFFE, 0x00000010),
        V128::from_u32(0xFFFFFFFF, 0x00020003, 0xFFFFFFFF, 0xFFFF0010));
}

void PPCTestRunner::vrefp() {
    // TODO
}

void PPCTestRunner::vrfim() {
    // TODO
}

void PPCTestRunner::vrfin() {
    TEST_INSTRUCTION(test_vrfin, V1, V2, {
        state.v[1] = V1;
        run({ a.vrfin(v2, v1); });
        expect(state.v[2] == V2);
    });

    test_vrfin(
        V128::from(+1.0f, +1.5f, +1.1f, +1.9f),
        V128::from(+1.0f, +2.0f, +1.0f, +2.0f));
    test_vrfin(
        V128::from(-1.0f, -1.5f, -1.1f, -1.9f),
        V128::from(-1.0f, -2.0f, -1.0f, -2.0f));
}

void PPCTestRunner::vrfip() {
    // TODO
}

void PPCTestRunner::vrfiz() {
    // TODO
}

void PPCTestRunner::vrlb() {
    // TODO
}

void PPCTestRunner::vrlh() {
    TEST_INSTRUCTION(test_vrlh, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vrlh(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vrlh(
        V128::from_u32(0x12345678, 0x87654321, 0x11223344, 0x55667788),
        V128::from_u32(0x000D000D, 0x000D000D, 0x000D000D, 0x000D000D),
        V128::from_u32(0x82460ACF, 0xB0EC2864, 0x42248668, 0xCAAC0EF1));
}

void PPCTestRunner::vrlw() {
    // TODO
}

void PPCTestRunner::vrsqrtefp() {
    // TODO
}

void PPCTestRunner::vsel() {
    TEST_INSTRUCTION(test_vsel, V1, V2, V3, V4, {
        state.v[1] = V1;
        state.v[2] = V2;
        state.v[3] = V3;
        run({ a.vsel(v4, v1, v2, v3); });
        expect(state.v[4] == V4);
    });

    test_vsel(
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000),
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F));
    test_vsel(
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F));
    test_vsel(
        V128::from_u32(0x0C010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x1D111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x10101010, 0x10101010, 0x10101010, 0x10101010),
        V128::from_u32(0x1C111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F));
}

void PPCTestRunner::vsl() {
    TEST_INSTRUCTION(test_vsl, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vsl(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vsl(
        V128::from_u32(0xFEFEFEFE, 0xFEFEFEFE, 0xFEFEFEFE, 0xFEFEFEFE),
        V128::from_u32(0x04040404, 0x04040404, 0x04040404, 0x04040404),
        V128::from_u32(0xEFEFEFEF, 0xEFEFEFEF, 0xEFEFEFEF, 0xEFEFEFE0));
    test_vsl(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0x04040404, 0x04040404, 0x04040404, 0x04040404),
        V128::from_u32(0x01122334, 0x45566778, 0x899AABBC, 0xCDDEEFF0));
    test_vsl(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0x07070707, 0x07070707, 0x07070707, 0x07070707),
        V128::from_u32(0x089119A2, 0x2AB33BC4, 0x4CD55DE6, 0x6EF77F80));
    test_vsl(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x089119A2, 0x2AB33BC4, 0x4CD55DE6, 0x6EF77F80));
}

void PPCTestRunner::vslb() {
    TEST_INSTRUCTION(test_vslb, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vslb(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vslb(
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000),
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
    test_vslb(
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x01010101, 0x01010101, 0x01010101, 0x01010101),
        V128::from_u32(0xFEFEFEFE, 0xFEFEFEFE, 0xFEFEFEFE, 0xFEFEFEFE));
    test_vslb(
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x07070707, 0x07070707, 0x07070707, 0x07070707),
        V128::from_u32(0x80808080, 0x80808080, 0x80808080, 0x80808080));
    test_vslb(
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x08080808, 0x08080808, 0x08080808, 0x08080808),
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
}

void PPCTestRunner::vsldoi() {
    TEST_INSTRUCTION(test_vsldoi, UIMM, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vsldoi(v3, v1, v2, UIMM); });
        expect(state.v[3] == V3);
    });

    test_vsldoi(0,
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F));
    test_vsldoi(1,
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10));
    test_vsldoi(15,
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x10111213, 0x14151617, 0x18191A1B, 0x1C1D1E1F),
        V128::from_u32(0x0F101112, 0x13141516, 0x1718191A, 0x1B1C1D1E));
}

void PPCTestRunner::vslh() {
    TEST_INSTRUCTION(test_vslh, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vslh(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vslh(
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000),
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
    test_vslh(
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x00010001, 0x00010001, 0x00010001, 0x00010001),
        V128::from_u32(0xFFFEFFFE, 0xFFFEFFFE, 0xFFFEFFFE, 0xFFFEFFFE));
    test_vslh(
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x000F000F, 0x000F000F, 0x000F000F, 0x000F000F),
        V128::from_u32(0x80008000, 0x80008000, 0x80008000, 0x80008000));
    test_vslh(
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x00100010, 0x00100010, 0x00100010, 0x00100010),
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
}

void PPCTestRunner::vslo() {
    TEST_INSTRUCTION(test_vslo, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vslo(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vslo(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0x04040404, 0x04040404, 0x04040404, 0x04040404),
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF));
    test_vslo(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0x08080808, 0x08080808, 0x08080808, 0x08080808),
        V128::from_u32(0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00));
    test_vslo(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0x12121212, 0x12121212, 0x12121212, 0x12121212),
        V128::from_u32(0x22334455, 0x66778899, 0xAABBCCDD, 0xEEFF0000));
    test_vslo(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0x80808080, 0x80808080, 0x80808080, 0x80808080),
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF));
    test_vslo(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0xFF000000, 0x00000000, 0x00000000, 0x00000000));
}

void PPCTestRunner::vslw() {
    TEST_INSTRUCTION(test_vslw, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vslw(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vslw(
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000),
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
    test_vslw(
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x00000001, 0x00000001, 0x00000001, 0x00000001),
        V128::from_u32(0xFFFFFFFE, 0xFFFFFFFE, 0xFFFFFFFE, 0xFFFFFFFE));
    test_vslw(
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x0000001F, 0x0000001F, 0x0000001F, 0x0000001F),
        V128::from_u32(0x80000000, 0x80000000, 0x80000000, 0x80000000));
    test_vslw(
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x00000020, 0x00000020, 0x00000020, 0x00000020),
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
}

void PPCTestRunner::vspltb() {
    TEST_INSTRUCTION(test_vspltb, UIMM, V1, V2, {
        state.v[1] = V1;
        run({ a.vspltb(v2, v1, UIMM); });
        expect(state.v[2] == V2);
    });

    test_vspltb(0,
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000));
    test_vspltb(1,
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x01010101, 0x01010101, 0x01010101, 0x01010101));
    test_vspltb(15,
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x0F0F0F0F, 0x0F0F0F0F, 0x0F0F0F0F, 0x0F0F0F0F));
}

void PPCTestRunner::vsplth() {
    TEST_INSTRUCTION(test_vsplth, UIMM, V1, V2, {
        state.v[1] = V1;
        run({ a.vsplth(v2, v1, UIMM); });
        expect(state.v[2] == V2);
    });

    test_vsplth(0,
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x00010001, 0x00010001, 0x00010001, 0x00010001));
    test_vsplth(1,
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x02030203, 0x02030203, 0x02030203, 0x02030203));
    test_vsplth(7,
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x0E0F0E0F, 0x0E0F0E0F, 0x0E0F0E0F, 0x0E0F0E0F));
}

void PPCTestRunner::vspltisb() {
    TEST_INSTRUCTION(test_vspltisb, SIMM, V1, {
        run({ a.vspltisb(v1, SIMM); });
        expect(state.v[1] == V1);
    });

    test_vspltisb( 0, V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000));
    test_vspltisb( 1, V128::from_u32(0x01010101, 0x01010101, 0x01010101, 0x01010101));
    test_vspltisb(-1, V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
    test_vspltisb(-2, V128::from_u32(0xFEFEFEFE, 0xFEFEFEFE, 0xFEFEFEFE, 0xFEFEFEFE));
}

void PPCTestRunner::vspltish() {
    TEST_INSTRUCTION(test_vspltish, SIMM, V1, {
        run({ a.vspltish(v1, SIMM); });
        expect(state.v[1] == V1);
    });

    test_vspltish( 0, V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000));
    test_vspltish( 1, V128::from_u32(0x00010001, 0x00010001, 0x00010001, 0x00010001));
    test_vspltish(-1, V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
    test_vspltish(-2, V128::from_u32(0xFFFEFFFE, 0xFFFEFFFE, 0xFFFEFFFE, 0xFFFEFFFE));
}

void PPCTestRunner::vspltisw() {
    TEST_INSTRUCTION(test_vspltisw, SIMM, V1, {
        run({ a.vspltisw(v1, SIMM); });
        expect(state.v[1] == V1);
    });

    test_vspltisw( 0, V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000));
    test_vspltisw( 1, V128::from_u32(0x00000001, 0x00000001, 0x00000001, 0x00000001));
    test_vspltisw(-1, V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
    test_vspltisw(-2, V128::from_u32(0xFFFFFFFE, 0xFFFFFFFE, 0xFFFFFFFE, 0xFFFFFFFE));
}

void PPCTestRunner::vspltw() {
    TEST_INSTRUCTION(test_vspltw, UIMM, V1, V2, {
        state.v[1] = V1;
        run({ a.vspltw(v2, v1, UIMM); });
        expect(state.v[2] == V2);
    });

    test_vspltw(0,
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x00010203, 0x00010203, 0x00010203, 0x00010203));
    test_vspltw(1,
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x04050607, 0x04050607, 0x04050607, 0x04050607));
    test_vspltw(3,
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x0C0D0E0F, 0x0C0D0E0F, 0x0C0D0E0F, 0x0C0D0E0F));
}

void PPCTestRunner::vsr() {
    TEST_INSTRUCTION(test_vsr, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vsr(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vsr(
        V128::from_u32(0xFEFEFEFE, 0xFEFEFEFE, 0xFEFEFEFE, 0xFEFEFEFE),
        V128::from_u32(0x04040404, 0x04040404, 0x04040404, 0x04040404),
        V128::from_u32(0x0FEFEFEF, 0xEFEFEFEF, 0xEFEFEFEF, 0xEFEFEFEF));
    test_vsr(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0x04040404, 0x04040404, 0x04040404, 0x04040404),
        V128::from_u32(0x00011223, 0x34455667, 0x78899AAB, 0xBCCDDEEF));
    test_vsr(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0x07070707, 0x07070707, 0x07070707, 0x07070707),
        V128::from_u32(0x00002244, 0x6688AACC, 0xEF113355, 0x7799BBDD));
    test_vsr(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x00002244, 0x6688AACC, 0xEF113355, 0x7799BBDD));
}

void PPCTestRunner::vsrab() {
    // TODO
}

void PPCTestRunner::vsrah() {
    TEST_INSTRUCTION(test_vsrah, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vsrah(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vsrah(
        V128::from_u32(0x12345678, 0x87654321, 0x11223344, 0x55667788),
        V128::from_u32(0x000D000D, 0x000D000D, 0x000D000D, 0x000D000D),
        V128::from_u32(0x00000002, 0xFFFC0002, 0x00000001, 0x00020003));
}

void PPCTestRunner::vsraw() {
    // TODO
}

void PPCTestRunner::vsrb() {
    // TODO
}

void PPCTestRunner::vsrh() {
    TEST_INSTRUCTION(test_vsrh, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vsrh(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vsrh(
        V128::from_u32(0x12345678, 0x87654321, 0x11223344, 0x55667788),
        V128::from_u32(0x000D000D, 0x000D000D, 0x000D000D, 0x000D000D),
        V128::from_u32(0x00000002, 0x00040002, 0x00000001, 0x00020003));
}

void PPCTestRunner::vsro() {
    TEST_INSTRUCTION(test_vsro, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vsro(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vsro(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0x04040404, 0x04040404, 0x04040404, 0x04040404),
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF));
    test_vsro(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0x08080808, 0x08080808, 0x08080808, 0x08080808),
        V128::from_u32(0x00001122, 0x33445566, 0x778899AA, 0xBBCCDDEE));
    test_vsro(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0x12121212, 0x12121212, 0x12121212, 0x12121212),
        V128::from_u32(0x00000011, 0x22334455, 0x66778899, 0xAABBCCDD));
    test_vsro(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0x80808080, 0x80808080, 0x80808080, 0x80808080),
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF));
    test_vsro(
        V128::from_u32(0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF),
        V128::from_u32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
        V128::from_u32(0x00000000, 0x00000000, 0x00000000, 0x00000000));
}

void PPCTestRunner::vsrw() {
    // TODO
}

void PPCTestRunner::vsubcuw() {
    // TODO
}

void PPCTestRunner::vsubfp() {
    TEST_INSTRUCTION(test_vsubuhm, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vsubuhm(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vsubuhm(
        V128::from(+10.0f, -10.0f, +15.0f, -15.0f),
        V128::from(-10.0f, -10.0f, +20.0f, +30.0f),
        V128::from(+20.0f,  +0.0f,  -5.0f, -45.0f));
}

void PPCTestRunner::vsubsbs() {
    // TODO
}

void PPCTestRunner::vsubshs() {
    TEST_INSTRUCTION(test_vsubshs, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vsubshs(v3, v1, v2); });
        expect(state.v[3] == V3);
        // TODO: Test SAT bit
    });

    test_vsubshs(
        V128::from_u32(0x7FFF8001, 0x7FFF8003, 0x7FFF8005, 0x80068007),
        V128::from_u32(0x00018001, 0x10000000, 0x42568124, 0x00000000),
        V128::from_u32(0x7FFE0000, 0x6FFF8003, 0x3DA9FEE1, 0x80068007));
}

void PPCTestRunner::vsubsws() {
    // TODO
}

void PPCTestRunner::vsububm() {
    // TODO
}

void PPCTestRunner::vsububs() {
    // TODO
}

void PPCTestRunner::vsubuhm() {
    TEST_INSTRUCTION(test_vsubuhm, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vsubuhm(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vsubuhm(
        V128::from_u32(0x7FFF8001, 0x7FFF8003, 0x7FFF8005, 0x80068007),
        V128::from_u32(0x00018001, 0x10000000, 0x42568124, 0x00000000),
        V128::from_u32(0x7FFE0000, 0x6FFF8003, 0x3DA9FEE1, 0x80068007));
}

void PPCTestRunner::vsubuhs() {
    // TODO
}

void PPCTestRunner::vsubuwm() {
    // TODO
}

void PPCTestRunner::vsubuws() {
    // TODO
}

void PPCTestRunner::vsum2sws() {
    // TODO
}

void PPCTestRunner::vsum4sbs() {
    // TODO
}

void PPCTestRunner::vsum4shs() {
    // TODO
}

void PPCTestRunner::vsum4ubs() {
    // TODO
}

void PPCTestRunner::vsumsws() {
    // TODO
}

void PPCTestRunner::vupkhpx() {
    // TODO
}

void PPCTestRunner::vupkhsb() {
    TEST_INSTRUCTION(test_vupkhsb, V1, V2, {
        state.v[1] = V1;
        run({ a.vupkhsb(v2, v1); });
        expect(state.v[2] == V2);
    });

    test_vupkhsb(
        V128::from_u32(0xF8F9FAFB, 0xFCFDFEFF, 0x00010203, 0x04050607),
        V128::from_u32(0xFFF8FFF9, 0xFFFAFFFB, 0xFFFCFFFD, 0xFFFEFFFF));
    test_vupkhsb(
        V128::from_u32(0x00FFFF00, 0x0000FF00, 0xFF0000FF, 0xFFFF00FF),
        V128::from_u32(0x0000FFFF, 0xFFFF0000, 0x00000000, 0xFFFF0000));
}

void PPCTestRunner::vupkhsh() {
    TEST_INSTRUCTION(test_vupkhsh, V1, V2, {
        state.v[1] = V1;
        run({ a.vupkhsh(v2, v1); });
        expect(state.v[2] == V2);
    });

    test_vupkhsh(
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x00000001, 0x00000203, 0x00000405, 0x00000607));
    test_vupkhsh(
        V128::from_u32(0x7F800203, 0x04050607, 0x7F800A0B, 0x0C0D0E0F),
        V128::from_u32(0x00007F80, 0x00000203, 0x00000405, 0x00000607));
    test_vupkhsh(
        V128::from_u32(0x0000FFFF, 0xFFFF0000, 0x00000000, 0xFFFF0000),
        V128::from_u32(0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000));
    test_vupkhsh(
        V128::from_u32(0xFFFCFFFD, 0xFFFEFFFF, 0x00000001, 0x00020003),
        V128::from_u32(0xFFFFFFFC, 0xFFFFFFFD, 0xFFFFFFFE, 0xFFFFFFFF));
}

void PPCTestRunner::vupklpx() {
    // TODO
}

void PPCTestRunner::vupklsb() {
    TEST_INSTRUCTION(test_vupklsb, V1, V2, {
        state.v[1] = V1;
        run({ a.vupklsb(v2, v1); });
        expect(state.v[2] == V2);
    });

    test_vupklsb(
        V128::from_u32(0xF8F9FAFB, 0xFCFDFEFF, 0x00010203, 0x04050607),
        V128::from_u32(0x00000001, 0x00020003, 0x00040005, 0x00060007));
    test_vupklsb(
        V128::from_u32(0x00FFFF00, 0x0000FF00, 0xFF0000FF, 0xFFFF00FF),
        V128::from_u32(0xFFFF0000, 0x0000FFFF, 0xFFFFFFFF, 0x0000FFFF));
}

void PPCTestRunner::vupklsh() {
    TEST_INSTRUCTION(test_vupklsh, V1, V2, {
        state.v[1] = V1;
        run({ a.vupklsh(v2, v1); });
        expect(state.v[2] == V2);
    });

    test_vupklsh(
        V128::from_u32(0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F),
        V128::from_u32(0x00000809, 0x00000A0B, 0x00000C0D, 0x00000E0F));
    test_vupklsh(
        V128::from_u32(0xFFFFFFFF, 0x00000A0B, 0x00007FFF, 0x8000FFFF),
        V128::from_u32(0x00000000, 0x00007FFF, 0xFFFF8000, 0xFFFFFFFF));
}

void PPCTestRunner::vxor() {
    TEST_INSTRUCTION(test_vxor, V1, V2, V3, {
        state.v[1] = V1;
        state.v[2] = V2;
        run({ a.vxor(v3, v1, v2); });
        expect(state.v[3] == V3);
    });

    test_vxor(
        V128::from_u32(0xFFFF0101, 0x7070FFFF, 0xFFFFFFFF, 0x00000000),
        V128::from_u32(0x80081010, 0x7070FFFF, 0x7FFFFFFF, 0x8FFFFFFF),
        V128::from_u32(0x7FF71111, 0x00000000, 0x80000000, 0x8FFFFFFF));
}
