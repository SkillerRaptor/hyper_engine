#-------------------------------------------------------------------------------------------
# Copyright (C) HyperEngine. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_library(Physics STATIC ${SOURCES} ${HEADERS})
set_target_properties(Physics PROPERTIES FOLDER Modules)
target_include_directories(Physics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Physics PUBLIC Core)
