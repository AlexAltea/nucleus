/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "x86_emitter.h"

namespace cpu {
namespace backend {
namespace x86 {

X86Emitter::X86Emitter(const Settings& settings) : CodeGenerator(), settings(settings) {
}

bool X86Emitter::isExtensionAvailable(U32 queriedExtensions) const {
    return (extensions & queriedExtensions);
}

}  // namespace x86
}  // namespace backend
}  // namespace cpu
