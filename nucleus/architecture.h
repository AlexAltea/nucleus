/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#if defined(__i386) || defined(__i386__) || defined(_M_IX86) || defined(_X86_)
#define NUCLEUS_ARCH_X86_32BITS
#endif
#if defined(__x86_64__) || defined(__amd64__) || defined(_M_X64) || defined(_M_AMD64)
#define NUCLEUS_ARCH_X86_64BITS
#endif
#if defined(NUCLEUS_ARCH_X86_32BITS) || defined(NUCLEUS_ARCH_X86_64BITS)
#define NUCLEUS_ARCH_X86
#endif
#if defined(__arm__) || defined(_M_ARM)
#define NUCLEUS_ARCH_ARM
#endif

namespace core {

enum Architecture {
    ARCHITECTURE_X86,
    ARCHITECTURE_ARM,
    ARCHITECTURE_PPC,
};

}  // namespace core
