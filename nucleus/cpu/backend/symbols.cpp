/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "symbols.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/cell.h"
#include "nucleus/cpu/frontend/ppu/ppu_tables.h"
#include "nucleus/logger/logger.h"

namespace cpu {
namespace backend {

// Nucleus-defined symbols
extern "C" {
    void* nucleusGetState() {
        return static_cast<ppu::Thread*>(nucleus.cell.getCurrentThread())->state;
    }

    void nucleusLogState(U64 address) {
        printf("> 0x%08X\n", address);
    }

    void nucleusIntermodularCall(u64 address) {
        return;
    }

    void nucleusSystemCall() {
        ppu::State& state = *static_cast<ppu::Thread*>(nucleus.cell.getCurrentThread())->state;
        nucleus.lv2.call(state);
    }
}

SymbolInfo SymbolResolver::findSymbol(const std::string& name)
{
    // Nucleus symbols
    if (name == "nucleusGetState") {
        return SymbolInfo((uint64_t)nucleusGetState, llvm::JITSymbolFlags::Exported);
    }
    if (name == "nucleusLogState") {
        return SymbolInfo((uint64_t)nucleusLogState, llvm::JITSymbolFlags::Exported);
    }
    if (name == "nucleusIntermodularCall") {
        return SymbolInfo((uint64_t)nucleusIntermodularCall, llvm::JITSymbolFlags::Exported);
    }
    if (name == "nucleusSystemCall") {
        return SymbolInfo((uint64_t)nucleusSystemCall, llvm::JITSymbolFlags::Exported);
    }

    logger.error(LOG_CPU, "Symbol \'%s\' not found", name.c_str());
    return SymbolInfo(0, llvm::JITSymbolFlags::None);
}

SymbolInfo SymbolResolver::findSymbolInLogicalDylib(const std::string &name)
{
    logger.error(LOG_CPU, "Symbol \'%s\' not found within the logical dynamic library", name.c_str());
    return SymbolInfo(0, llvm::JITSymbolFlags::None);
}

}  // namespace backend
}  // namespace cpu
