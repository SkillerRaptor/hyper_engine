#-------------------------------------------------------------------------------------------
# Copyright (c) 2025-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------
add_library(
        renderdoc
        INTERFACE
        include/renderdoc_app.h)
target_include_directories(
        renderdoc
        SYSTEM
        INTERFACE
        include/)

set_target_properties(
        renderdoc
        PROPERTIES
        FOLDER "third_party")