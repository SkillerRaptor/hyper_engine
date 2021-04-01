cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(OUTPUT_DIR_DEBUG "${CMAKE_BINARY_DIR}/bin/HyperEngine/HyperEditor/Debug")
set(OUTPUT_DIR_RELEASE "${CMAKE_BINARY_DIR}/bin/HyperEngine/HyperEditor/Release")

file(GLOB_RECURSE "SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE "HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

foreach(item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE "." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach()

add_executable(HyperEditor ${SOURCES} ${HEADERS})

target_link_libraries(HyperEditor PRIVATE
        HyperEngine
        HyperResource
        imgui
        ImGuizmo)

target_include_directories(HyperEditor PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/"
        "${CMAKE_SOURCE_DIR}/vendor/imgui/include/"
        "${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/include/")

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/HyperEngine/HyperEditor/Debug/")
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/HyperEngine/HyperEditor/Release/")

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}/bin/HyperEngine/HyperEditor/${CMAKE_BUILD_TYPE}/")

set_property(TARGET HyperEditor PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/HyperEngine/HyperEditor/${CMAKE_BUILD_TYPE}/")
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT HyperEditor)