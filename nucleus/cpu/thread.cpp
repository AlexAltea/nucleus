/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "thread.h"
#include "cpu.h"

namespace cpu {

Thread::Thread(CPU* parent) : parent(parent) {
}

Emulator* Thread::getEmulator() const {
    return parent->getEmulator();
}

mem::Memory* Thread::getMemory() const {
    return parent->getMemory();
}

void Thread::join() {
    m_thread.join();
}

}  // namespace cpu
