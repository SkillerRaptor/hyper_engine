#-------------------------------------------------------------------------------------------
# Copyright (C) HyperEngine. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

set(HYPERENGINE_RENDERING_FOLDER ${CMAKE_SOURCE_DIR}/Source/Engine/Rendering)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${HYPERENGINE_RENDERING_FOLDER}/Common/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp ${HYPERENGINE_RENDERING_FOLDER}/Common/include/*.hpp)

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_library(DirectX11 SHARED ${SOURCES} ${HEADERS})
set_target_properties(DirectX11 PROPERTIES FOLDER Modules/Rendering)
target_include_directories(DirectX11 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${HYPERENGINE_RENDERING_FOLDER}/Common/include)
target_link_libraries(DirectX11 PUBLIC Core Math Platform)
