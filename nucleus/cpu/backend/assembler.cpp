/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "assembler.h"
#include "nucleus/assert.h"

namespace cpu {
namespace backend {

Assembler::Assembler(size_t codeSize, void* codeAddr) :
    codeSize(codeSize), codeAddr(codeAddr) {
    curAddr = codeAddr;
    curSize = 0;
}

}  // namespace backend
}  // namespace cpu
