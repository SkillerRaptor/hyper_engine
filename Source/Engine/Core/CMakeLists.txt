#-------------------------------------------------------------------------------------------
# Copyright (C) HyperEngine. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_library(Core STATIC ${SOURCES} ${HEADERS})
set_target_properties(Core PROPERTIES FOLDER Modules)
target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Core PUBLIC ${CMAKE_DL_LIBS} fmt)

if ("${HYPERENGINE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(Core PUBLIC HYPERENGINE_DEBUG=1)
else ()
    target_compile_definitions(Core PUBLIC HYPERENGINE_RELEASE=1)
endif ()
