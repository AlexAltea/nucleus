/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/screen.h"

#include "nucleus/ui/widgets/widget_texture.h"

class ScreenEmulator : public Screen
{
    WidgetTexture rsxDisplay;

public:
    ScreenEmulator();

    virtual void update() override;
    virtual void render() override;
};
