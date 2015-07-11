/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <unordered_map>
#include <vector>

struct PSFHeader {
    BE<U32> magic;
    LE<U32> version;
    LE<U32> table_keys;
    LE<U32> table_data;
    LE<U32> entries;
};

struct PSFEntry {
    enum Type : U16 {
        TEXT_RAW     = 0x0400, // String in UTF-8 format and not NULL terminated
        TEXT_NORMAL  = 0x0402, // String in UTF-8 format and NULL terminated
        INTEGER      = 0x0404, // Unsigned 32-bit integer
    };

    LE<U16> offset_key;
    LE<U16> type;
    LE<U32> size;
    LE<U32> capacity;
    LE<U32> offset_data;
};

class PSFLoader
{
    std::vector<char> psf;
    std::unordered_map<std::string, std::string> map_strings;
    std::unordered_map<std::string, U32> map_integers;

public:
    bool open(const std::string& path);
    bool save(const std::string& path);

    // Access data
    std::string get_string(const std::string& key);
    U32 get_integer(const std::string& key);
};
