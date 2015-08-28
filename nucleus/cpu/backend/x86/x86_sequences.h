/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/backend/sequences.h"

namespace cpu {
namespace backend {
namespace x86 {

class X86Sequences {
    static std::unordered_map<InstrKey, X86SequenceFunction> sequences;
public:

    // Constructor
    X86Sequences();

void emit();
};

}  // namespace x86
}  // namespace backend
}  // namespace cpu
