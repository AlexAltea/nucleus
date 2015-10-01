/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace cpu {

/**
 * Function placeholders in JIT-translated modules call this function to initialize
 * the translation and compilation process. This function will never return.
 * Instead, it will directly jump to the recompiled function's entry point.
 * @param[in]  guestFunc  Host address to the guest function object
 * @param[in]  guestAddr  Guest address where the function to be translated begins
 */
void nucleusTranslate(void* guestFunc, U64 guestAddr);

/**
 * Guest code may contain jumps/calls to addresses hold in register which might not be
 * guessed at translation time. Such jumps/calls are handled through this proxy function.
 * Executing AOT-translated code, it is assumed the target function exists.
 * Executing JIT-translated code, if the function doesn't exist, it will be generated.
 * Note that switch/cases via jump tables are handled through this function as well.
 * @param[in]  guestAddr  Guest address where the function to be executed begins
 */
void nucleusCall(U64 guestAddr);

/**
 * This is just an utility function that can be placed between guest instructions to
 * obtain information in real-time about the thread state.
 * @param[in]  guestAddr  Guest address where the instruction to be executed is
 */
void nucleusLog(U64 guestAddr);

}  // namespace cpu
