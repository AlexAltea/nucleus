/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <string>

namespace fs {

enum DirectoryEntryType {
    ENTRY_TYPE_FOLDER,
    ENTRY_TYPE_FILE,
};

class DirectoryEntry {
public:
    std::string name;
    DirectoryEntryType type;
};

}  // namespace fs
