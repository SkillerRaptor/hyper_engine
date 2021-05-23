#-------------------------------------------------------------------------------------------
# Copyright (C) HyperEngine. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

#-------------------------------------------------------------------------------------------
# Variables
#-------------------------------------------------------------------------------------------

if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    set(HYPERENGINE_IMPORT_LOCATION ${CMAKE_SOURCE_DIR}/Source/Platforms/Windows/Binaries)

    function(hyperengine_import_library target type library include)
        add_library(${target} ${type} IMPORTED GLOBAL)

        if ("${HYPERENGINE_BUILD_TYPE}" STREQUAL "Debug")
            set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION ${HYPERENGINE_IMPORT_LOCATION}/${library}d.lib)
        elseif ("${HYPERENGINE_BUILD_TYPE}" STREQUAL "Release")
            set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION ${HYPERENGINE_IMPORT_LOCATION}/${library}.lib)
        endif ()
        set_property(TARGET ${target} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/${include})
    endfunction()
elseif ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    set(HYPERENGINE_IMPORT_LOCATION ${CMAKE_SOURCE_DIR}/Source/Platforms/Linux/Binaries)

    function(hyperengine_import_library target type library include)
        add_library(${target} ${type} IMPORTED GLOBAL)
        if ("${HYPERENGINE_BUILD_TYPE}" STREQUAL "Debug")
            set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION ${HYPERENGINE_IMPORT_LOCATION}/${library}d.a)
        elseif ("${HYPERENGINE_BUILD_TYPE}" STREQUAL "Release")
            set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION ${HYPERENGINE_IMPORT_LOCATION}/${library}.a)
        endif ()
        set_property(TARGET ${target} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/${include})
    endfunction()
endif ()

#-------------------------------------------------------------------------------------------
# fmt
#-------------------------------------------------------------------------------------------
hyperengine_import_library(fmt STATIC fmt fmt)

#-------------------------------------------------------------------------------------------
# glfw
#-------------------------------------------------------------------------------------------
hyperengine_import_library(glfw STATIC glfw3 glfw)
