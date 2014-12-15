/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

class RSXRenderer {

public:
    virtual void Begin(u32 mode)=0;
    virtual void DrawArrays(u32 count, const be_t<u32>* args)=0;
    virtual void End()=0;
};
