/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "cpu.h"

namespace cpu {

thread_local Thread* gCurrentThread = nullptr;

CPU::CPU(Emulator* emulator, mem::Memory* memory)
    : m_emulator(emulator), m_memory(memory)
{
}

Emulator* CPU::getEmulator() const {
    return m_emulator;
}

mem::Memory* CPU::getMemory() const {
    return m_memory;
}

Thread* CPU::getCurrentThread() {
    return gCurrentThread;
}

void CPU::setCurrentThread(Thread* thread) {
    gCurrentThread = thread;
}

}  // namespace cpu
