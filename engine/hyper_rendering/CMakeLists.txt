#-------------------------------------------------------------------------------------------
# Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# hyper rendering
#-------------------------------------------------------------------------------------------
set(SOURCES
		src/hyper_rendering/graphics_context.c
		src/hyper_rendering/hyper_rendering.c
		src/hyper_rendering/vulkan/vulkan_context.c
		src/hyper_rendering/vulkan/vulkan_device.c)

set(HEADERS
		include/hyper_rendering/graphics_context.h
		include/hyper_rendering/hyper_rendering.h
		include/hyper_rendering/vulkan/vulkan_context.h
		include/hyper_rendering/vulkan/vulkan_device.h)

foreach(item IN ITEMS ${SOURCES} ${HEADERS})
		get_filename_component(src_path "${item}" PATH)
		string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
		string(REPLACE "/" "\\" group_path "${group_path}")
		source_group("${group_path}" FILES "${item}")
endforeach()

add_library(hyper_rendering SHARED ${SOURCES} ${HEADERS})
target_compile_definitions(
		hyper_rendering
		PRIVATE
		-DHYPER_SYMBOLS_EXPORT=1)
target_include_directories(
		hyper_rendering
		PUBLIC
		include)
target_link_libraries(
		hyper_rendering
		PRIVATE
		volk
		hyper_common
		hyper_platform)
