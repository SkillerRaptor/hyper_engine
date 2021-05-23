#-------------------------------------------------------------------------------------------
# Copyright (C) HyperEngine. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_library(DirectX11 SHARED ${SOURCES} ${HEADERS})
set_target_properties(DirectX11 PROPERTIES FOLDER Modules/Rendering)
target_include_directories(DirectX11 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(DirectX11 PUBLIC Core)
