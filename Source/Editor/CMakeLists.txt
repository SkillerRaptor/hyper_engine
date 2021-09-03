#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperEditor
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/main.cpp
        src/HyperEditor/Editor.cpp)

set(HEADERS
        include/HyperEditor/Editor.hpp)

hyperengine_define_executable(HyperEditor)
target_link_libraries(HyperEditor PRIVATE HyperEngine)

hyperengine_copy_shared(HyperEditor ${CMAKE_SOURCE_DIR}/Source/ThirdParty/DiscordGameSDK/lib/discord_game_sdk.dll)
