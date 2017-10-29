/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/screen.h"
#include "nucleus/ui/widgets/list.h"

// Forward declarations
class Emulator;

namespace ui {

class ScreenEmulator : public Screen {
private:
    Emulator* m_emulator;

    WidgetImage* app;

public:
    ScreenEmulator(UI* manager);

    virtual void update() override;
};

}  // namespace ui
