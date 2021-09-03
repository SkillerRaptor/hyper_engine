#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# Discord Game SDK
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/achievement_manager.cpp
        src/activity_manager.cpp
        src/application_manager.cpp
        src/core.cpp
        src/image_manager.cpp
        src/lobby_manager.cpp
        src/network_manager.cpp
        src/overlay_manager.cpp
        src/relationship_manager.cpp
        src/storage_manager.cpp
        src/store_manager.cpp
        src/types.cpp
        src/user_manager.cpp
        src/voice_manager.cpp)

set(HEADERS
        include/achievement_manager.h
        include/activity_manager.h
        include/application_manager.h
        include/core.h
        include/discord.h
        include/event.h
        include/ffi.h
        include/image_manager.h
        include/lobby_manager.h
        include/network_manager.h
        include/overlay_manager.h
        include/relationship_manager.h
        include/storage_manager.h
        include/store_manager.h
        include/types.h
        include/user_manager.h
        include/voice_manager.h)

add_library(DiscordAPI SHARED IMPORTED)
if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    set_target_properties(
            DiscordAPI
            PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/discord_game_sdk.dll"
            IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/discord_game_sdk.dll.lib")
elseif ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    set_target_properties(
            DiscordAPI
            PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/discord_game_sdk.so")
endif ()

add_library(DiscordGameSDK ${SOURCES} ${HEADERS})
target_include_directories(DiscordGameSDK PUBLIC SYSTEM include)
target_link_libraries(DiscordGameSDK PUBLIC DiscordAPI)
hyperengine_set_folder(DiscordGameSDK ThirdParty)
