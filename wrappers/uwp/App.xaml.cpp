/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "pch.h"
#include "MainPage.xaml.h"

using namespace nucleus_uwp;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Navigation;

// Entry Point
App::App() {
    InitializeComponent();
    Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
}

void App::OnLaunched(LaunchActivatedEventArgs^ e) {
#if _DEBUG
    // Show graphics profiling information while debugging.
    if (IsDebuggerPresent()) {
        DebugSettings->EnableFrameRateCounter = true;
    }
#endif

    auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);

    if (rootFrame == nullptr) {
        rootFrame = ref new Frame();
        rootFrame->NavigationFailed += ref new NavigationFailedEventHandler(this, &App::OnNavigationFailed);
        rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);
        Window::Current->Content = rootFrame;
    }
    
    if (rootFrame->Content == nullptr) {
        rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);
    }

    Window::Current->Activate();
}

void App::OnSuspending(Object^ sender, SuspendingEventArgs^ e) {
    (void) sender;  // Unused parameter
    (void) e;       // Unused parameter

    // TODO: Save application state and stop any background activity
}

void App::OnNavigationFailed(Platform::Object ^sender, NavigationFailedEventArgs ^e) {
    throw ref new FailureException("Failed to load Page " + e->SourcePageType.Name);
}
