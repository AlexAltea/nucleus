/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "compiler.h"
#include "nucleus/logger/logger.h"

namespace cpu {
namespace backend {

using namespace cpu::hir;

void Compiler::addPass(std::unique_ptr<Pass> pass) {
    passes.push_back(std::move(pass));
}

bool Compiler::optimize(Function* function) {
    for (auto& pass : passes) {
        if (!pass->run(function)) {
            logger.error(LOG_CPU, "Could not run pass: %s", pass->name());
            return false;
        }
    }
    return true;
}

}  // namespace backend
}  // namespace cpu
