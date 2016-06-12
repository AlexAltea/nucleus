cmake_minimum_required(VERSION 2.8)
project(nucleus)

# Cross-compiling
option(TARGET_ANDROID   "Enable to target Android devices")
option(TARGET_IOS       "Enable to target iOS devices")
option(TARGET_LINUX     "Enable to target X11-based Linux distributions")
option(TARGET_OSX       "Enable to target OS X systems")
option(TARGET_UWP       "Enable to target the Universal Windows Platform")
option(TARGET_WINDOWS   "Enable to target desktop Windows systems")

# Guest platforms
option(PLATFORM_ANY     "Enable to support emulation in any available platform")
option(PLATFORM_PS3     "Enable to support PlayStation 3 emulation")
option(PLATFORM_PS4     "Enable to support PlayStation 4 emulation")
option(PLATFORM_PSP     "Enable to support PlayStation Portable emulation")
option(PLATFORM_PSVITA  "Enable to support PlayStation Vita emulation")

# Features
set(FEATURE_AUDIO_BACKEND_XAUDIO2   "False")
set(FEATURE_CPU_BACKEND_ARM         "False")
set(FEATURE_CPU_BACKEND_X86         "False")
set(FEATURE_GFX_BACKEND_DIRECT3D11  "False")
set(FEATURE_GFX_BACKEND_DIRECT3D12  "False")
set(FEATURE_GFX_BACKEND_OPENGL      "False")
set(FEATURE_GFX_BACKEND_VULKAN      "False")

# Target auto-select
if (NOT (TARGET_ANDROID OR TARGET_IOS OR TARGET_LINUX OR TARGET_OSX OR TARGET_UWP OR TARGET_WINDOWS))
    message(STATUS "No target system specified (-DTARGET_*=ON). Choosing target automatically...")
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        message(STATUS "Using TARGET_LINUX.")
        set(TARGET_LINUX ON)
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        message(STATUS "Using TARGET_OSX.")
        set(TARGET_OSX ON)
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        message(STATUS "Using TARGET_WINDOWS.")
        set(TARGET_WINDOWS ON)
    endif()
endif()

# Feature auto-select
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(FEATURE_GFX_BACKEND_OPENGL "True")
    set(FEATURE_GFX_BACKEND_VULKAN "True")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(FEATURE_GFX_BACKEND_OPENGL "True")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(FEATURE_AUDIO_BACKEND_XAUDIO2  "True")
    set(FEATURE_GFX_BACKEND_DIRECT3D11 "True")
    set(FEATURE_GFX_BACKEND_DIRECT3D12 "True")
    set(FEATURE_GFX_BACKEND_OPENGL "True")
    set(FEATURE_GFX_BACKEND_VULKAN "True")
endif()

# Paths
set(NUCLEUS_PATH_SOLUTION    ${CMAKE_CURRENT_LIST_DIR})
set(NUCLEUS_PATH_BINARIES   "${NUCLEUS_PATH_SOLUTION}/bin/")
set(NUCLEUS_PATH_EXTERNALS  "${NUCLEUS_PATH_SOLUTION}/externals")
set(NUCLEUS_PATH_LIBRARIES  "${NUCLEUS_PATH_SOLUTION}/libs/")
set(NUCLEUS_PATH_TESTS      "${NUCLEUS_PATH_SOLUTION}/tests/")
set(NUCLEUS_PATH_WRAPPERS   "${NUCLEUS_PATH_SOLUTION}/wrappers/")

# Project
if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fpermissive -pthread")
endif()

# Dependencies
find_package(OpenGL QUIET)
find_package(ZLIB QUIET)
find_package(X11 QUIET)

if (NOT OPENGL_FOUND)
    message(WARNING "OpenGL not found. Corresponding backend will be disabled")
    set(FEATURE_GFX_BACKEND_OPENGL "False")
endif()

file(GLOB_RECURSE NUCLEUS_FILES
    "${NUCLEUS_PATH_SOLUTION}/externals/mongoose/mongoose.c"
    "${NUCLEUS_PATH_SOLUTION}/externals/mongoose/mongoose.h"
    "${NUCLEUS_PATH_SOLUTION}/externals/aes.cpp"
    "${NUCLEUS_PATH_SOLUTION}/externals/aes.h"
    "${NUCLEUS_PATH_SOLUTION}/externals/sha1.cpp"
    "${NUCLEUS_PATH_SOLUTION}/externals/sha1.h"
    "${NUCLEUS_PATH_SOLUTION}/nucleus/*.cpp"
    "${NUCLEUS_PATH_SOLUTION}/nucleus/*.h"
)

# File specification macro
macro(NUCLEUS_FILES_APPEND)
    file(GLOB_RECURSE FILES_APPEND ${ARGV})
    list(APPEND NUCLEUS_FILES ${FILES_APPEND})
endmacro()
macro(NUCLEUS_FILES_REMOVE)
    file(GLOB_RECURSE FILES_REMOVE ${ARGV})
    list(REMOVE_ITEM NUCLEUS_FILES ${FILES_REMOVE})
endmacro()

# Set target-dependent files
if (TARGET_ANDROID)
    NUCLEUS_FILES_APPEND(
        "${NUCLEUS_PATH_SOLUTION}/wrappers/android/*.cpp"
        "${NUCLEUS_PATH_SOLUTION}/wrappers/android/*.h"
    )
