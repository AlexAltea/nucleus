/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

// Visual Studio testing dependencies
#include "CppUnitTest.h"

// Nucleus testing dependencies
#include "tests/cpu/common.h"

// Target
#include "nucleus/cpu/cpu.h"
#include "nucleus/cpu/backend/x86/x86_compiler.h"
#include "nucleus/cpu/backend/spu/spu_assembler.h"
#include "nucleus/cpu/frontend/spu/spu_state.h"
#include "nucleus/cpu/frontend/spu/spu_tables.h"
#include "nucleus/cpu/frontend/spu/translator/spu_translator.h"
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
using namespace cpu::backend::spu;
using namespace cpu::frontend::spu;

// Initialize guest thread state
#define TEST_INSTRUCTION_INIT() \
    memset(&state, 0, sizeof(state))

// Execute assembler generated PowerPC code
#define run(expr) \
    execute([&](SPUAssembler& a) expr)

// Check if expected condition holds
#define expect(expr) \
    Assert::IsTrue(expr)

class SPUTestRunner {
public:
    hir::Module* module;
    hir::Function* function;
    hir::Block* block;

    std::unique_ptr<backend::Compiler> compiler;

    // Mock hardware
    std::shared_ptr<mem::Memory> memory;
    std::shared_ptr<cpu::CPU> cpu;

    U32 buffer[256];
    SPUState state;

    SPUTestRunner() {
        module = new hir::Module();
        function = new hir::Function(module, hir::TYPE_VOID);

        // Create mock hardware
        memory = std::make_shared<mem::Memory>();
        cpu = std::make_shared<cpu::CPU>(memory);

        compiler = std::make_unique<backend::x86::X86Compiler>();
        compiler->addPass(std::make_unique<hir::passes::RegisterAllocationPass>(compiler->targetInfo));
    }

protected:
    void execute(std::function<void(SPUAssembler&)> spuFunc) {
        function->reset();
        block = new hir::Block(function);

        Translator translator(cpu.get(), nullptr);
        translator.builder.setInsertPoint(block);
        translator.currentAddress = 0x10000;

        U32 buffer[256];
        SPUAssembler a(sizeof(buffer), buffer);
        spuFunc(a);

        for (Size i = 0; (i * sizeof(U32)) < a.curSize; i++) {
            Instruction instr;
            instr.value = static_cast<U32*>(a.codeAddr)[i];
            auto method = get_entry(instr).recompile;
            (translator.*method)(instr);
        }

        compiler->compile(function);
        compiler->call(function, &state);
    }

public:
#define INSTRUCTION(name) void name()
#include "test_spu.inl"
#undef INSTRUCTION
};
