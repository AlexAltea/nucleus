/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/pass.h"

namespace cpu {
namespace hir {
namespace passes {

class RegisterAllocationPass : public Pass {
public:
    bool run(Function* function) override;
};

}  // namespace passes
}  // namespace hir
}  // namespace cpu
