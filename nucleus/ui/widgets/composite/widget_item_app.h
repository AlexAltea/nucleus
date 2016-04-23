/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/ui/widgets/widget_container.h"
#include "nucleus/system/information.h"

#include <memory>

namespace ui {

// Forward declarations
class WidgetText;

class WidgetItemApp : public WidgetContainer {
    std::unique_ptr<WidgetText> wName;
    std::unique_ptr<WidgetText> wVersion;

public:
    WidgetItemApp() {}
    WidgetItemApp(const std::string& id) : WidgetContainer(id) {}
    WidgetItemApp(Widget* parent, const std::string& id = "") : WidgetContainer(parent, id) {}

    /**
     * Update the widget contents to match the given application information
     * @param[in]  info  Information of the application
     */
    void setInfoApp(const sys::InfoApp& info);

    /**
     * Extends the widget size to show additional information and options
     */
    void expand();

    /**
     * Reduces the widget size to show only essential information
     */
    void collapse();
};

}  // namespace ui
