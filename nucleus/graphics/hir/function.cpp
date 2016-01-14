/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "function.h"
#include "nucleus/graphics/hir/block.h"
#include "nucleus/graphics/hir/module.h"

namespace gfx {
namespace hir {

Function::Function(Module* parent)
    : parent(parent), flags(0)  {
    // Set flags
    flags |= FUNCTION_IS_DECLARED;

    // Add function to parent module
    parent->addFunction(this);
}

Function::~Function() {
    for (auto block : blocks) {
        delete block;
    }
}

S32 Function::getId() {
    return id;
}

std::string Function::dump() {
    std::string output;
    output += "f" + std::to_string(getId()) + "() {\n";
    for (const auto& block : blocks) {
        output += block->dump();
    }
    output += "}\n\n";
    return output;
}

}  // namespace hir
}  // namespace gfx
