#-------------------------------------------------------------------------------------------
# Copyright (C) HyperEngine. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_library(Platform STATIC ${SOURCES} ${HEADERS})
set_target_properties(Platform PROPERTIES FOLDER Modules)
target_include_directories(Platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/Source/Engine/Rendering/Common/include)
target_link_libraries(Platform PUBLIC Core)

if ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    find_package(X11)
    target_include_directories(Platform PUBLIC ${X11_INCLUDE_DIR})
    target_link_libraries(Platform PUBLIC ${X11_LIBRARIES})
endif()

#-------------------------------------------------------------------------------------------
# Graphic APIs
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# DirectX
#-------------------------------------------------------------------------------------------
if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    target_compile_definitions(Platform PUBLIC HYPERENGINE_BUILD_DIRECTX11=1)
    target_compile_definitions(Platform PUBLIC HYPERENGINE_BUILD_DIRECTX12=1)
endif ()

#-------------------------------------------------------------------------------------------
# OpenGL
#-------------------------------------------------------------------------------------------
find_package(OpenGL)
if (OPENGL_FOUND)
    target_compile_definitions(Platform PUBLIC HYPERENGINE_BUILD_OPENGL33=1)
    target_compile_definitions(Platform PUBLIC HYPERENGINE_BUILD_OPENGL46=1)
endif ()

#-------------------------------------------------------------------------------------------
# Vulkan
#-------------------------------------------------------------------------------------------
find_package(Vulkan)
if (Vulkan_FOUND)
    target_compile_definitions(Platform PUBLIC HYPERENGINE_BUILD_VULKAN=1)
endif ()
