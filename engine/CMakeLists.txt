#-------------------------------------------------------------------------------------------
# Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# kernel
#-------------------------------------------------------------------------------------------
set(SOURCES
		src/hyper_engine/main.c)

set(HEADERS
		)
		
foreach(item IN ITEMS ${SOURCES} ${HEADERS})
		get_filename_component(src_path "${item}" PATH)
		string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
		string(REPLACE "/" "\\" group_path "${group_path}")
		source_group("${group_path}" FILES "${item}")
endforeach()

add_executable(hyper_engine ${SOURCES} ${HEADERS})
target_include_directories(
		hyper_engine
		PUBLIC
		include)
