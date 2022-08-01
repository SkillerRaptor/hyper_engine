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

add_library(hyper_project_options INTERFACE)
target_compile_features(hyper_project_options INTERFACE c_std_99)

add_library(hyper_project_warnings INTERFACE)

include(cmake/standard_project_settings.cmake)

include(cmake/compiler_warnings.cmake)
hyper_enable_warnings(hyper_project_warnings)

include(cmake/doxygen.cmake)
hyper_enable_doxygen()

#-------------------------------------------------------------------------------------------
# third party
#-------------------------------------------------------------------------------------------
add_subdirectory(third_party)

#-------------------------------------------------------------------------------------------
# engine
#-------------------------------------------------------------------------------------------
add_subdirectory(engine)
