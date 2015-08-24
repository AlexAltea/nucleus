/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace cpu {
namespace hir {

class Block;
class Module;

class Function {
	Module* parent;

public:
	Block* entry;

    // Constructor
	Function(Module* parent);

    
    // Backend methods

    /**
     * Compile this function
     * @return           True on success
     */
    bool compile();
};

}  // namespace hir
}  // namespace cpu
