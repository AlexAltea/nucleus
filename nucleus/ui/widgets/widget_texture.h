/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/widget.h"

namespace ui {

class WidgetTexture : public Widget {
public:
    // Initialize the texture
    void init();

    // Render the image
    virtual void render() override;
};

}  // namespace ui
