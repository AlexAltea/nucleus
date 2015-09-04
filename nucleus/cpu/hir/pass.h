/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/hir/function.h"

namespace cpu {
namespace hir {

class Pass {
public:
    /**
     * Get the name of this pass
     * @return               Name of this pass
     */
    virtual const char* name() = 0;

    /**
     * Apply this pass on a function
     * @param[in]  function  Function to be processed
     * @return               True on success
     */
    virtual bool run(Function* function) = 0;
};

}  // namespace hir
}  // namespace cpu
