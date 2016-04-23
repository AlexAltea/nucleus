/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <map>
#include <string>
#include <vector>

namespace sys {

/**
 * Information about an application
 */
struct InfoApp {
    std::string id;
    std::string name;
    std::string path;
    std::string version;
    std::string platform;
    std::string image;
    std::vector<std::string> genres;
    std::map<std::string, std::string> properties;
    U64 size;
};

/**
 * Information about a platform
 */
struct InfoPlatform {
    std::string name;
    std::string version;
    std::vector<InfoApp> apps;
    std::map<std::string, std::string> properties;
    U64 size;
};

InfoPlatform getInformation(core::Platform platform);

}  // namespace sys
