/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "thread.h"

namespace cpu {

Thread::Thread(CPU* parent) : parent(parent) {
}

void Thread::join() {
    m_thread.join();
}

}  // namespace cpu
