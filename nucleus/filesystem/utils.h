/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/filesystem/path.h"

namespace fs {

// Get the path to the folder that contains the emulated ELF binary
Path getProcessPath(const Path& elfPath);

}  // namespace fs
