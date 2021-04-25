#-------------------------------------------------------------------------------------------
# Copyright (C) Hyperion. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

#-------------------------------------------------------------------------------------------
# HyperResource
#-------------------------------------------------------------------------------------------
file(GLOB_RECURSE "C_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE "C_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE "CXX_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE "CXX_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

set(SOURCES ${C_SOURCES} ${CXX_SOURCES})
set(HEADERS ${C_HEADERS} ${CXX_HEADERS})

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")
add_library(HyperResource STATIC ${SOURCES} ${HEADERS})

target_include_directories(HyperResource PUBLIC ${CMAKE_SOURCE_DIR}/HyperResource/src/HyperResource/)

target_link_libraries(HyperResource PUBLIC stb_image)
target_include_directories(HyperResource PUBLIC ${CMAKE_SOURCE_DIR}/vendor/stb_image/include/)

hyperengine_set_output_directories(HyperResource)