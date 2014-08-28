/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "loader/self.h"
#include "emulator.h"

#include <iostream>

bool Emulator::load(const std::string& filepath)
{
    // Initialize everything
    memory.init();

    // Load title / title_id / etc. ?

    // Load ELF/SELF
    SELFLoader self;
    if (!self.open(filepath)) {
        std::cerr << "Invalid file given." << std::endl;
        return false;
    }
    self.load();

    // Prepare Thread

    return true;
}

void Emulator::run()
{
}

void Emulator::pause()
{
}

void Emulator::stop()
{
}
