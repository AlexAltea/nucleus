/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/fence.h"
#include "nucleus/graphics/backend/opengl/opengl.h"

#include <condition_variable>
#include <mutex>

namespace gfx {
namespace opengl {

class OpenGLFence : public Fence {
    std::condition_variable cv;
    std::mutex mutex;

    bool signaled;

public:
    // Clear the signal status
    void clear();

    // Trigger the signal status
    void signal();

    virtual void wait() override;
    virtual void wait(Clock::duration timeout) override;
};

}  // namespace opengl
}  // namespace gfx
