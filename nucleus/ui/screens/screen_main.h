/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/screen.h"

namespace ui {

class ScreenMain : public Screen {
public:
    ScreenMain(UI* manager);

    virtual void update() override;
};

}  // namespace ui
