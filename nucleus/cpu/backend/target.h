/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <vector>

namespace cpu {
namespace backend {

struct RegisterSet {
    enum Type {
        TYPE_INT     = (1 << 1),
        TYPE_FLOAT   = (1 << 2),
        TYPE_VECTOR  = (1 << 3),
    };

    U32 types;
    U32 count;

    // Calling convention
    std::vector<int> argIndex;
    int retIndex;
};

struct TargetInfo {
    RegisterSet regSets[4];
};

}  // namespace backend
}  // namespace cpu
