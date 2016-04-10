/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/gpu/gpu.h"

#include <memory>

namespace gpu {
namespace r10xx {

class R10XX : public GPU {
public:
    R10XX(std::shared_ptr<gfx::IBackend> graphics);

    virtual gfx::Texture* getFrontBuffer() override;
};

}  // namespace r10xx
}  // namespace gpu
