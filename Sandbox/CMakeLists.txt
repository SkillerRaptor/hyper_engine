cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(OUTPUT_DIR_DEBUG "${CMAKE_BINARY_DIR}/bin/Sandbox/Debug")
set(OUTPUT_DIR_RELEASE "${CMAKE_BINARY_DIR}/bin/Sandbox/Release")

file(GLOB_RECURSE "SANDBOX_SOURCE" "${CMAKE_SOURCE_DIR}/Sandbox/src/*.cpp")
file(GLOB_RECURSE "SANDBOX_HEADERS" "${CMAKE_SOURCE_DIR}/Sandbox/src/*.hpp")

foreach(item IN ITEMS ${SANDBOX_SOURCE} ${SANDBOX_HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE "/" "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach()

add_executable(Sandbox ${SANDBOX_SOURCE} ${SANDBOX_HEADERS})

target_link_libraries(Sandbox PRIVATE HyperEngine)
target_include_directories(Sandbox PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/"
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

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")