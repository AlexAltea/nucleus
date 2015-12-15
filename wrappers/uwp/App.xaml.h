/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "App.g.h"

namespace nucleus_uwp {

ref class App sealed {
protected:
    // Invoked when the application is launched normally by the end user
    virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

internal:
    App();

private:
    // Invoked when application execution is being suspended
    void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);

    // Invoked when Navigation to a certain page fails
    void OnNavigationFailed(Platform::Object^ sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs^ e);
};

}  // nucleus_uwp