elseif (TARGET_IOS)
    NUCLEUS_FILES_APPEND(
        "${NUCLEUS_PATH_SOLUTION}/wrappers/ios/*.cpp"
        "${NUCLEUS_PATH_SOLUTION}/wrappers/ios/*.h"
    )
elseif (TARGET_LINUX)
    NUCLEUS_FILES_APPEND(
        "${NUCLEUS_PATH_SOLUTION}/wrappers/linux/*.cpp"
        "${NUCLEUS_PATH_SOLUTION}/wrappers/linux/*.h"
    )
elseif (TARGET_OSX)
    NUCLEUS_FILES_APPEND(
        "${NUCLEUS_PATH_SOLUTION}/wrappers/osx/*.cpp"
        "${NUCLEUS_PATH_SOLUTION}/wrappers/osx/*.h"
    )
elseif (TARGET_UWP)
    NUCLEUS_FILES_APPEND(
        "${NUCLEUS_PATH_SOLUTION}/wrappers/uwp/*.cpp"
        "${NUCLEUS_PATH_SOLUTION}/wrappers/uwp/*.h"
    )
elseif (TARGET_WINDOWS)
    NUCLEUS_FILES_APPEND(
        "${NUCLEUS_PATH_SOLUTION}/wrappers/windows/*.cpp"
        "${NUCLEUS_PATH_SOLUTION}/wrappers/windows/*.h"
    )
endif()

# Set feature-dependent files
if (NOT FEATURE_AUDIO_BACKEND_XAUDIO2)
    NUCLEUS_FILES_REMOVE(
        "${NUCLEUS_PATH_SOLUTION}/nucleus/audio/backend/xaudio2/*"
    )
endif()
if (NOT FEATURE_GFX_BACKEND_DIRECT3D11)
    NUCLEUS_FILES_REMOVE(
        "${NUCLEUS_PATH_SOLUTION}/nucleus/graphics/backend/direct3d11/*"
    )
endif()
if (NOT FEATURE_GFX_BACKEND_DIRECT3D12)
    NUCLEUS_FILES_REMOVE(
        "${NUCLEUS_PATH_SOLUTION}/nucleus/graphics/backend/direct3d12/*"
    )
endif()
if (NOT FEATURE_GFX_BACKEND_OPENGL)
    NUCLEUS_FILES_REMOVE(
        "${NUCLEUS_PATH_SOLUTION}/nucleus/graphics/backend/opengl/*"
    )
endif()
if (NOT FEATURE_GFX_BACKEND_VULKAN)
    NUCLEUS_FILES_REMOVE(
        "${NUCLEUS_PATH_SOLUTION}/nucleus/graphics/backend/vulkan/*"
    )
endif()

# Executable
add_executable(nucleus ${NUCLEUS_FILES})
set_target_properties(nucleus PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${NUCLEUS_PATH_BINARIES})
set_target_properties(nucleus PROPERTIES OUTPUT_NAME "Nucleus")
set_target_properties(nucleus PROPERTIES LINKER_LANGUAGE CXX)
include_directories(${NUCLEUS_PATH_SOLUTION})

# Set feature definitions
if (FEATURE_AUDIO_BACKEND_XAUDIO2)
    add_definitions(-D_NUCLEUS_FEATURE_AUDIOBACKEND_XAUDIO2)
endif()
if (FEATURE_GFX_BACKEND_DIRECT3D11)
    add_definitions(-D_NUCLEUS_FEATURE_GFXBACKEND_DIRECT3D11)
endif()
if (FEATURE_GFX_BACKEND_DIRECT3D12)
    add_definitions(-D_NUCLEUS_FEATURE_GFXBACKEND_DIRECT3D12)
endif()
if (FEATURE_GFX_BACKEND_OPENGL)
    add_definitions(-D_NUCLEUS_FEATURE_GFXBACKEND_OPENGL)
endif()
if (FEATURE_GFX_BACKEND_VULKAN)
    add_definitions(-D_NUCLEUS_FEATURE_GFXBACKEND_VULKAN)
endif()

# Dependencies
if (OPENGL_FOUND)
    target_link_libraries(nucleus ${OPENGL_LIBRARIES})
endif()

if (ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIR})
    target_link_libraries(nucleus ${ZLIB_LIBRARY})
else()
    file(GLOB_RECURSE ZLIB_FILES
        "${NUCLEUS_PATH_EXTERNALS}/zlib/*.c"
        "${NUCLEUS_PATH_EXTERNALS}/zlib/*.h"
    )
    add_library(zlib STATIC ${ZLIB_FILES})
    include_directories("${NUCLEUS_PATH_EXTERNALS}/zlib")
    target_link_libraries(nucleus zlib)
endif()

# Set platform-dependent dependencies
if (TARGET_LINUX)
    target_link_libraries(nucleus ${CMAKE_DL_LIBS})
    if (X11_FOUND)
        include_directories(${X11_INCLUDE_DIR})
        target_link_libraries(nucleus ${X11_LIBRARIES})
    else()
        message(FATAL_ERROR "X11 could not be located")
    endif()
endif()
