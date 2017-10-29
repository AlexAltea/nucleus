/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "system.h"

namespace sys {

System::System(Emulator* emulator) : m_emulator(emulator)
{
}

Emulator* System::getEmulator() const
{
    return m_emulator;
}

}  // namespace sys
