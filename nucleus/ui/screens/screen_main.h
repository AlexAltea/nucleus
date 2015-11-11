/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/screen.h"

namespace ui {

class ScreenMain : public Screen {
public:
    ScreenMain(UI* parent);

    virtual void update() override;
    virtual void render() override;
};

}  // namespace ui
