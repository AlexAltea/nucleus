/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace cpu {
namespace backend {

struct Settings {
    bool isCached;
    bool isJIT;
    bool isAOT;
};

}  // namespace backend
}  // namespace cpu
