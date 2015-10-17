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

TEST_CLASS(PPUTests) {
public:
    hir::Module* module;
    hir::Function* function;
    hir::Block* block;

    std::unique_ptr<backend::Compiler> compiler;

    U32 buffer[256];
    PPUState state;

    PPUTests() {
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

    TEST_METHOD(PPU_addx) {
        // Add with condition
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

        test_add(0x00000001'0000FFFFULL, 0x00000002'00000001ULL, 0x00000003'00010000ULL);
        test_add(0xFFFFFFFF'FFFFFFFFULL, 0x00000000'00000001ULL, 0x00000000'00000000ULL);

        // Add with condition
        TEST_INSTRUCTION(test_add_, R1, R2, R3, LT, GT, EQ, SO, {
            state.r[1] = R1;
            state.r[2] = R2;
            run(add_(r3, r1, r2));
            then(state.r[3] == R3);
            then(state.cr[0].lt == LT);
            then(state.cr[0].gt == GT);
            then(state.cr[0].eq == EQ);
            then(state.cr[0].so == SO);
        });

        test_add_(0x00000001'0000FFFFULL, 0x00000002'00000001ULL, 0x00000003'00010000ULL, false, false, false, false);
        test_add_(0xFFFFFFFF'FFFFFFFFULL, 0x00000000'00000001ULL, 0x00000000'00000000ULL, false, false, false, false);

        // Add with overflow
        TEST_INSTRUCTION(test_addo, R1, R2, R3, OV, SO, {
            state.r[1] = R1;
            state.r[2] = R2;
            run(addo(r3, r1, r2));
            then(state.r[3] == R3);
            then(state.xer.ov == OV);
            then(state.xer.so == SO);
        });

        test_addo(0x00000001'0000FFFFULL, 0x00000002'00000001ULL, 0x00000003'00010000ULL, true, true);
        test_addo(0xFFFFFFFF'FFFFFFFFULL, 0x00000000'00000001ULL, 0x00000000'00000000ULL, true, false);

        // Add with overflow and condition
        TEST_INSTRUCTION(test_addo_, R1, R2, R3, OV, SO, {
            state.r[1] = R1;
            state.r[2] = R2;
            run(addo_(r3, r1, r2));
            then(state.r[3] == R3);
            then(state.xer.ov == OV);
            then(state.xer.so == SO);
        });

        test_addo_(0x00000001'0000FFFFULL, 0x00000002'00000001ULL, 0x00000003'00010000ULL, true, true);
        test_addo_(0xFFFFFFFF'FFFFFFFFULL, 0x00000000'00000001ULL, 0x00000000'00000000ULL, true, false);
    }

    TEST_METHOD(PPU_addzex) {
    }

#undef run
#undef then
};
