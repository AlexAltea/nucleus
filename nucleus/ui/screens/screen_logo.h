/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/screen.h"

#include "nucleus/ui/widgets/widget_image.h"

namespace ui {

class ScreenLogo : public Screen {
    WidgetImage logo;

public:
    ScreenLogo(UI* parent);

    virtual void update() override;
    virtual void render() override;
};

}  // namespace ui
