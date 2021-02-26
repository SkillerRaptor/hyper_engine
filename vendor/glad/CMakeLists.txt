cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(glad LANGUAGES C)

file(GLOB_RECURSE "SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE "HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

add_library(glad STATIC ${SOURCES} ${HEADERS})
target_include_directories(glad PUBLIC include)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)