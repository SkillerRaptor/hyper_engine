#-------------------------------------------------------------------------------------------
# Copyright (C) HyperEngine. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_library(DirectX12 SHARED ${SOURCES} ${HEADERS})
set_target_properties(DirectX12 PROPERTIES FOLDER Modules/Rendering)
target_include_directories(DirectX12 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(DirectX12 PUBLIC Core)
