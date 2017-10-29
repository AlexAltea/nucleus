/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/filesystem/filesystem_virtual.h"

// Forward declarations
class Emulator;

namespace sys {

class System {
    Emulator* m_emulator;

public:
    fs::VirtualFileSystem vfs;

    System(Emulator* emulator);

    /**
     * Get the emulator object to which this HLE-OS belongs.
     * @return  Emulator object
     */
    Emulator* getEmulator() const;

    /**
     * Start new process from the specified source.
     * @param[in]  path  Executable path
     * @return  True on success
     */
    virtual bool start(const std::string& path) = 0;
};

}  // namespace sys
