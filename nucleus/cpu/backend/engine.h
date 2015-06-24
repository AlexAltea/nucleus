/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/cell.h"
#include "nucleus/cpu/hir/module.h"
#include "nucleus/cpu/frontend/frontend_segment.h"
#include "nucleus/cpu/backend/symbols.h"

#include "llvm/ADT/Triple.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm/Support/Host.h"

namespace cpu {
namespace backend {

template <typename TAddr>
void Generate(frontend::ISegment<TAddr>* segment) {
    // NOTE: Avoid generating COFF objects on Windows which are not supported by MCJIT
    llvm::Triple triple(llvm::sys::getProcessTriple());
    if (triple.getOS() == llvm::Triple::OSType::Win32) {
        triple.setObjectFormat(llvm::Triple::ObjectFormatType::ELF);
    }
    segment->module.module->setTargetTriple(triple.str());
    
    // Create execution engine
    llvm::EngineBuilder engineBuilder(std::unique_ptr<llvm::Module>(segment->module.module));
    engineBuilder.setEngineKind(llvm::EngineKind::JIT);
    engineBuilder.setOptLevel(llvm::CodeGenOpt::Default);
    engineBuilder.setSymbolResolver(std::make_unique<SymbolResolver>());

    segment->ee = engineBuilder.create();
    segment->ee->addModule(std::unique_ptr<llvm::Module>(nucleus.cell.module));
    segment->ee->finalizeObject();
}

}  // namespace backend
}  // namespace cpu
