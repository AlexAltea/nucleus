/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/backend/compiler.h"

#include <memory>

namespace cpu {
namespace backend {
namespace x86 {

enum X86Extension {
    AVX    = (1 << 0),  // Advanced Vector Extensions
    AVX2   = (1 << 1),  // Advanced Vector Extensions 2
    AVX512 = (1 << 2),  // Advanced Vector Extensions 3
    BMI2   = (1 << 3),  // Bit Manipulation Instructions 2
    LZCNT  = (1 << 4),  // Leading Zeros Count
    MOVBE  = (1 << 5),  // Move Data After Swapping Bytes
    SSSE3  = (1 << 6),  // Supplemental Streaming SIMD Extensions 3
};

class X86Compiler : public Compiler {
private:
    // Initialize compiler
    void init();

public:
    // Available x86 extensions
    U32 extensions;

    // Constructor
    X86Compiler();
    X86Compiler(const Settings& settings);

    virtual bool compile(hir::Block* block) override;
    virtual bool compile(hir::Function* function) override;
    virtual bool compile(hir::Module* module) override;

    virtual bool call(hir::Function* function, void* state, const std::vector<hir::Value*>& args = {}) override;
};

}  // namespace x86
}  // namespace backend
}  // namespace cpu
