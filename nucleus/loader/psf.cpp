/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "psf.h"
#include "nucleus/emulator.h"
#include "nucleus/format.h"
#include "nucleus/filesystem/filesystem.h"
#include "nucleus/loader/loader.h"

bool PSFLoader::open(const std::string& path)
{
    // Load file
    FileSystem* fs = getFilesystem(path.c_str());
    File* file = fs->openFile(path, Read);

    if (!fs->isOpen(file)) {
        return false;
    }

    const u64 psfSize = fs->getFileSize(path);
    psf.resize(psfSize);
    fs->seekFile(file, 0, SeekSet);
    fs->readFile(file, &psf[0], psfSize);
    fs->closeFile(file);

    if (detectFiletype(path) != FILETYPE_PSF) {
        return false;
    }

    // Parse file contents
    const auto& header = (PSFHeader&)psf[0];
    for (u32 i = 0; i < header.entries; i++) {
        const u32 offset = sizeof(PSFHeader) + i * sizeof(PSFEntry);
        const auto& entry = (PSFEntry&)psf[offset];

        std::string key = &psf[header.table_keys + entry.offset_key];
        if (entry.type == PSFEntry::Type::TEXT_RAW ||
            entry.type == PSFEntry::Type::TEXT_NORMAL) {
            map_strings[key] = &psf[header.table_data + entry.offset_data];
        }
        if (entry.type == PSFEntry::Type::INTEGER) {
            map_integers[key] = (u32&)psf[header.table_data + entry.offset_data];
        }
    }
    return true;
}

bool PSFLoader::save(const std::string& path)
{
    FileSystem* fs = getFilesystem(path.c_str());
    File* file = fs->openFile(path, Write);

    if (!fs->isOpen(file)) {
        return false;
    }

    PSFHeader header;
    header.magic = 0x00505346;
    header.entries = map_strings.size() + map_integers.size();
    header.table_keys = sizeof(PSFHeader) + header.entries * sizeof(PSFEntry);
    header.table_data = 0; // TODO
    header.version = 0x00000101;

    u32 offsetKey = 0;
    u32 offsetData = 0;
    fs->seekFile(file, sizeof(PSFHeader), SeekSet);
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

    fs->seekFile(file, 0, SeekSet);
    fs->writeFile(file, &header, sizeof(PSFHeader));
    fs->closeFile(file);
}

std::string PSFLoader::get_string(const std::string& key)
{
    if (map_strings.find(key) != map_strings.end()) {
        return map_strings.at(key);
    }
    if (map_integers.find(key) != map_integers.end()) {
        return format("%u", map_integers.at(key));
    }
    nucleus.log.error(LOG_LOADER, "PSFLoader: Entry not found (%s)", key.c_str());
    return "";
}
