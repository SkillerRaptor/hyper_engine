cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

file(GLOB_RECURSE "HYPEREDITOR_SOURCE" "${CMAKE_SOURCE_DIR}/HyperEditor/src/*.cpp")
file(GLOB_RECURSE "HYPEREDITOR_HEADERS" "${CMAKE_SOURCE_DIR}/HyperEditor/src/*.hpp")

foreach(item IN ITEMS ${HYPEREDITOR_SOURCE} ${HYPEREDITOR_HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE "/" "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach()

add_executable(HyperEditor ${HYPEREDITOR_SOURCE} ${HYPEREDITOR_HEADERS})

target_link_libraries(HyperEditor PRIVATE HyperEngine)
target_include_directories(HyperEditor PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/HyperEditor/"
        "${CMAKE_SOURCE_DIR}/HyperEngine/src/"
        "${CMAKE_SOURCE_DIR}/HyperEngine/src/HyperEngine/"
        "${CMAKE_SOURCE_DIR}/vendor/fmt/include/"
        "${CMAKE_SOURCE_DIR}/vendor/glad/include/"
        "${CMAKE_SOURCE_DIR}/vendor/glfw/include/"
        "${CMAKE_SOURCE_DIR}/vendor/imgui/"
        "${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/"
        "${CMAKE_SOURCE_DIR}/vendor/json/include/"
        "${CMAKE_SOURCE_DIR}/vendor/rttr/src"
        "${CMAKE_SOURCE_DIR}/vendor/stb_image/include/"
        )

set_target_properties(HyperEditor PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/HyperEngine/Debug/")
set_target_properties(HyperEditor PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/HyperEngine/Release/")

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}/bin/HyperEngine/${CMAKE_BUILD_TYPE}/")

set_property(TARGET HyperEditor PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/HyperEngine/${CMAKE_BUILD_TYPE}/")
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT HyperEditor)