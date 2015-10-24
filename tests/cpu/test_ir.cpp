/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

// Visual Studio testing dependencies
#include "CppUnitTest.h"

// Target
#include "nucleus/cpu/hir/builder.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/hir/function.h"
#include "nucleus/cpu/hir/module.h"
#include "nucleus/cpu/hir/passes.h"
#include "nucleus/cpu/backend/x86/x86_compiler.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// Target
using namespace cpu::hir;
using namespace cpu::backend;

TEST_CLASS(CpuIrTests) {

public:
    TEST_METHOD(CPU_BackendTests) {
        Module* module = new Module();
        Function* function = new Function(module, TYPE_I64, {TYPE_I64, TYPE_I64});
        Block* block = new Block(function);

        Builder builder;
        builder.setInsertPoint(block);
        auto result1 = builder.createAdd(function->args[0], function->args[1]);
        auto result2 = builder.createMul(result1, builder.getConstantI64(4));
        builder.createRet(result2);
        function->flags |= FUNCTION_IS_DEFINED;

        Compiler* compiler = new x86::X86Compiler();
        compiler->addPass(std::make_unique<passes::RegisterAllocationPass>(compiler->targetInfo));
        compiler->compile(module);
        //auto result = function->call(3,4);
        //Assert::IsTrue(result == 28);
    }
};
