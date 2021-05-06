#-------------------------------------------------------------------------------------------
# Copyright (C) Hyperion. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

file(GLOB_RECURSE "SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE "HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")
add_library(HyperCore STATIC "${SOURCES}" "${HEADERS}")

target_include_directories(HyperCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

target_link_libraries(HyperCore PUBLIC fmt)

hyperengine_set_output_directories(HyperCore)