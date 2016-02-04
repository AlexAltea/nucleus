/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "screen_main.h"

namespace ui {

ScreenMain::ScreenMain(UI* manager) : Screen(manager) {
    body.layout = WidgetContainer::LAYOUT_VERTICAL;

    auto* header = new WidgetContainer("header");
    header->style.width = 100_pct;
    header->style.padding = 100_px;
    header->style.background = Color{0.7f, 0.7f, 0.2f, 1.0f};

    auto* content = new WidgetContainer("content");
    content->style.padding = 100_px;

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

    body.addElement(header);
    body.addElement(content);
}

void ScreenMain::update() {
}

}  // namespace ui
