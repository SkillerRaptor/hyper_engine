#-------------------------------------------------------------------------------------------
# Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.15)

#-------------------------------------------------------------------------------------------
# Functions
#-------------------------------------------------------------------------------------------
if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(HYPERENGINE_IMPORT_LOCATION ${CMAKE_SOURCE_DIR}/Source/Platforms/Windows/Binaries/MSVC)
    endif ()

    function(hyperengine_import_library target type library include)
        add_library(${target} ${type} IMPORTED GLOBAL)

        if ("${type}" STREQUAL "STATIC")
            set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION ${HYPERENGINE_IMPORT_LOCATION}/${library}.lib)
            set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION_DEBUG ${HYPERENGINE_IMPORT_LOCATION}/${library}d.lib)
            set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION_RELEASE ${HYPERENGINE_IMPORT_LOCATION}/${library}.lib)
            set_property(TARGET ${target} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/${include})
        endif ()
    endfunction()
elseif ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(HYPERENGINE_IMPORT_LOCATION ${CMAKE_SOURCE_DIR}/Source/Platforms/Linux/Binaries/Clang)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(HYPERENGINE_IMPORT_LOCATION ${CMAKE_SOURCE_DIR}/Source/Platforms/Linux/Binaries/GCC)
    endif ()

    function(hyperengine_import_library target type library include)
        add_library(${target} ${type} IMPORTED GLOBAL)

        if ("${type}" STREQUAL "STATIC")
            set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION ${HYPERENGINE_IMPORT_LOCATION}/lib${library}.a)
            set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION_DEBUG ${HYPERENGINE_IMPORT_LOCATION}/lib${library}d.a)
            set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION_RELEASE ${HYPERENGINE_IMPORT_LOCATION}/lib${library}.a)
            set_property(TARGET ${target} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/${include})
        endif ()
    endfunction()
endif ()

#-------------------------------------------------------------------------------------------
# fmt
#-------------------------------------------------------------------------------------------
hyperengine_import_library(fmt STATIC fmt fmt)
