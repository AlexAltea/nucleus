/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/widget.h"

#include <vector>

namespace ui {

class WidgetContainer : public Widget {
    // Elements of the widget
    std::vector<Widget*> children;

public:
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
    virtual void render(gfx::CommandBuffer* cmdBuffer) override;
};

}  // namespace ui
