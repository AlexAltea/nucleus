/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "rsx.h"

void RSX::init() {
    m_thread_pfifo = new std::thread([&](){
        task();
    });
}

void RSX::task() {
}
