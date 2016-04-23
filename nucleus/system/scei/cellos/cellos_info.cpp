/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "cellos_info.h"
#include "nucleus/filesystem/filesystem_app.h"
#include "nucleus/system/scei/psf.h"

namespace sys {
namespace scei {
namespace cellos {

InfoPlatform getInformation() {
    InfoPlatform platform;
    platform.name = "PlayStation 3";
    platform.version = "4.41 CEX";

    std::string dirPath = "platforms/ps3/dev_hdd0/game";
    auto dirList = fs::AppFileSystem::listDirectory(fs::APP_LOCATION_LOCAL, dirPath);
    for (const auto& entry : dirList) {
        InfoApp app;
        app.path = dirPath + "/" + entry.name;
        if (fs::AppFileSystem::existsFile(fs::APP_LOCATION_LOCAL, app.path + "/PARAM.SFO")) {
            auto file = fs::AppFileSystem::openFile(fs::APP_LOCATION_LOCAL, app.path + "/PARAM.SFO", fs::Read);
            PSFLoader psf;
            psf.open(file.get());
            app.id = psf.get_string("TITLE_ID");
            app.name = psf.get_string("TITLE");
            app.version = psf.get_string("VERSION");
            app.properties["Category"] = psf.get_string("CATEGORY");
            app.properties["Firmware"] = psf.get_string("PS3_SYSTEM_VER");
            if (fs::AppFileSystem::existsFile(fs::APP_LOCATION_LOCAL, app.path + "/ICON0.PNG")) {
                app.image = app.path + "/ICON0.PNG";
            }
            platform.apps.push_back(app);
        }
    }
    return platform;
}

}  // namespace cellos
}  // namespace scei
}  // namespace sys
