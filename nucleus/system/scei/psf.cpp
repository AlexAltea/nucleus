/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "psf.h"
#include "nucleus/system/system.h"
#include "nucleus/assert.h"
#include "nucleus/format.h"
#include "nucleus/filesystem/filesystem_virtual.h"
#include "nucleus/system/loader.h"
#include "nucleus/logger/logger.h"

namespace sys {
namespace scei {

void PSFLoader::open(fs::File* file) {
    assert_true(detectFiletype(file) == FILETYPE_PSF);

    const U64 psfSize = file->attributes().size;
    psf.resize(psfSize);
    file->seek(0, fs::SeekSet);
    file->read(&psf[0], psfSize);

    // Parse file contents
    const auto& header = (PSFHeader&)psf[0];
    for (U32 i = 0; i < header.entries; i++) {
        const U32 offset = sizeof(PSFHeader) + i * sizeof(PSFEntry);
        const auto& entry = (PSFEntry&)psf[offset];

        std::string key = (char*)&psf[header.table_keys + entry.offset_key];
        if (entry.type == PSFEntry::Type::TEXT_RAW ||
            entry.type == PSFEntry::Type::TEXT_NORMAL) {
            map_strings[key] = (char*)&psf[header.table_data + entry.offset_data];
        }
        if (entry.type == PSFEntry::Type::INTEGER) {
            map_integers[key] = (U32&)psf[header.table_data + entry.offset_data];
        }
    }
}

void PSFLoader::save(fs::File* file) {
    PSFHeader header;
    header.magic = 0x00505346;
    header.entries = map_strings.size() + map_integers.size();
    header.table_keys = sizeof(PSFHeader) + header.entries * sizeof(PSFEntry);
    header.table_data = 0; // TODO
    header.version = 0x00000101;

    U32 offsetKey = 0;
    U32 offsetData = 0;
    file->seek(sizeof(PSFHeader), fs::SeekSet);
    for (auto& item : map_strings) {
        auto& key = item.first;
        auto& string = item.second;

        PSFEntry entry;
        entry.type = PSFEntry::Type::TEXT_NORMAL;
        entry.size = string.size() + 1;
        entry.capacity = (string.size() + 4) & ~0x3;

        offsetData += entry.capacity;
        offsetKey += key.size() + 1;
    }
    for (auto& item : map_integers) {
        auto& key = item.first;
        auto& integer = item.second;

        PSFEntry entry;
        entry.type = PSFEntry::Type::INTEGER;
        entry.size = 4;
        entry.capacity = 4;

        offsetData += entry.capacity;
        offsetKey += key.size() + 1;
    }

    file->seek(0, fs::SeekSet);
    file->write(&header, sizeof(PSFHeader));
}

std::string PSFLoader::get_string(const std::string& key) {
    if (map_strings.find(key) != map_strings.end()) {
        return map_strings.at(key);
    }
    if (map_integers.find(key) != map_integers.end()) {
        return format("%u", map_integers.at(key));
    }
    logger.error(LOG_LOADER, "PSFLoader: Entry not found (%s)", key.c_str());
    return "";
}

}  // namespace scei
}  // namespace sys
