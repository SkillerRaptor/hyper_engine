#-------------------------------------------------------------------------------------------
# Copyright (C) Hyperion. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

file(GLOB_RECURSE "SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE "HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")
add_library(HyperEngine STATIC "${SOURCES}" "${HEADERS}")

target_include_directories(HyperEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

target_link_libraries(HyperEngine PUBLIC HyperCore)

target_link_libraries(HyperEngine PUBLIC HyperMath)
target_link_libraries(HyperEngine PUBLIC HyperPlatform)

target_link_libraries(HyperEngine PUBLIC HyperAnimation)
target_link_libraries(HyperEngine PUBLIC HyperAudio)
target_link_libraries(HyperEngine PUBLIC HyperGame)
target_link_libraries(HyperEngine PUBLIC HyperNetwork)
target_link_libraries(HyperEngine PUBLIC HyperPhysics)
target_link_libraries(HyperEngine PUBLIC HyperResource)

hyperengine_set_output_directories(HyperEngine)
