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
        set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION ${HYPERENGINE_IMPORT_LOCATION}/${library}.lib)
        set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION_DEBUG ${HYPERENGINE_IMPORT_LOCATION}/${library}d.lib)
        set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION_RELEASE ${HYPERENGINE_IMPORT_LOCATION}/${library}.lib)
        set_property(TARGET ${target} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/${include})
    endfunction()
elseif ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    # TODO: Fixing undefined reference for linux build

    set(HYPERENGINE_IMPORT_LOCATION ${CMAKE_SOURCE_DIR}/Source/Platforms/Linux/Binaries)

    function(hyperengine_import_library target type library include)
        add_library(${target} ${type} IMPORTED GLOBAL)
        set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION ${HYPERENGINE_IMPORT_LOCATION}/lib${library}.a)
        set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION_DEBUG ${HYPERENGINE_IMPORT_LOCATION}/lib${library}d.a)
        set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION_RELEASE ${HYPERENGINE_IMPORT_LOCATION}/lib${library}.a)
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
