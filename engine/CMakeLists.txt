#-------------------------------------------------------------------------------------------
# Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# kernel
#-------------------------------------------------------------------------------------------
set(SOURCES
		src/hyper_engine/main.c
		src/hyper_engine/utilities/assertion.c
		src/hyper_engine/utilities/logger.c)

set(HEADERS
		include/hyper_engine/utilities/assertion.h
		include/hyper_engine/utilities/logger.h
		include/hyper_engine/utilities/source_location.h)
		
foreach(item IN ITEMS ${SOURCES} ${HEADERS})
		get_filename_component(src_path "${item}" PATH)
		string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
		string(REPLACE "/" "\\" group_path "${group_path}")
		source_group("${group_path}" FILES "${item}")
endforeach()

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_LENGTH)

add_executable(hyper_engine ${SOURCES} ${HEADERS})
target_include_directories(
		hyper_engine
		PRIVATE
		include)
target_compile_definitions(
		hyper_engine
		PRIVATE
		"-D SOURCE_PATH_LENGTH=${SOURCE_PATH_LENGTH}")
