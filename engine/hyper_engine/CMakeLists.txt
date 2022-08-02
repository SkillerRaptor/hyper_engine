#-------------------------------------------------------------------------------------------
# Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# hyper engine
#-------------------------------------------------------------------------------------------
set(SOURCES
		src/main.c
		src/hyper_engine/engine.c
		src/hyper_engine/module_loader.c)

set(HEADERS
		include/hyper_engine/engine.h
		include/hyper_engine/module_loader.h)

foreach(item IN ITEMS ${SOURCES} ${HEADERS})
		get_filename_component(src_path "${item}" PATH)
		string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
		string(REPLACE "/" "\\" group_path "${group_path}")
		source_group("${group_path}" FILES "${item}")
endforeach()

add_executable(hyper_engine ${SOURCES} ${HEADERS})
target_include_directories(
		hyper_engine
		PRIVATE
		include)
target_link_libraries(
		hyper_engine
		PRIVATE
		hyper_project_options
		hyper_project_warnings
		hyper_common
		hyper_game
		hyper_math
		hyper_platform
		hyper_rendering)

add_custom_command(
	TARGET hyper_engine
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets/ $<TARGET_FILE_DIR:hyper_engine>/assets)
