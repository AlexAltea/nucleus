/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "information.h"

#include "nucleus/system/scei/cellos/cellos_info.h"

namespace sys {

InfoPlatform getInformation(core::Platform platform) {
    if (platform == core::PLATFORM_PS3) {
        return scei::cellos::getInformation();
    }
}

}  // namespace sys
