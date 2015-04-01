/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/screen.h"

class ScreenMain : public Screen {
public:
    ScreenMain();

    virtual void update() override;
    virtual void render() override;
};
