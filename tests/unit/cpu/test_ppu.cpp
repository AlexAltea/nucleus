/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

// Visual Studio testing dependencies
#include "CppUnitTest.h"

// Nucleus testing dependencies
#include "tests/unit/cpu/common.h"

// Target
#include "nucleus/cpu/backend/x86/x86_compiler.h"
#include "nucleus/cpu/backend/ppc/ppc_assembler.h"
#include "nucleus/cpu/frontend/ppu/ppu_state.h"
#include "nucleus/cpu/frontend/ppu/ppu_tables.h"
#include "nucleus/cpu/frontend/ppu/recompiler/ppu_recompiler.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/hir/function.h"
#include "nucleus/cpu/hir/module.h"
#include "nucleus/cpu/hir/passes.h"

// Utility
#include "nucleus/assert.h"

#include <functional>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// Target
using namespace cpu;
using namespace cpu::backend::ppc;
using namespace cpu::frontend::ppu;

TEST_CLASS(PPCTests) {
public:
    hir::Module* module;
    hir::Function* function;
    hir::Block* block;

    std::unique_ptr<backend::Compiler> compiler;

    U32 buffer[256];
    PPUState state;

    PPCTests() {
        module = new hir::Module();
        function = new hir::Function(module, hir::TYPE_VOID);

        compiler = std::make_unique<backend::x86::X86Compiler>();
        compiler->addPass(std::make_unique<hir::passes::RegisterAllocationPass>(compiler->targetInfo));
    }

private:
    void execute(std::function<void(PPCAssembler&)> ppcFunc) {
        function->reset();
        block = new hir::Block(function);

        Recompiler recompiler(nullptr);
        recompiler.builder.setInsertPoint(block);

        U32 buffer[256];
        PPCAssembler a(sizeof(buffer), buffer);
        ppcFunc(a);

        for (size_t i = 0; i < a.codeSize; i += 4) {
            Instruction instr;
            instr.instruction = *static_cast<U32*>(a.codeAddr);
            auto method = get_entry(instr).recompile;
            (recompiler.*method)(instr);
        }

        compiler->compile(function);
        compiler->call(function, &state);
    }

public:
#define run(t) \
    execute([&](PPCAssembler& a) {a.##t; })
#define then(exp) \
    Assert::IsTrue(exp)

    TEST_METHOD(PPC_addx) {
        // Add
        TEST_INSTRUCTION(test_add, R1, R2, R3, {
            state.r[1] = R1;
            state.r[2] = R2;
            run(add(r3, r1, r2));
            then(state.r[3] == R3);
            then(!state.cr[0].lt);
            then(!state.cr[0].gt);
            then(!state.cr[0].eq);
            then(!state.cr[0].so);
            then(!state.xer.so);
            then(!state.xer.ov);
            then(!state.xer.ca);
        });

        test_add(0x000000010000FFFFULL, 0x0000000200000001ULL, 0x0000000300010000ULL);
        test_add(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, 0x0000000000000000ULL);

        // Add (with condition)
        TEST_INSTRUCTION(test_add_, R1, R2, R3, LT, GT, EQ, SO, {
            state.r[1] = R1;
            state.r[2] = R2;
            run(add_(r3, r1, r2));
            then(state.r[3] == R3);
            then(state.cr[0].lt == LT);
            then(state.cr[0].gt == GT);
            then(state.cr[0].eq == EQ);
            then(state.cr[0].so == SO);
            then(!state.xer.so);
            then(!state.xer.ov);
            then(!state.xer.ca);
        });

        test_add_(0x000000010000FFFFULL, 0x0000000200000001ULL, 0x0000000300010000ULL, 0,1,0,0);
        test_add_(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, 0x0000000000000000ULL, 0,0,1,0);
        test_add_(0x0000000000000001ULL, 0x7FFFFFFFFFFFFFFFULL, 0x8000000000000000ULL, 1,0,0,0);
    }

    TEST_METHOD(PPC_addcx) {
        // Add Carrying
        TEST_INSTRUCTION(test_addc, R1, R2, oldCA, R3, newCA, {
            state.r[1] = R1;
            state.r[2] = R2;
            state.xer.ca = oldCA;
            run(addc(r3, r1, r2));
            then(state.r[3] == R3);
            then(!state.cr[0].lt);
            then(!state.cr[0].gt);
            then(!state.cr[0].eq);
            then(!state.cr[0].so);
            then(!state.xer.so);
            then(!state.xer.ov);
            then(state.xer.ca == newCA);
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
            then(state.r[3] == R3);
            then(state.cr[0].lt == LT);
            then(state.cr[0].gt == GT);
            then(state.cr[0].eq == EQ);
            then(state.cr[0].so == SO);
            then(!state.xer.so);
            then(!state.xer.ov);
            then(state.xer.ca == newCA);
        });

        test_addc_(0x000000010000FFFFULL, 0x0000000200000001ULL, false, 0x0000000300010000ULL, false, 0,1,0,0);
        test_addc_(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, false, 0x0000000000000000ULL, true,  0,0,1,0);
        test_addc_(0xFFFFFFFFFFFFFFF0ULL, 0x000000000000000FULL, true,  0xFFFFFFFFFFFFFFFFULL, false, 1,0,0,0);
        test_addc_(0x000000000000FFFFULL, 0xFFFFFFFFFFFF0011ULL, true,  0x0000000000000010ULL, true,  0,1,0,0);
    }

    TEST_METHOD(PPC_addex) {
        // Add Extended
        TEST_INSTRUCTION(test_adde, R1, oldCA, R2, R3, newCA, {
            state.r[1] = R1;
            state.r[2] = R2;
            state.xer.ca = oldCA;
            run(adde(r3, r1, r2));
            then(state.r[3] == R3);
            then(!state.cr[0].lt);
            then(!state.cr[0].gt);
            then(!state.cr[0].eq);
            then(!state.cr[0].so);
            then(!state.xer.so);
            then(!state.xer.ov);
            then(state.xer.ca == newCA);
        });

        test_adde(0x000000010000FFFFULL, false, 0x0000000200000001ULL, 0x0000000300010000ULL, false);
        test_adde(0xFFFFFFFFFFFFFFFFULL, false, 0x0000000000000001ULL, 0x0000000000000000ULL, true);
        test_adde(0x000000000000FFFFULL, true,  0x0000000000000000ULL, 0x0000000000010000ULL, false);
        test_adde(0xFFFFFFFFFFFFFFF0ULL, true,  0x000000000000000FULL, 0x0000000000000000ULL, true);
        
        // Add Extended (with condition)
        TEST_INSTRUCTION(test_adde_, R1, R2, oldCA, R3, newCA, LT, GT, EQ, SO, {
            state.r[1] = R1;
            state.r[2] = R2;
            state.xer.ca = oldCA;
            run(adde_(r3, r1, r2));
            then(state.r[3] == R3);
            then(state.cr[0].lt == LT);
            then(state.cr[0].gt == GT);
            then(state.cr[0].eq == EQ);
            then(state.cr[0].so == SO);
            then(!state.xer.so);
            then(!state.xer.ov);
            then(state.xer.ca == newCA);
        });

        test_adde_(0x000000010000FFFFULL, 0x0000000200000001ULL, false, 0x0000000300010000ULL, false, 0,1,0,0);
        test_adde_(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, false, 0x0000000000000000ULL, true,  0,0,1,0);
        test_adde_(0x000000000000FFFFULL, 0x0000000000000000ULL, true,  0x0000000000010000ULL, false, 0,1,0,0);
        test_adde_(0xFFFFFFFFFFFFFFFFULL, 0x8000000000000000ULL, true,  0x8000000000000000ULL, true,  1,0,0,0);
    }

    TEST_METHOD(PPC_addi) {
        // Add Immediate
        TEST_INSTRUCTION(test_addi, RAIndex, RA, SIMM, R2, {
            state.r[RAIndex] = RA;
            run(addi(r2, RAIndex, SIMM));
            then(state.r[2] == R2);
            then(!state.cr[0].lt);
            then(!state.cr[0].gt);
            then(!state.cr[0].eq);
            then(!state.cr[0].so);
            then(!state.xer.so);
            then(!state.xer.ov);
            then(!state.xer.ca);
        });

        test_addi(r1, 0x000000010000FFFFULL, 0x0001, 0x0000000100010000ULL);
        test_addi(r1, 0x00000000FFFFFFFFULL, 0x0001, 0x0000000100000000ULL);
        test_addi(r1, 0x00000000FFFF0001ULL, 0xFFFF, 0x00000000FFFF0000ULL);
        test_addi(r1, 0xFFFFFFFFFFFFFFFFULL, 0x0001, 0x0000000000000000ULL);
        test_addi(r0, 0x000000010000FFFFULL, 0x0001, 0x0000000000000001ULL);
        test_addi(r0, 0x00000000FFFFFFFFULL, 0xFFFF, 0xFFFFFFFFFFFFFFFFULL);
        test_addi(r0, 0x00000000FFFF0001ULL, 0x0000, 0x0000000000000000ULL);
    }

    TEST_METHOD(PPC_addic) {
        // Add Immediate
        TEST_INSTRUCTION(test_addic, R1, SIMM, R2, CA, {
            state.r[1] = R1;
            run(addic(r2, r1, SIMM));
            then(state.r[2] == R2);
            then(!state.cr[0].lt);
            then(!state.cr[0].gt);
            then(!state.cr[0].eq);
            then(!state.cr[0].so);
            then(!state.xer.so);
            then(!state.xer.ov);
            then(state.xer.ca == CA);
        });

        test_addic(0x000000010000FFFFULL, 0x0001, 0x0000000100010000ULL, false);
        // TODO: test_addic(0x00000000FFFFFFFFULL, 0x0001, 0x0000000100000000ULL, true);
        // TODO: test_addic(0x00000000FFFF0001ULL, 0xFFFF, 0x00000000FFFF0000ULL, false);
        test_addic(0xFFFFFFFFFFFFFFFFULL, 0x0001, 0x0000000000000000ULL, true);
    }

    TEST_METHOD(PPC_addzex) {
    }

    TEST_METHOD(PPC_andx) {
        // And
        TEST_INSTRUCTION(test_and_, R1, R2, R3, {
            state.r[1] = R1;
            state.r[2] = R2;
            run(and_(r3, r1, r2));
            then(state.r[3] == R3);
            then(!state.cr[0].lt);
            then(!state.cr[0].gt);
            then(!state.cr[0].eq);
            then(!state.cr[0].so);
            then(!state.xer.so);
            then(!state.xer.ov);
            then(!state.xer.ca);
        });

        test_and_(0x0000111100001111ULL, 0x1111111100000000ULL, 0x0000111100000000ULL);
        test_and_(0x00000000000000FFULL, 0x000000000000F0F0ULL, 0x00000000000000F0ULL);

        // And (with condition)
        TEST_INSTRUCTION(test_and__, R1, R2, R3, LT, GT, EQ, SO, {
            state.r[1] = R1;
            state.r[2] = R2;
            run(and__(r3, r1, r2));
            then(state.r[3] == R3);
            then(state.cr[0].lt == LT);
            then(state.cr[0].gt == GT);
            then(state.cr[0].eq == EQ);
            then(state.cr[0].so == SO);
            then(!state.xer.so);
            then(!state.xer.ov);
            then(!state.xer.ca);
        });

        test_and__(0x00000000FFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0x00000000FFFFFFFFULL, 0,1,0,0);
        test_and__(0xFFFFFFFF00000000ULL, 0x800000000000FFFFULL, 0x8000000000000000ULL, 1,0,0,0);
        test_and__(0xFFFFFFFF00000000ULL, 0x00000000FFFFFFFFULL, 0x0000000000000000ULL, 0,0,1,0);
    }

    TEST_METHOD(PPC_andcx) {
        // And with complement
        TEST_INSTRUCTION(test_andc, R1, R2, R3, {
            state.r[1] = R1;
            state.r[2] = R2;
            run(andc(r3, r1, r2));
            then(state.r[3] == R3);
            then(!state.cr[0].lt);
            then(!state.cr[0].gt);
            then(!state.cr[0].eq);
            then(!state.cr[0].so);
            then(!state.xer.so);
            then(!state.xer.ov);
            then(!state.xer.ca);
        });

        test_andc(0x0000111100001111ULL, 0x1111111100000000ULL, 0x0000000000001111ULL);
        test_andc(0x00000000000000FFULL, 0x000000000000F0F0ULL, 0x000000000000000FULL);

        // And with complement (with condition)
        TEST_INSTRUCTION(test_andc_, R1, R2, R3, LT, GT, EQ, SO, {
            state.r[1] = R1;
            state.r[2] = R2;
            run(andc_(r3, r1, r2));
            then(state.r[3] == R3);
            then(state.cr[0].lt == LT);
            then(state.cr[0].gt == GT);
            then(state.cr[0].eq == EQ);
            then(state.cr[0].so == SO);
            then(!state.xer.so);
            then(!state.xer.ov);
            then(!state.xer.ca);
        });

        test_andc_(0x00000000FFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL, 0,0,1,0);
        test_andc_(0xFFFFFFFF00005555ULL, 0xFFFFFFFFFFFF0000ULL, 0x0000000000005555ULL, 0,1,0,0);
        test_andc_(0xFFFFFFFF00000000ULL, 0x00000000FFFFFFFFULL, 0xFFFFFFFF00000000ULL, 1,0,0,0);
    }

    TEST_METHOD(PPC_andi_) {
        // Add Immediate
        TEST_INSTRUCTION(test_andi_, R1, UIMM, R2, LT, GT, EQ, SO, {
            state.r[1] = R1;
            run(andi_(r2, r1, UIMM));
            then(state.r[2] == R2);
            then(state.cr[0].lt == LT);
            then(state.cr[0].gt == GT);
            then(state.cr[0].eq == EQ);
            then(state.cr[0].so == SO);
            then(!state.xer.so);
            then(!state.xer.ov);
            then(!state.xer.ca);
        });

        test_andi_(0x0000FFFFFFFFFFF0ULL, 0x000F, 0x0000000000000000ULL, 0,0,1,0);
        test_andi_(0x0000FFFFFFFFFFFFULL, 0xFFFF, 0x000000000000FFFFULL, 0,1,0,0);
    }

    TEST_METHOD(PPC_andis_) {
        // Add Immediate
        TEST_INSTRUCTION(test_andis_, R1, UIMM, R2, LT, GT, EQ, SO, {
            state.r[1] = R1;
            run(andis_(r2, r1, UIMM));
            then(state.r[2] == R2);
            then(state.cr[0].lt == LT);
            then(state.cr[0].gt == GT);
            then(state.cr[0].eq == EQ);
            then(state.cr[0].so == SO);
            then(!state.xer.so);
            then(!state.xer.ov);
            then(!state.xer.ca);
        });

        test_andis_(0x0000FFFFFFF0FFFFULL, 0x000F, 0x0000000000000000ULL, 0,0,1,0);
        test_andis_(0x0000FFFFFFFFFFFFULL, 0xFFFF, 0x00000000FFFF0000ULL, 0,1,0,0);
    }

#undef run
#undef then
};
