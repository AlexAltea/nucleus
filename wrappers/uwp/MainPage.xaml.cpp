/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "pch.h"
#include "MainPage.xaml.h"

#include "nucleus/nucleus.h"

using namespace nucleus_uwp;

using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;

MainPage::MainPage() {
    InitializeComponent();

    // Global colors
    Color colorWhite    = ColorHelper::FromArgb(255, 255, 255, 255);
    Color colorWhiteD1  = ColorHelper::FromArgb(255, 224, 224, 224);
    Color colorWhiteD2  = ColorHelper::FromArgb(255, 200, 200, 200);
    Color colorBlue     = ColorHelper::FromArgb(255,  40,  40,  92);
    Color colorBlueL1   = ColorHelper::FromArgb(255,  56,  56, 110);
    Color colorBlueL2   = ColorHelper::FromArgb(255,  72,  72, 128);

    // Set Title Bar color
    ApplicationViewTitleBar^ titleBar = ApplicationView::GetForCurrentView()->TitleBar;
    titleBar->BackgroundColor = colorBlue;
    titleBar->ForegroundColor = colorWhite;
    titleBar->InactiveBackgroundColor = colorBlueL1;
    titleBar->InactiveForegroundColor = colorWhiteD1;

    titleBar->ButtonBackgroundColor = colorBlue;
    titleBar->ButtonHoverBackgroundColor = colorBlueL1;
    titleBar->ButtonPressedBackgroundColor = colorBlueL2;
    titleBar->ButtonInactiveBackgroundColor = colorBlueL1;

    titleBar->ButtonForegroundColor = colorWhiteD1;
    titleBar->ButtonHoverForegroundColor = colorWhite;
    titleBar->ButtonPressedForegroundColor = colorWhite;
    titleBar->ButtonInactiveForegroundColor = colorWhiteD2;

    // Initialize UI
    IUnknown* window = reinterpret_cast<IUnknown*>(CoreWindow::GetForCurrentThread());
    unsigned int width = 960;
    unsigned int height = 544;
    nucleusPrepare(window, width, height);
}
