/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/resource.h"

namespace gfx {

// Forward declarations
class Resource;

enum ResourceState {
    RESOURCE_STATE_PRESENT = 0,
    RESOURCE_STATE_COLOR_TARGET  = (1 << 0),
    RESOURCE_STATE_GENERIC_READ  = (1 << 1),
};

struct ResourceBarrier {
    struct ResourceTransition {
        Resource* resource;
        ResourceState before;
        ResourceState after;
    } transition;
};

class Resource {
public:
    /**
     * Map this resource into the user address space
     * @return  Address where this resource was mapped into
     */
    virtual void* map() = 0;

    /**
     * Release the mapping of this resource from the user address space
     * @return  True on success
     */
    virtual bool unmap() = 0;
};

}  // namespace gfx
