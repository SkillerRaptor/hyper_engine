#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# ThirdParty
#-------------------------------------------------------------------------------------------
set(BUILD_SHARED_LIBS OFF)

#-------------------------------------------------------------------------------------------
# GLFW
#-------------------------------------------------------------------------------------------
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(glfw)
set_target_properties(
        glfw
        PROPERTIES
        FOLDER "ThirdParty")
set_target_properties(
        update_mappings
        PROPERTIES
        FOLDER "ThirdParty")

#-------------------------------------------------------------------------------------------
# Vulkan-Headers
#-------------------------------------------------------------------------------------------
add_subdirectory(Vulkan-Headers)
set_target_properties(
        uninstall
        PROPERTIES
        FOLDER "ThirdParty")

#-------------------------------------------------------------------------------------------
# volk
#-------------------------------------------------------------------------------------------
set(VOLK_STATIC_DEFINES ON)
set(VOLK_PULL_IN_VULKAN ON)
set(VOLK_INSTALL OFF)
add_subdirectory(volk)
set_target_properties(
        volk
        PROPERTIES
        FOLDER "ThirdParty")
