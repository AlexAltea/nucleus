/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace cpu {
namespace backend {

class Assembler {
public:
    void* curAddr;
    size_t curSize;

    void* codeAddr;
    size_t codeSize;

    Assembler(size_t codeSize = 4096, void* codeAddr = nullptr);
};

class Label {
public:
};

}  // namespace backend
}  // namespace cpu
