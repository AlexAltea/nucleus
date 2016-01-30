/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/ui/widget.h"

#include <string>

namespace ui {

class WidgetImage : public Widget {
    // Image
    unsigned char* imBuffer;
    int imWidth = 0;
    int imHeight = 0;
    int imComponents;

public:
    ~WidgetImage();

    // Read PNG from file
    void init(const std::string& pngfile);

    // Read PNG from buffer
    void init(const Byte* pngbuffer, Size size);

    virtual void dimensionalize() override;
    virtual void render() override;
};

}  // namespace ui
