/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "language.h"
#include "nucleus/emulator.h"
#include "nucleus/resource.h"
#include "externals/rapidxml/rapidxml.hpp"

#include <cstring>

#ifdef NUCLEUS_PLATFORM_WINDOWS
#include <Windows.h>
#endif

struct LanguageResource {
    ConfigLanguage id;
    int winResource;
};

static const LanguageResource langResources[] = {
    { LANGUAGE_DEFAULT, IDR_LANGUAGE_EN_US },
    { LANGUAGE_DE_DE,   IDR_LANGUAGE_DE_DE },
    { LANGUAGE_EN_US,   IDR_LANGUAGE_EN_US },
    { LANGUAGE_ES_ES,   IDR_LANGUAGE_ES_ES }
};

void Language::open(ConfigLanguage language)
{
    if (buffer) {
        close();
    }

    // Get language file/resource or use the default one
    LanguageResource lang = { LANGUAGE_DEFAULT, IDR_LANGUAGE_EN_US };

    for (const auto& langResource : langResources) {
        if (langResource.id == language) {
            lang = langResource;
            break;
        }
    }

    // Open the file/resource
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    HRSRC hRes = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(lang.winResource), "LANGUAGE");
    HGLOBAL hGlob = LoadResource(GetModuleHandle(NULL), hRes);
    DWORD langSize = SizeofResource(GetModuleHandle(NULL), hRes);
    LPVOID langData = LockResource(hGlob);
#else
    nucleus.log.error(LOG_UI, "Languages not supported in your platform");
    unsigned long langSize = 0;
    char* langData = nullptr;
#endif

    // Copy language resource to buffer and parse XML (this will modify the buffer)
    buffer = new char[langSize+1]();
    memcpy(buffer, langData, langSize);
    doc.parse<0>(buffer);
}

char* Language::translate(const std::string& msgid)
{
    auto nodeLang = doc.first_node();
    auto nodeEntries = nodeLang->first_node("entries");
    for (auto node = nodeEntries->first_node(); node; node = node->next_sibling("msg")) {
        char* id = node->first_attribute("id")->value();
        char* value = node->first_attribute("value")->value();
        if (msgid == id) {
            return value;
        }
    }
    return "UNKNOWN";
}

void Language::close()
{
    // Delete the buffer holding the parsed XML language file
    delete[] buffer;
}
