/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "widget_item_app.h"
#include "nucleus/ui/widgets/list.h"
#include "nucleus/ui/ui.h"

namespace ui {

void WidgetItemApp::setInfoApp(const sys::InfoApp& info) {
    const auto* lightFont = manager->fontLight.get();

    wName = std::make_unique<WidgetText>(this);
    wVersion = std::make_unique<WidgetText>(this);
    wName->update(lightFont, 30_px, info.name);
    wVersion->update(lightFont, 20_px, info.version);
}

}  // namespace ui
