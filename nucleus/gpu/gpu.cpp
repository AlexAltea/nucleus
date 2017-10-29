/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "gpu.h"

namespace gpu {

GPU::GPU(Emulator* emulator) : m_emulator(emulator)
{
}

Emulator* GPU::getEmulator() const {
    return m_emulator;
}

}  // namespace gpu
