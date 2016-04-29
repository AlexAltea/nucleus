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

    // Body
    body.layout = WidgetContainer::LAYOUT_VERTICAL;

    // Header
    auto* header = new WidgetContainer(&body, "header");
    header->layout = WidgetContainer::LAYOUT_HORIZONTAL;
    header->style.alignH = ALIGN_HORIZONTAL_CENTER;
    header->style.alignV = ALIGN_VERTICAL_BOTTOM;
    header->style.width = 100_pct;
    header->style.height = 100_px;
    header->style.background = Color{0.282f, 0.282f, 0.500f, 1.0f};

    auto* buttonHome = new WidgetButton(header);
    buttonHome->setDesign(WidgetButton::DESIGN_HEADER);
    buttonHome->setIcon(ICON_HOME);
    buttonHome->setText("Home");
    auto* buttonSettings = new WidgetButton(header);
    buttonSettings->setDesign(WidgetButton::DESIGN_HEADER);
    buttonSettings->setIcon(ICON_SETTINGS);
    buttonSettings->setText("Settings");

    // Content
    auto* content = new WidgetContainer(&body, "content");
    content->layout = WidgetContainer::LAYOUT_VERTICAL;
    content->style.background = Color{0.157f, 0.157f, 0.360f, 1.0f};

    auto* home = new WidgetContainer(content, "home");
    home->layout = WidgetContainer::LAYOUT_VERTICAL;
    home->style.width = 100_pct;
    home->style.alignH = ALIGN_HORIZONTAL_CENTER;

    auto* current = new WidgetContainer(home, "current");
    current->layout = WidgetContainer::LAYOUT_VERTICAL;
    current->style.width = 30_pct;
    auto* currentHeader = new WidgetText(current);
    currentHeader->update(lightFont, 60_px, "Current");
    currentHeader->style.color = Color{1,1,1,1};

    auto* recent = new WidgetContainer(home, "recent");
    recent->layout = WidgetContainer::LAYOUT_VERTICAL;
    recent->style.width = 30_pct;
    auto* recentHeader = new WidgetText(recent);
    recentHeader->update(lightFont, 60_px, "Recent");
    recentHeader->style.color = Color{1,1,1,1};

    auto* platforms = new WidgetContainer(home, "platforms");
    platforms->layout = WidgetContainer::LAYOUT_VERTICAL;
    platforms->style.width = 30_pct;
    auto* platformsHeader = new WidgetText(platforms);
    platformsHeader->update(lightFont, 60_px, "Platforms");
    platformsHeader->style.color = Color{1,1,1,1};


    auto* platform = new WidgetContainer(content, "platform");
    platform->layout = WidgetContainer::LAYOUT_VERTICAL;
    platform->style.width = 100_pct;
    platform->style.alignH = ALIGN_HORIZONTAL_CENTER;

    auto* apps = new WidgetContainer(content, "apps");
    apps->layout = WidgetContainer::LAYOUT_VERTICAL;
    apps->style.width = 100_pct;
    apps->style.alignH = ALIGN_HORIZONTAL_CENTER;

    sys::InfoPlatform infoPlatform = sys::getInformation(core::PLATFORM_PS3);
    for (const auto& infoApp : infoPlatform.apps) {
        auto widgetApp = new WidgetItemApp(home);
        widgetApp->setInfoApp(infoApp);
    }

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
