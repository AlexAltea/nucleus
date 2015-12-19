/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/pipeline.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"

namespace gfx {

class Direct3D12Pipeline : public Pipeline {
public:
    ID3D12PipelineState* state;
};

}  // namespace gfx
