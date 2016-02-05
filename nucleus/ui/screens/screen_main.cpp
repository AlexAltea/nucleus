/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "screen_main.h"
#include "nucleus/ui/widgets/list.h"
#include "nucleus/ui/ui.h"

namespace ui {

ScreenMain::ScreenMain(UI* manager) : Screen(manager) {
    const auto* defaultFont = manager->fontRegular.get();
    const auto* lightFont = manager->fontLight.get();

    body.layout = WidgetContainer::LAYOUT_VERTICAL;

    auto* header = new WidgetContainer(&body, "header");
    header->style.width = 100_pct;
    header->style.height = 100_px;
    header->style.background = Color{0.282f, 0.282f, 0.500f, 1.0f};

    auto* content = new WidgetContainer(&body, "content");
    content->layout == WidgetContainer::LAYOUT_HORIZONTAL;

    auto* home = new WidgetContainer(content, "home");
    home->layout == WidgetContainer::LAYOUT_HORIZONTAL;
    home->style.alignH == ALIGN_HORIZONTAL_CENTER;

    auto* current = new WidgetContainer(home, "current");
    current->layout == WidgetContainer::LAYOUT_VERTICAL;
    auto* currentHeader = new WidgetText(current);
    currentHeader->update(lightFont, 80_px, "Current");
    currentHeader->style.color = Color{1,1,1,1};


    // content->home (@from=header)
    // content->home->current
    // content->home->current->window1..windowN
    // content->home->recent
    // content->home->recent->app1..appN
    // content->home->platforms
    // content->home->platforms->platform1..platformN

    // content->platform (@from=content->home->platforms, dynamic)
    // content->platform->details
    // content->platform->apps
    // content->platform->apps->app1..appN

    // content->profile (@from=header)
    // content->profile->details
    // content->profile->messages?
    // content->profile->achievements?
    // content->profile->friends?

    // content->settings (@from=header)
    // content->settings->?

    // content->help (@from=header)
    // content->help->?
}

void ScreenMain::update() {
}

}  // namespace ui
