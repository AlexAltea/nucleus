/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/filesystem/filesystem_virtual.h"

namespace sys {

class System {
public:
    fs::VirtualFileSystem vfs;
};

}  // namespace sys
