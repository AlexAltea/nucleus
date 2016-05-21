/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "spu_translator.h"
#include "nucleus/core/config.h"
#include "nucleus/assert.h"

namespace cpu {
namespace frontend {
namespace spu {

Translator::Translator(CPU* parent, spu::Function* function) : parent(parent), IRecompiler<U32>(function) {
}

hir::Value* Translator::getGPR(int index) {
    // TODO
    return nullptr;
}

void Translator::setGPR(int index, hir::Value* value) {
    // TODO
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
