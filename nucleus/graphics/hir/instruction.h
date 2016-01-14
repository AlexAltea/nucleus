/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/hir/hir.h"

#include <string>
#include <vector>

namespace gfx {
namespace hir {

class Instruction {
public:
    Literal resultId;
    Literal typeId;
    Literal opcode;
    std::vector<Literal> operands;
    std::string comment;
};

}  // namespace hir
}  // namespace gfx
