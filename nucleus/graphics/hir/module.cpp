/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "module.h"
#include "nucleus/graphics/hir/function.h"
#include "nucleus/graphics/hir/instruction.h"

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

    // Header
    for (const auto& i : hCapabilities)     { i->dump(); }
    for (const auto& i : hExtensions)       { i->dump(); }
    for (const auto& i : hImports)          { i->dump(); }
    for (const auto& i : hMemoryModel)      { i->dump(); }
    for (const auto& i : hEntryPoints)      { i->dump(); }
    for (const auto& i : hExecutions)       { i->dump(); }
    for (const auto& i : hDebugs)           { i->dump(); }
    for (const auto& i : hAnnotation)       { i->dump(); }
    for (const auto& i : hConstsTypesGlobs) { i->dump(); }

    // Functions
    for (const auto& function : functions) {
        output += function->dump();
    }
    return output;
}

}  // namespace hir
}  // namespace gfx
