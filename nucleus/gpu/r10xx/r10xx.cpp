/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "r10xx.h"

namespace gpu {
namespace r10xx {

R10XX::R10XX(std::shared_ptr<gfx::IBackend> graphics) {
}

gfx::Texture* R10XX::getFrontBuffer() {
    return nullptr;
}

}  // namespace r10xx
}  // namespace gpu
