/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

class RSXRenderer {

public:
    virtual void AlphaFunc(u32 func, f32 ref)=0;
    virtual void Begin(u32 mode)=0;
    virtual void DrawArrays(u32 mode, u32 first, u32 count)=0;
    virtual void Enable(u32 prop, u32 enabled)=0;
    virtual void End()=0;
    virtual void Flip()=0;
};
