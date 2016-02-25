/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "function.h"
#include "nucleus/cpu/hir/block.h"
#include "nucleus/cpu/hir/module.h"

namespace cpu {
namespace hir {

Function::Function(Module* parent, TypeOut tOut, TypeIn tIn)
    : parent(parent), typeOut(tOut), typeIn(tIn), flags(0), nativeAddress(nullptr) {
    // Set flags
    flags |= FUNCTION_IS_DECLARED;

    // Argument values
    for (auto type : tIn) {
        Value* value = new Value();
        value->type = type;
        args.push_back(value);
    }

    // Add function to parent module
    parent->addFunction(this);
}

Function::~Function() {
    for (auto block : blocks) {
        delete block;
    }
}

S32 Function::getId() {
    if (id < 0) {
        id = parent->functionIdCounter++;
    }
    return id;
}

void Function::reset() {
    flags = FUNCTION_IS_DECLARED;
    blocks.clear();
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
}  // namespace cpu
