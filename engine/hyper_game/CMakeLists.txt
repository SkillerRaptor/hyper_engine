#-------------------------------------------------------------------------------------------
# Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# hyper game
#-------------------------------------------------------------------------------------------
set(SOURCES
		src/hyper_game/event_bus.c
		src/hyper_game/hyper_game.c)

set(HEADERS
		include/hyper_game/event_bus.h
		include/hyper_game/hyper_game.h
		include/hyper_game/key_codes.h
		include/hyper_game/key_events.h
		include/hyper_game/mouse_codes.h
		include/hyper_game/mouse_events.h
		include/hyper_game/window_events.h)

foreach(item IN ITEMS ${SOURCES} ${HEADERS})
		get_filename_component(src_path "${item}" PATH)
		string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
		string(REPLACE "/" "\\" group_path "${group_path}")
		source_group("${group_path}" FILES "${item}")
endforeach()

add_library(hyper_game SHARED ${SOURCES} ${HEADERS})
target_compile_definitions(
		hyper_game
		PRIVATE
		-DHYPER_SYMBOLS_EXPORT=1)
target_include_directories(
		hyper_game
		PUBLIC
		include)
target_link_libraries(
		hyper_game
		PRIVATE
		hyper_common
		hyper_platform)
