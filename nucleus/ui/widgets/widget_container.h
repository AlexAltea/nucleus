/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/widget.h"

#include <vector>

namespace ui {

class WidgetContainer : public Widget {
private:
    // Input
    WidgetContainerInput input;

    // Elements of the widget
    std::vector<Widget*> children;

public:
    enum Layout {
        LAYOUT_VERTICAL,
        LAYOUT_HORIZONTAL,
    } layout;

    bool scrollV;
    bool scrollH;
    float scrollVoffset = 0.0f;
    float scrollHoffset = 0.0f;

    WidgetContainer() {}
    WidgetContainer(const std::string& id) : Widget(id) {}
    WidgetContainer(Widget* parent, const std::string& id = "") : Widget(parent, id) {}

    /**
     * Create a pipeline to process this kind of widgets
     * @param[in]  backend  Graphics backend where the pipeline state object will be created
     */
    static gfx::Pipeline* createPipeline(gfx::IBackend& backend);

    /**
     * Add an element to this container
     * @param[in]  widget  Widget to be added
     * @return             True on success
     */
    bool addElement(Widget* widget);

    /**
     * Removes an element from this container given its identifier
     * @param[in]  target  Identifier of the widget to be removed
     * @return             True on success
     */
    bool removeElement(const std::string& target);

    virtual Widget* find(const std::string& query) override;
    virtual void dimensionalize() override;
    virtual void render() override;
    virtual void handle(Event& evt) override;
};

}  // namespace ui
