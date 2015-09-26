/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/backend/sequences.h"
#include "nucleus/cpu/backend/x86/x86_emitter.h"

#include <unordered_map>

namespace cpu {
namespace backend {
namespace x86 {

class X86Sequences {
    // Sequence selection function type
    using SelectFunction = void(*)(X86Emitter&, const hir::Instruction*);

    // Registered sequences
    static std::unordered_map<InstrKey::Value, SelectFunction> sequences;

    // Sequence registration
    template <typename T>
    static void registerSequence() {
        sequences.insert({ T::key, T::select });
    }
    template <typename T0, typename T1, typename... Ts>
    static void registerSequence() {
        registerSequence<T0>();
        registerSequence<T1, Ts...>();
    }

public:
    /**
     * Initialize the table of sequences
     */
    static void init();

    /**
     * Emit the corresponding x86 instructions for a given IR instruction sequence
     * @param[in]  emitter  Emitter of x86 machine code
     * @param[in]  instr    Instruction pointer
     * @return              True on success
     */
    static bool select(X86Emitter& emitter, const hir::Instruction* instr);
};

}  // namespace x86
}  // namespace backend
}  // namespace cpu
