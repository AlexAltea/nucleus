/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace core {

enum ResourceName {
#define RESOURCE(name, path) RES_##name,
#include "resource.inl"
#undef RESOURCE
};

class Resource {
public:
    size_t size;
    void* data;

    Resource(ResourceName name);
    ~Resource();
};

}  // namespace core