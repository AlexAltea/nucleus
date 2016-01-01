/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "App.h"
#include <collection.h>
#include <ppltasks.h>

#include "nucleus/nucleus.h"

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;

// Entry Point
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^) {
    auto app = ref new NucleusAppSource();
    CoreApplication::Run(app);
    return 0;
}

IFrameworkView^ NucleusAppSource::CreateView() {
    return ref new NucleusApp();
}

NucleusApp::NucleusApp() {
}

void NucleusApp::Initialize(CoreApplicationView^ applicationView) {
    // App lifecycle events
    applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &NucleusApp::OnActivated);
    CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>(this, &NucleusApp::OnSuspending);
    CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>(this, &NucleusApp::OnResuming);
}

void NucleusApp::SetWindow(CoreWindow^ window) {
    // Window events
    window->SizeChanged +=
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &NucleusApp::OnWindowSizeChanged);
    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &NucleusApp::OnVisibilityChanged);
    window->Closed +=
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &NucleusApp::OnWindowClosed);

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
}

void NucleusApp::Load(Platform::String^ entryPoint) {
    // Initialize Nucleus UI
    IUnknown* window = reinterpret_cast<IUnknown*>(CoreWindow::GetForCurrentThread());
    unsigned int width = 960;
    unsigned int height = 544;
    nucleusPrepare(window, width, height);
}

void NucleusApp::Run() {
    nucleusInitialize(0, nullptr);

    CoreDispatcher^ dispatcher = CoreWindow::GetForCurrentThread()->Dispatcher;
    while (true) {
        dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void NucleusApp::Uninitialize() {
}

// Events handlers
void NucleusApp::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args) {
    CoreWindow::GetForCurrentThread()->Activate();
}

void NucleusApp::OnResuming(Platform::Object^ sender, Platform::Object^ args) {
}

void NucleusApp::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args) {
    SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

    Concurrency::create_task([this, deferral]() {
        deferral->Complete();
    });
}

void NucleusApp::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args) {
}

void NucleusApp::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args) {
}

void NucleusApp::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args) {
}
