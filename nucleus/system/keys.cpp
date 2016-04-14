/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "keys.h"
#include "nucleus/format.h"
#include "nucleus/filesystem/filesystem_app.h"
#include "nucleus/logger/logger.h"

#include "externals/rapidxml/rapidxml.hpp"

#include <unordered_map>
#include <vector>

#define KEYVAULT_FILE "keys.xml"

static const std::unordered_map<U32, const char*> keyType = {
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
    std::vector<char> buffer;
    rapidxml::xml_document<> doc;

    KeyvaultHandler() {
        auto file = fs::AppFileSystem::openFile(fs::APP_LOCATION_ROAMING, KEYVAULT_FILE, fs::Read);
        auto size = file->attributes().size;
        buffer.resize(size + 1);
        file->read(buffer.data(), size);
        doc.parse<0>(buffer.data());
    }
} keyvault;

const SelfKey getSelfKey(U32 type, U64 version, U16 revision) {
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
        const U64 versionValue = hexToNumber(node->first_attribute("version")->value());
        const U16 revisionValue = hexToNumber(node->first_attribute("revision")->value());
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

    logger.error(LOG_LOADER, "Key not found in keyvault!");
    return SelfKey{};
}
