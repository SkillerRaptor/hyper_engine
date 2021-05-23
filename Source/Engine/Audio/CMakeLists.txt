#-------------------------------------------------------------------------------------------
# Copyright (C) HyperEngine. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_library(Audio STATIC ${SOURCES} ${HEADERS})
set_target_properties(Audio PROPERTIES FOLDER Modules)
target_include_directories(Audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Audio PUBLIC Core)
