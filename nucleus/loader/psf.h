/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <unordered_map>
#include <vector>

struct PSFHeader {
    be_t<u32> magic;
    u32 version;
    u32 table_keys;
    u32 table_data;
    u32 entries;
};

struct PSFEntry {
    enum Type : u16 {
        TEXT_RAW     = 0x0400, // String in UTF-8 format and not NULL terminated
        TEXT_NORMAL  = 0x0402, // String in UTF-8 format and NULL terminated
        INTEGER      = 0x0404, // Unsigned 32-bit integer
    };

    u16 offset_key;
    u16 type;
    u32 size;
    u32 capacity;
    u32 offset_data;
};

class PSFLoader
{
    std::vector<char> psf;
    std::unordered_map<std::string, std::string> map_strings;
    std::unordered_map<std::string, u32> map_integers;

public:
    bool open(const std::string& path);
    bool save(const std::string& path);

    // Access data
    std::string get_string(const std::string& key);
    u32 get_integer(const std::string& key);
};
