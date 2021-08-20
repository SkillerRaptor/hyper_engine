#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# Sandbox
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/main.cpp
        src/Sandbox.cpp)

set(HEADERS
        include/Sandbox.hpp)

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")

hyperengine_define_executable(Sandbox)
target_link_libraries(Sandbox PRIVATE HyperEngine)
