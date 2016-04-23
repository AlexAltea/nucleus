/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/system/information.h"

namespace sys {
namespace scei {
namespace cellos {

/**
 * Retrieves information about CellOS and the apps installed on it
 * @return  Platform information
 */
InfoPlatform getInformation();

}  // namespace cellos
}  // namespace scei
}  // namespace sys
