/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "keys.h"
#include "nucleus/emulator.h"
#include "nucleus/format.h"
#include "nucleus/filesystem/filesystem.h"
#include "externals/rapidxml/rapidxml.hpp"

#include <map>

#if defined(NUCLEUS_PLATFORM_WINDOWS)
#define fseeko64 _fseeki64
#define ftello64 _ftelli64
#endif

#define KEYVAULT_FILE "keys.xml"

static const std::map<u32, const char*> keyType = {
    { KEY_LV0,   "LV0"   },
    { KEY_LV1,   "LV1"   },
    { KEY_LV2,   "LV2"   },
    { KEY_APP,   "APP"   },
    { KEY_ISO,   "ISO"   },
    { KEY_LDR,   "LDR"   },
    { KEY_UNK7,  "UNK7"  },
    { KEY_NPDRM, "NPDRM" }
};

static struct KeyvaultHandler {
    // Holds the parsed XML file
    char* buffer;
    rapidxml::xml_document<> doc;

    KeyvaultHandler() {
        // Access the Keyvault file and get its size
        std::FILE* file = fopen(KEYVAULT_FILE, "rb");
        fseeko64(file, 0, SEEK_END);
        const u64 kvSize = ftello64(file);

        // Copy and parse its contents
        buffer = new char[kvSize+1]();
        fseeko64(file, 0, SeekSet);
        fread(buffer, 1, kvSize, file);
        fclose(file);
        doc.parse<0>(buffer);
    }
    ~KeyvaultHandler() {
        delete[] buffer;
    }
} keyvault;

const SelfKey getSelfKey(u32 type, u64 version, u16 revision)
{
    // Locate the SELF keys node
    auto nodeKeyvault = keyvault.doc.first_node();
    auto nodeSelf = nodeKeyvault->first_node("self");
    std::string requestedType = keyType.at(type);

    // Search for the appropriate key
    for (auto node = nodeSelf->first_node(); node; node = node->next_sibling("key")) {
        // Check type
        const char* typeStr = node->first_attribute("type")->value();
        if (requestedType != typeStr) {
            continue;
        }

        // Check version/revision IDs
        const u64 versionValue = hexToNumber(node->first_attribute("version")->value());
        const u16 revisionValue = hexToNumber(node->first_attribute("revision")->value());
        if (versionValue != version && (type == KEY_LV1 || type == KEY_LV2 || type == KEY_UNK7 || type == KEY_ISO)) {
            continue;
        }
        if (revisionValue != revision && (type == KEY_APP || type == KEY_ISO || type == KEY_NPDRM)) {
            continue;
        }

        // Parse and return key
        SelfKey key{};
        key.type = type;
        key.version = versionValue;
        key.revision = revisionValue;
        hexToBytes(node->first_node()->next_sibling("erk")->value(), (char*)key.erk);
        hexToBytes(node->first_node()->next_sibling("riv")->value(), (char*)key.riv);
        hexToBytes(node->first_node()->next_sibling("pub")->value(), (char*)key.pub);
        hexToBytes(node->first_node()->next_sibling("priv")->value(), (char*)key.priv);
        return key;
    }

    nucleus.log.error(LOG_LOADER, "Key not found in keyvault!");
    return SelfKey{};
}
