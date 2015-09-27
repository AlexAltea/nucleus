/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <list>

namespace cpu {
namespace hir {

class Function;
class Instruction;

class Block {
public:
    Function* parent;

    std::list<Instruction*> instructions;

    // Constructor
    Block(Function* parent);
    ~Block();
};

}  // namespace hir
}  // namespace cpu
