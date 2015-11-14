/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <chrono>

namespace gfx {

enum FenceFlags : U32 {
    FENCE_FLAG_NONE = 0,
};

struct FenceDesc {
    U32 flags;
};

class Fence {
protected:
    using Clock = std::chrono::high_resolution_clock;

public:
    /**
     * Wait for the GPU to reach this fence
     */
    virtual void wait() = 0;

    /**
     * Wait for the GPU to reach this fence specifying a timeout
     * @param[in]  timeout  Wait timeout
     */
    virtual void wait(Clock::duration timeout) = 0;
};

}  // namespace gfx
