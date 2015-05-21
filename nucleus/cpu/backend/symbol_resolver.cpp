/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "symbol_resolver.h"
#include "nucleus\emulator.h"
#include "nucleus\cpu\cell.h"

namespace cpu {
namespace backend {

SymbolInfo SymbolResolver::findSymbol(const std::string& name)
{
    // Nucleus symbols
    if (name == "nucleusGetState") {
        return SymbolInfo((uint64_t)getState, llvm::JITSymbolFlags::Exported);
    }

    // Other symbols
    if (name == "printf") {
        return SymbolInfo((uint64_t)printf, llvm::JITSymbolFlags::Exported);
    }

    nucleus.log.error(LOG_CPU, "Symbol \'%s\' not found", name.c_str());
}

SymbolInfo SymbolResolver::findSymbolInLogicalDylib(const std::string &name)
{
    nucleus.log.error(LOG_CPU, "Symbol \'%s\' not found within the logical dynamic library", name.c_str());
    return SymbolInfo(0, llvm::JITSymbolFlags::None);
}

}  // namespace backend
}  // namespace cpu
