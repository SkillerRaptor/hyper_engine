#-------------------------------------------------------------------------------------------
# Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# cmake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.20)
project(HyperEngine LANGUAGES C)

#-------------------------------------------------------------------------------------------
# standard settings
#-------------------------------------------------------------------------------------------
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(hyper_project_options INTERFACE)
target_compile_features(hyper_project_options INTERFACE c_std_99)

add_library(hyper_project_warnings INTERFACE)

include(cmake/compiler_warnings.cmake)
hyper_enable_warnings(hyper_project_warnings)

#-------------------------------------------------------------------------------------------
# third party
#-------------------------------------------------------------------------------------------
add_subdirectory(third_party)

#-------------------------------------------------------------------------------------------
# engine
#-------------------------------------------------------------------------------------------
add_subdirectory(engine)
