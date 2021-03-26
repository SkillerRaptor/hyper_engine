cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(PROJECT "Blank")

set(OUTPUT_DIR_DEBUG "${CMAKE_BINARY_DIR}/bin/${PROJECT}/Debug")
set(OUTPUT_DIR_RELEASE "${CMAKE_BINARY_DIR}/bin/${PROJECT}/Release")

file(GLOB_RECURSE "SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE "HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

foreach(item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE "." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach()

add_executable(${PROJECT} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT} PRIVATE HyperEngine HyperResource)
target_include_directories(${PROJECT} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/")

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/HyperEngine/${PROJECT}/Debug/")
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/HyperEngine/${PROJECT}/Release/")

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}/bin/HyperEngine/${PROJECT}/${CMAKE_BUILD_TYPE}/")