/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "module.h"
#include "nucleus/graphics/hir/function.h"

namespace gfx {
namespace hir {

bool Module::setInputs(std::vector<Type> inputTypes) {
    S32 index = 0;
    inputs.clear();
    for (auto& type : inputTypes) {
        auto* value = new Value();
        value->flags |= VALUE_IS_INPUT;
        value->type = type;
        value->setId(index++);
        inputs.push_back(value);
    }
    return true;
}

bool Module::setOutputs(std::vector<Type> outputTypes) {
    S32 index = 0;
    outputs.clear();
    for (auto& type : outputTypes) {
        auto* value = new Value();
        value->flags |= VALUE_IS_OUTPUT;
        value->type = type;
        value->setId(index++);
        inputs.push_back(value);
    }
    return true;
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
