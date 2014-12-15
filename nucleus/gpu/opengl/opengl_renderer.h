/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/gpu/renderer.h"

class RSXRendererOpenGL : public RSXRenderer {

public:
    virtual void Begin(u32 mode) override;
    virtual void End() override;
};
