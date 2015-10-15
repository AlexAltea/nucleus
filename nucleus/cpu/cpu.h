/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/thread.h"
#include "nucleus/cpu/backend/compiler.h"

namespace cpu {

class CPU {
protected:
    std::unique_ptr<backend::Compiler> compiler;

    std::vector<Thread>
};

}  // namespace cpu
