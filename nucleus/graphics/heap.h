/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace gfx {

// Forward declarations
class Texture;
class VertexBuffer;

enum HeapType {
    HEAP_TYPE_CT,       // Heap for color target descriptors
    HEAP_TYPE_DST,      // Heap for depth-stencil target descriptors
    HEAP_TYPE_RESOURCE,
    HEAP_TYPE_SAMPLER,
};

struct HeapDesc {
    HeapType type;  // Type of descriptors
    Size size;      // Number of descriptors
};

class Heap {
public:
    /**
     * Reset the heap
     */
    virtual void reset() = 0;

    /**
     * Allocate texture
     */
    virtual void pushTexture(Texture* texture) = 0;

    /**
     * Allocate vertex buffer
     */
    virtual void pushVertexBuffer(VertexBuffer* buffer) = 0;
};

}  // namespace gfx
