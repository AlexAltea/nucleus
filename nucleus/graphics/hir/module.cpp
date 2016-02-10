/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "module.h"
#include "nucleus/graphics/hir/function.h"

namespace gfx {
namespace hir {

Module::Module() : idInstructions({ nullptr }) {
}

bool Module::addFunction(Function* function) {
    functions.push_back(function);
    return true;
}

std::string Module::dump() const {
    std::string output;
    for (const auto& function : functions) {
        output += function->dump();
    }
    return output;
}

}  // namespace hir
}  // namespace gfx
