/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "language.h"
#include "nucleus/core/resource.h"
#include "nucleus/logger/logger.h"
#include "externals/rapidxml/rapidxml.hpp"

#include <cstring>
#include <unordered_map>

namespace ui {

static const std::unordered_map<ConfigLanguage, core::ResourceName> langResources = {
    { LANGUAGE_DEFAULT, core::RES_LANGUAGE_EN_US },
    { LANGUAGE_DE_DE,   core::RES_LANGUAGE_DE_DE },
    { LANGUAGE_EN_US,   core::RES_LANGUAGE_EN_US },
    { LANGUAGE_ES_ES,   core::RES_LANGUAGE_ES_ES }
};

void Language::open(ConfigLanguage language) {
    core::ResourceName resName = langResources.at(language);
    core::Resource res(resName);

    // Copy language resource to buffer and parse XML (this will modify the buffer)
    buffer.resize(res.size + 1);
    memcpy(buffer.data(), res.data, res.size);
    doc.parse<0>(buffer.data());
}

const char* Language::translate(const std::string& msgid) {
    auto nodeLang = doc.first_node();
    auto nodeEntries = nodeLang->first_node("entries");
    for (auto node = nodeEntries->first_node(); node; node = node->next_sibling("msg")) {
        const char* id = node->first_attribute("id")->value();
        const char* value = node->first_attribute("value")->value();
        if (msgid == id) {
            return value;
        }
    }
    return "UNKNOWN";
}

}  // namespace ui
