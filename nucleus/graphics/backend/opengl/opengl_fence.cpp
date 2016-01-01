/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_fence.h"

namespace gfx {
namespace opengl {

void OpenGLFence::clear() {
    signaled = false;
}

void OpenGLFence::signal() {
    signaled = true;
    cv.notify_all();
}

void OpenGLFence::wait() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [&]{ return signaled; });
}

void OpenGLFence::wait(Clock::duration timeout) {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait_for(lock, timeout, [&]{ return signaled; });
}

}  // namespace opengl
}  // namespace gfx
