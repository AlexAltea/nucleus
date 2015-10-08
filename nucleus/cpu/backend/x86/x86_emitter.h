/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/backend/settings.h"

// Xbyak dependency
#define XBYAK_NO_OP_NAMES
#include "externals/xbyak/xbyak.h"

#include <unordered_map>

namespace cpu {
namespace backend {
namespace x86 {

// Forward declarations
class X86Compiler;

enum X86Mode {
    X86_MODE_32BITS = (1 << 0),
    X86_MODE_64BITS = (1 << 1),
};

class X86Emitter : public Xbyak::CodeGenerator {
private:
    // Available x86 extensions
    const X86Compiler* compiler;

public:
    // Chosen x86 mode
    U32 mode;

    // Labels
    std::unordered_map<const hir::Block*, Xbyak::Label> labels;
    Xbyak::Label labelEntry;
    Xbyak::Label labelProlog;
    Xbyak::Label labelEpilog;

    // Constructor
    X86Emitter(const X86Compiler* compiler);
    X86Emitter(const X86Compiler* compiler, void* address, U64 size);

    /**
     * Check whether a specific x86 extension is available
     * @return True if the extension exists 
     */
    bool isExtensionAvailable(U32 queriedExtension) const;

    /**
     * Return global generic compiler settings
     * @return Compiler settings member
     */
    const Settings& settings() const;
};

}  // namespace x86
}  // namespace backend
}  // namespace cpu
