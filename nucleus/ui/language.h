/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/core/config.h"
#include "externals/rapidxml/rapidxml.hpp"

#include <string>

class Language {
    // Holds the parsed XML file
    char* buffer;
    rapidxml::xml_document<> doc;

public:
    // Open and process language file
    void open(ConfigLanguage language);

    // Translate a message
    char* translate(const std::string& msgid);

    // Close language file and clear cache
    void close();
};
