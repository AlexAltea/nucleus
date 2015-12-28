/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "resource.h"

#include <string>
#include <unordered_map>

#if defined(NUCLEUS_PLATFORM_UWP)
#elif defined(NUCLEUS_PLATFORM_WINDOWS)
#include "wrappers/windows/resource.h"
#include <Windows.h>
#endif

namespace core {

#if defined(NUCLEUS_PLATFORM_UWP)
#define RESOURCE(name, path) { RES_##name, "ms-appdata:///assets/" #path },
std::unordered_map<ResourceName, const char*> resourceMap = {
};

#elif defined(NUCLEUS_PLATFORM_WINDOWS)
#define RESOURCE(name, path) { RES_##name, IDR_##name },
std::unordered_map<ResourceName, int> resourceMap = {
};

#endif
#undef RESOURCE

Resource::Resource(ResourceName name) {
#if defined(NUCLEUS_PLATFORM_UWP)
#elif defined(NUCLEUS_PLATFORM_WINDOWS)
    HRSRC hRes = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceMap[name]), "LANGUAGE");
    HGLOBAL hGlob = LoadResource(GetModuleHandle(NULL), hRes);
    DWORD resSize = SizeofResource(GetModuleHandle(NULL), hRes);
    LPVOID resData = LockResource(hGlob);
    size = static_cast<size_t>(resSize);
    data = static_cast<void*>(resData);
#endif
}

Resource::~Resource() {
#if defined(NUCLEUS_PLATFORM_UWP)
    // TODO
#endif
}

}  // namespace core