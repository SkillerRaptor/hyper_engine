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

if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    set(DISCORD_GAME_SDK "${CMAKE_SOURCE_DIR}/Source/ThirdParty/DiscordGameSDK/lib/discord_game_sdk.dll")
elseif ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    set(DISCORD_GAME_SDK "${CMAKE_SOURCE_DIR}/Source/ThirdParty/DiscordGameSDK/lib/discord_game_sdk.so")
endif ()

hyperengine_copy_post_build(HyperEditor "${DISCORD_GAME_SDK}")
