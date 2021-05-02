#-------------------------------------------------------------------------------------------
# Copyright (C) Hyperion. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(stb_image LANGUAGES C)

#-------------------------------------------------------------------------------------------
# Library
#-------------------------------------------------------------------------------------------
file(GLOB_RECURSE "SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE "HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

add_library(stb_image STATIC "${SOURCES}" "${HEADERS}")
target_include_directories(stb_image PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
set_target_properties(stb_image PROPERTIES LINKER_LANGUAGE C)