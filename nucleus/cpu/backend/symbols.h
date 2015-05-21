/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "nucleus/common.h"

#include "llvm/ExecutionEngine/RuntimeDyld.h"

namespace cpu {
namespace backend {

// Types
using SymbolInfo = llvm::RuntimeDyld::SymbolInfo;

// Nucleus-defined symbols
extern "C" {
    void* nucleusGetState();
    void nucleusLogState(u64 address);
    void nucleusIntermodularCall(u64 address);
    void nucleusSystemCall();
}

class SymbolResolver : public llvm::RuntimeDyld::SymbolResolver {
public:
    virtual SymbolInfo findSymbol(const std::string &name) override;
    virtual SymbolInfo findSymbolInLogicalDylib(const std::string &name) override;
};

}  // namespace backend
}  // namespace cpu
