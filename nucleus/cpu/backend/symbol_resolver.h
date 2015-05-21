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

class SymbolResolver : public llvm::RuntimeDyld::SymbolResolver {
public:
    virtual SymbolInfo findSymbol(const std::string &name) override;
    virtual SymbolInfo findSymbolInLogicalDylib(const std::string &name) override;
};

}  // namespace backend
}  // namespace cpu
