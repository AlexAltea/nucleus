/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/ui/widgets/widget_container.h"
#include "nucleus/ui/widgets/composite/widget_item.h"

#include <vector>

namespace ui {

class WidgetList : public WidgetContainer {
    std::vector<WidgetItem*> items;

public:
};

}  // namespace ui
