/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "function.h"
#include "nucleus/emulator.h"
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

Value* Function::call(const std::vector<Value*>& args) {
    if (flags & FUNCTION_IS_COMPILED) {
        nucleus.cell.compiler->translationEnter();
        auto function = reinterpret_cast<void(*)()>(nativeAddress);
        function();
        nucleus.cell.compiler->translationExit();
    } else {
        logger.error(LOG_CPU, "Function is not ready");
        return 0;
    }
}

}  // namespace hir
}  // namespace cpu
