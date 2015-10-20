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

// Initialize guest thread state
#define TEST_INSTRUCTION_INIT() \
    memset(&state, 0, sizeof(state))

// Execute assembler generated PowerPC code
#define run(expr) \
    execute([&](PPCAssembler& a) { a.##expr##; })

// Check if expected condition holds
#define expect(expr) \
    Assert::IsTrue(expr)

class PPCTestRunner {
public:
    hir::Module* module;
    hir::Function* function;
    hir::Block* block;

    std::unique_ptr<backend::Compiler> compiler;

    U32 buffer[256];
    PPUState state;

    PPCTestRunner() {
        module = new hir::Module();
        function = new hir::Function(module, hir::TYPE_VOID);

        compiler = std::make_unique<backend::x86::X86Compiler>();
        compiler->addPass(std::make_unique<hir::passes::RegisterAllocationPass>(compiler->targetInfo));
    }

protected:
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
#define INSTRUCTION(name) void name()
#include "test_ppu.inl"
#undef INSTRUCTION
};
