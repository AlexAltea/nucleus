/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include "llvm/IR/Value.h"

namespace cpu {
namespace hir {

template <typename T=Any, int N=1>
class Value
{
public:
    llvm::Value* value;

    // Constructor
    Value(llvm::Value* v = nullptr) : value(v) {}

    // Conversion
    operator llvm::Value*() { return value; }
};

}  // namespace hir
}  // namespace cpu
