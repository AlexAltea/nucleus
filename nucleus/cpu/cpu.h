/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

namespace cpu {

//  Forward declarations
class Thread;

class CPU {
public:
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
