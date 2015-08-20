/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "nucleus/common.h"

namespace cpu {
namespace backend {

// Types
using SymbolInfo = llvm::RuntimeDyld::SymbolInfo;

// Nucleus-defined symbols
extern "C" {
    void* nucleusGetState();
    void nucleusLogState(U64 address);
    void nucleusDebugState(U64 address);
    void nucleusIntermodularCall(U64 address);
    void nucleusSystemCall();
}

class SymbolResolver : public llvm::RuntimeDyld::SymbolResolver {
public:
    virtual SymbolInfo findSymbol(const std::string &name) override;
    virtual SymbolInfo findSymbolInLogicalDylib(const std::string &name) override;
};

}  // namespace backend
}  // namespace cpu
