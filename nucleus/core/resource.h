/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <cstdio>
#include <vector>

namespace core {

enum ResourceName {
#define RESOURCE(name, path) RES_##name,
#include "resource.inl"
#undef RESOURCE
};

class Resource {
#if defined(NUCLEUS_PLATFORM_UWP)
    std::FILE* file;
    std::vector<char> buffer;
#endif

public:
    size_t size;
    void* data;

    Resource(ResourceName name);
    ~Resource();
};

}  // namespace core