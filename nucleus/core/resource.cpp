/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "resource.h"

#include <string>
#include <unordered_map>

#if defined(NUCLEUS_PLATFORM_WINDOWS)
#include "wrappers/windows/resource.h"
#include <Windows.h>
#endif

namespace core {

#if defined(NUCLEUS_PLATFORM_UWP)
#define RESOURCE(name, path) { RES_##name, path },
std::unordered_map<ResourceName, const char*> resourceMap = {
#include "resource.inl"
};

#elif defined(NUCLEUS_PLATFORM_WINDOWS)
#define RESOURCE(name, path) { RES_##name, IDR_##name },
std::unordered_map<ResourceName, int> resourceMap = {
#include "resource.inl"
};

#endif
#undef RESOURCE

Resource::Resource(ResourceName name) {
#if defined(NUCLEUS_PLATFORM_UWP)
    fopen_s(&file, resourceMap[name], "rb");
    _fseeki64(file, 0, SEEK_END);
    size = ftell(file);

    buffer.resize(size);
    data = buffer.data();
    _fseeki64(file, 0, SEEK_SET);
    fread(data, size, 1, file);

#elif defined(NUCLEUS_PLATFORM_WINDOWS)
    HRSRC hRes = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceMap[name]), "RESOURCE");
    HGLOBAL hGlob = LoadResource(GetModuleHandle(NULL), hRes);
    DWORD resSize = SizeofResource(GetModuleHandle(NULL), hRes);
    LPVOID resData = LockResource(hGlob);
    size = static_cast<size_t>(resSize);
    data = static_cast<void*>(resData);
#endif
}

Resource::~Resource() {
#if defined(NUCLEUS_PLATFORM_UWP)
    fclose(file);
#endif
}

}  // namespace core