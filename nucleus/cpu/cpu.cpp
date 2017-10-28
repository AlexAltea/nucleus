/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "cpu.h"
#include "thread.h"

thread_local Thread* gCurrentThread = nullptr;

namespace cpu {

Thread* CPU::getCurrentThread() {
    return gCurrentThread;
}

void CPU::setCurrentThread(Thread* thread) {
    gCurrentThread = thread;
}


}  // namespace cpu
