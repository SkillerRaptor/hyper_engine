#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# ThirdParty
#-------------------------------------------------------------------------------------------
find_package(OpenGL QUIET)
if (OPENGL_FOUND)
    message(STATUS "'OpenGL' found and enabled")
    set(HYPERENGINE_BUILD_OPENGL ON PARENT_SCOPE)
endif ()

find_package(Vulkan QUIET)
if (Vulkan_FOUND)
    message(STATUS "'Vulkan' found and enabled")
    # set(HYPERENGINE_BUILD_VULKAN ON PARENT_SCOPE)
endif ()
