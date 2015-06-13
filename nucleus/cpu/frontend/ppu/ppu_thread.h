/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/thread.h"
#include "nucleus/cpu/frontend/ppu/interpreter/ppu_interpreter.h"

namespace cpu {
namespace ppu {

// Translator declarations
class Interpreter;

class Thread : public CellThread
{
    u32 m_stackAddr;
    u32 m_stackPointer;

    // Translators
    Interpreter* interpreter = nullptr;

public:
    State* state;

    Thread(u32 entry);
    ~Thread();

    virtual void start() override;
    virtual void task() override;

    // Control the thread once it is started
    virtual void run() override;
    virtual void pause() override;
    virtual void stop() override;
};

}  // namespace ppu
}  // namespace cpu
