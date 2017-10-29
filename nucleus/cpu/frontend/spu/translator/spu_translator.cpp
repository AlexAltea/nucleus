/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "spu_translator.h"
#include "nucleus/cpu/frontend/spu/spu_state.h"
#include "nucleus/cpu/thread.h"
#include "nucleus/core/config.h"
#include "nucleus/memory/guest_virtual/guest_virtual_memory.h"
#include "nucleus/assert.h"

namespace cpu {
namespace frontend {
namespace spu {

using namespace cpu::hir;

Translator::Translator(CPU* parent, spu::Function* function) : parent(parent), IRecompiler(function) {
}

hir::Value* Translator::getGPR(int index) {
    const U32 offset = offsetof(SPUState, r[index]);

    // TODO: Use volatility information?
    return builder.createCtxLoad(offset, TYPE_V128);
}

hir::Value* Translator::getSPR(int index) {
    const U32 offset = offsetof(SPUState, s[index]);

    // TODO: Use volatility information?
    return builder.createCtxLoad(offset, TYPE_V128);
}

void Translator::setGPR(int index, hir::Value* value) {
    const U32 offset = offsetof(SPUState, r[index]);

    // TODO: Use volatility information?
    builder.createCtxStore(offset, value);
}

void Translator::setSPR(int index, hir::Value* value) {
    const U32 offset = offsetof(SPUState, s[index]);

    // TODO: Use volatility information?
    builder.createCtxStore(offset, value);
}

/**
 * Memory access
 */
Value* Translator::readMemory(hir::Value* addr, hir::Type type) {
    assert_true(type == TYPE_V128);

    // Get host address
    auto* memory = dynamic_cast<mem::GuestVirtualMemory*>(CPU::getCurrentThread()->getMemory());
    void* baseAddress = memory->getBaseAddr();
    addr = builder.createZExt(addr, TYPE_PTR);
    addr = builder.createAdd(addr, builder.getConstantPointer(baseAddress));
    return builder.createLoad(addr, type, ENDIAN_BIG);
}

void Translator::writeMemory(Value* addr, Value* value) {
    assert_true(value->type == TYPE_V128);

    // Get host address
    auto* memory = dynamic_cast<mem::GuestVirtualMemory*>(CPU::getCurrentThread()->getMemory());
    void* baseAddress = memory->getBaseAddr();
    addr = builder.createZExt(addr, TYPE_PTR);
    addr = builder.createAdd(addr, builder.getConstantPointer(baseAddress));
    builder.createStore(addr, value, ENDIAN_BIG);
}

void Translator::createProlog() {
}

void Translator::createEpilog() {
    assert_true(epilog == nullptr, "The frontend epilog block was already declared");

    epilog = new hir::Block(function->hirFunction);
    builder.setInsertPoint(epilog);

    if (config.spuTranslator != CPU_TRANSLATOR_MODULE) {
        builder.createRet();
    }
}


}  // namespace spu
}  // namespace frontend
}  // namespace cpu
