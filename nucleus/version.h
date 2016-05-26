/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/macro.h"

#define NUCLEUS_VERSION_MAJOR  0
#define NUCLEUS_VERSION_MINOR  1
#define NUCLEUS_VERSION_BUILD  1

#define NUCLEUS_VERSION "v" \
    __NUCLEUS_STRINGIFY(NUCLEUS_VERSION_MAJOR) "." \
    __NUCLEUS_STRINGIFY(NUCLEUS_VERSION_MINOR) "." \
    __NUCLEUS_STRINGIFY(NUCLEUS_VERSION_BUILD)
