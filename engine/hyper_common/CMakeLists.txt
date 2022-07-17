#-------------------------------------------------------------------------------------------
# Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# hyper common
#-------------------------------------------------------------------------------------------
set(SOURCES
		src/hyper_common/assertion.c
		src/hyper_common/logger.c
		src/hyper_common/memory.c
		src/hyper_common/vector.c)

set(HEADERS
		include/hyper_common/assertion.h
		include/hyper_common/compiler.h
		include/hyper_common/debug.h
		include/hyper_common/file.h
		include/hyper_common/library.h
		include/hyper_common/logger.h
		include/hyper_common/memory.h
		include/hyper_common/platform.h
		include/hyper_common/source_location.h
		include/hyper_common/vector.h)

foreach(item IN ITEMS ${SOURCES} ${HEADERS})
		get_filename_component(src_path "${item}" PATH)
		string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
		string(REPLACE "/" "\\" group_path "${group_path}")
		source_group("${group_path}" FILES "${item}")
endforeach()

add_library(hyper_common STATIC ${SOURCES} ${HEADERS})
target_include_directories(
		hyper_common
		PUBLIC
		include)
target_link_libraries(
		hyper_common
		PRIVATE
		${CMAKE_DL_LIBS})
