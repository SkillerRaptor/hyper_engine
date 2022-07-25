#-------------------------------------------------------------------------------------------
# Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# hyper platform
#-------------------------------------------------------------------------------------------
set(SOURCES
		src/hyper_platform/hyper_platform.c
		src/hyper_platform/window.c)

set(HEADERS
		include/hyper_platform/hyper_platform.h
		include/hyper_platform/window.h)

foreach(item IN ITEMS ${SOURCES} ${HEADERS})
		get_filename_component(src_path "${item}" PATH)
		string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
		string(REPLACE "/" "\\" group_path "${group_path}")
		source_group("${group_path}" FILES "${item}")
endforeach()

add_library(hyper_platform SHARED ${SOURCES} ${HEADERS})
target_compile_definitions(
		hyper_platform
		PRIVATE
		-DHYPER_SYMBOLS_EXPORT=1)
target_include_directories(
		hyper_platform
		PUBLIC
		include)
target_link_libraries(
		hyper_platform
		PRIVATE
		glfw
		volk
		hyper_project_options
		hyper_project_warnings
		hyper_common)
