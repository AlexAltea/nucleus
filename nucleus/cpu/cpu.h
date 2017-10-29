/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

// Forward declarations
class Emulator;
namespace mem { class Memory; }
namespace cpu { class Thread; }

namespace cpu {

class CPU {
    Emulator* m_emulator;

    mem::Memory* m_memory;

public:
    CPU(Emulator* emulator, mem::Memory* memory);

    /**
     * Get the emulator object to which this this CPU is belongs.
     * @return  Emulator object
     */
    Emulator* getEmulator() const;

    /**
     * Get the memory object to which this this CPU is belongs.
     * @return  Memory object
     */
    mem::Memory* getMemory() const;

    // Thread management
    /**
     * Start or resume the execution of all threads
     */
    virtual void run() = 0;

    /**
     * Pause the execution of all threads
     */
    virtual void pause() = 0;

    /**
     * Stop the execution of all threads
     */
    virtual void stop() = 0;


    static Thread* getCurrentThread();

    static void setCurrentThread(Thread* thread);
};

}  // namespace cpu
