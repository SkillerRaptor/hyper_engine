#-------------------------------------------------------------------------------------------
# Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# hyper math
#-------------------------------------------------------------------------------------------
set(SOURCES
		src/hyper_math/hyper_math.c)

set(HEADERS
		include/hyper_math/common.h
		include/hyper_math/hyper_math.h)

foreach(item IN ITEMS ${SOURCES} ${HEADERS})
		get_filename_component(src_path "${item}" PATH)
		string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
		string(REPLACE "/" "\\" group_path "${group_path}")
		source_group("${group_path}" FILES "${item}")
endforeach()

add_library(hyper_math SHARED ${SOURCES} ${HEADERS})
target_compile_definitions(
		hyper_math
		PRIVATE
		-DHYPER_SYMBOLS_EXPORT=1)
target_include_directories(
		hyper_math
		PUBLIC
		include)
target_link_libraries(
		hyper_math
		PRIVATE
		hyper_common)
