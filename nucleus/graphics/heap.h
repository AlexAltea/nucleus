/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace gfx {

using HeapSize = U32;

enum HeapType {
    HEAP_TYPE_CT,   // Heap for color target descriptors
    HEAP_TYPE_DST,  // Heap for depth-stencil target descriptors
};

struct HeapDesc {
    HeapType type;  // Type of descriptors
    HeapSize size;  // Number of descriptors
};

class IHeap {
public:
};

}  // namespace gfx
