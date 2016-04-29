/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/ui/icon.h"
#include "nucleus/ui/widgets/widget_container.h"
#include "nucleus/system/information.h"

#include <memory>

namespace ui {

// Forward declarations
class WidgetText;

class WidgetButton : public WidgetContainer {
    std::unique_ptr<WidgetText> wIcon;
    std::unique_ptr<WidgetText> wLabel;

    // Button is enabled
    bool enabled = true;

public:
    WidgetButton() {}
    WidgetButton(const std::string& id) : WidgetContainer(id) {}
    WidgetButton(Widget* parent, const std::string& id = "") : WidgetContainer(parent, id) {}

    enum Design {
        DESIGN_HEADER,
        DESIGN_DEFAULT,
    };

    /**
     * Specify the button text
     * @parma[in]  label  Button text
     */
    void setText(const std::string& label);

    /**
     * Specify the button icon
     * @parma[in]  icon  Button icon
     */
    void setIcon(Icon icon);

    /**
     * Set a predefined button design
     * @param[in]  design  Design
     */
    void setDesign(Design design);

    /**
     * Enables the button
     */
    void enable();

    /**
     * Disables the button
     */
    void disable();
};

}  // namespace ui
