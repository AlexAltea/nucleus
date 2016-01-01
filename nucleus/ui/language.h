/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/core/config.h"
#include "externals/rapidxml/rapidxml.hpp"

#include <string>
#include <vector>

namespace ui {

class Language {
    // Holds the parsed XML file
    std::vector<char> buffer;
    rapidxml::xml_document<> doc;

public:
    // Open and process language file
    void open(ConfigLanguage language);

    // Translate a message
    const char* translate(const std::string& msgid);
};

}  // namespace ui
