/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include "nucleus/filesystem/filesystem_host.h"

namespace sys {
namespace scei {
namespace orbis {

/**
 * Checks whether the given file can be processed by OrbisOS
 * @param[in]  file  File to analyze
 * @return           True on valid file
 */
bool isValid(fs::File* file);

}  // namespace orbis
}  // namespace scei
}  // namespace sys
