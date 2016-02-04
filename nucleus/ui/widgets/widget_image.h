/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/core/resource.h"
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
    WidgetImage() {}
    WidgetImage(const std::string& id) : Widget(id) {}
    ~WidgetImage();

    /**
     * Create a pipeline to process this kind of widgets
     * @param[in]  backend  Graphics backend where the pipeline state object will be created
     */
    static gfx::Pipeline* createPipeline(gfx::IBackend& backend);

    /**
     * Read image from resource
     * @param[in]  resName   Resource identifier
     */
    void update(core::ResourceName resName);

    /**
     * Read image from file
     * @param[in]  filepath  Path to the image file
     */
    void update(const std::string& filepath);

    /**
     * Read image from buffer
     * @param[in]  buffer    Buffer holding the image
     * @param[in]  size      Size in bytes of the buffer
     */
    void update(const Byte* buffer, Size size);

    virtual void dimensionalize() override;
    virtual void render() override;
};

}  // namespace ui
