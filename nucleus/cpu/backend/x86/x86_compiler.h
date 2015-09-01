/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/backend/compiler.h"
#include "nucleus/cpu/backend/x86/x86_emitter.h"

#include <memory>

namespace cpu {
namespace backend {
namespace x86 {

// Class declarations
class X86Emitter;

class X86Compiler : public Compiler {
    std::unique_ptr<X86Emitter> emitter;

public:
    // Constructor
    X86Compiler();

    virtual bool compile(hir::Block* block) override;
    virtual bool compile(hir::Function* function) override;
    virtual bool compile(hir::Module* module) override;
};

}  // namespace x86
}  // namespace backend
}  // namespace cpu
