cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

file(GLOB_RECURSE "HYPERRESOURCE_SOURCE" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE "HYPERRESOURCE_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

foreach(item IN ITEMS ${HYPERRESOURCE_SOURCE} ${HYPERRESOURCE_HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE "/" "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach()

add_library(HyperResource ${HYPERRESOURCE_SOURCE} ${HYPERRESOURCE_HEADERS})
target_include_directories(HyperResource PUBLIC
        "${CMAKE_SOURCE_DIR}/HyperResource/src/HyperResource/"
        "${CMAKE_SOURCE_DIR}/vendor/stb_image/include/")

target_link_libraries(HyperResource PUBLIC
        stb_image)

set_target_properties(HyperResource PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/HyperResource/Debug/")
set_target_properties(HyperResource PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/HyperResource/Release/")
