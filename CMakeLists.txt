cmake_minimum_required(VERSION 2.8)
project(nucleus)

# Paths
set(NUCLEUS_SOLUTION_DIR ${CMAKE_CURRENT_LIST_DIR})
set(EXECUTABLE_OUTPUT_PATH "${NUCLEUS_SOLUTION_DIR}/bin/")

# Requirements
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Nucleus can only be compiled on 64-bit platforms.")
endif()

# Dependencies
find_package(LLVM REQUIRED CONFIG)
find_package(OpenGL REQUIRED)
find_package(ZLIB)
if(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIR})
else()
    file(GLOB_RECURSE ZLIB_FILES
        "${NUCLEUS_SOLUTION_DIR}/externals/zlib/*.c"
        "${NUCLEUS_SOLUTION_DIR}/externals/zlib/*.h"
    )
    add_library(zlib STATIC ${ZLIB_FILES})
    include_directories("${NUCLEUS_SOLUTION_DIR}/externals/zlib")
    set(ZLIB_LIBRARY zlib)
endif()

# Find GLEW
if (NOT WIN32)
    find_path(GLEW_INCLUDE_PATH GL/glew.h
        /usr/include
        /usr/local/include
        /sw/include
        /opt/local/include
    )
    find_library(GLEW_LIBRARY NAMES GLEW glew PATHS
        /usr/lib64
        /usr/lib
        /usr/local/lib64
        /usr/local/lib
        /sw/lib
        /opt/local/lib
    )
endif()

# LLVM
llvm_map_components_to_libnames(LLVM_LIBS
    core
    support
)

# Project
if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
endif()

include_directories(
    ${NUCLEUS_SOLUTION_DIR}
    ${LLVM_INCLUDE_DIRS}
)

add_definitions(
    ${LLVM_DEFINITIONS}
)

file(GLOB_RECURSE NUCLEUS_FILES
    "${NUCLEUS_SOLUTION_DIR}/externals/mongoose/mongoose.c"
    "${NUCLEUS_SOLUTION_DIR}/externals/mongoose/mongoose.h"
    "${NUCLEUS_SOLUTION_DIR}/externals/aes.cpp"
    "${NUCLEUS_SOLUTION_DIR}/externals/aes.h"
    "${NUCLEUS_SOLUTION_DIR}/externals/sha1.cpp"
    "${NUCLEUS_SOLUTION_DIR}/externals/sha1.h"
    "${NUCLEUS_SOLUTION_DIR}/nucleus/*.cpp"
    "${NUCLEUS_SOLUTION_DIR}/nucleus/*.h"
)

# Discard platform-dependent files
if (NOT WIN32)
    file(GLOB_RECURSE DISCARD_FILES
        "${NUCLEUS_SOLUTION_DIR}/nucleus/gpu/direct3d/*.cpp"
        "${NUCLEUS_SOLUTION_DIR}/nucleus/gpu/direct3d/*.h"
        "${NUCLEUS_SOLUTION_DIR}/nucleus/ui/windows/*.cpp"
        "${NUCLEUS_SOLUTION_DIR}/nucleus/ui/windows/*.h"
    )
    list(REMOVE_ITEM NUCLEUS_FILES ${DISCARD_FILES})
endif()

# Target
add_executable(nucleus ${NUCLEUS_FILES})

target_link_libraries(nucleus ${ZLIB_LIBRARY} ${LLVM_LIBS} ${GLEW_LIBRARY} ${OPENGL_LIBRARIES})
set_target_properties(nucleus PROPERTIES LINKER_LANGUAGE CXX)
