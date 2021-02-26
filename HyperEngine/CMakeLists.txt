cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

file(GLOB_RECURSE "HYPERENGINE_SOURCE" "${CMAKE_SOURCE_DIR}/HyperEngine/src/*.cpp")
file(GLOB_RECURSE "HYPERENGINE_HEADERS" "${CMAKE_SOURCE_DIR}/HyperEngine/src/*.hpp")

foreach(item IN ITEMS ${HYPERENGINE_SOURCE} ${HYPERENGINE_HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE "/" "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach()

add_library(HyperEngine ${HYPERENGINE_SOURCE} ${HYPERENGINE_HEADERS})
target_include_directories(HyperEngine PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/HyperEngine/"
        "${CMAKE_SOURCE_DIR}/vendor/fmt/include/"
        "${CMAKE_SOURCE_DIR}/vendor/glad/include/"
        "${CMAKE_SOURCE_DIR}/vendor/glfw/include/"
        "${CMAKE_SOURCE_DIR}/vendor/imgui/"
        "${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/"
        "${CMAKE_SOURCE_DIR}/vendor/json/include/"
        "${CMAKE_SOURCE_DIR}/vendor/rttr/src"
        "${CMAKE_SOURCE_DIR}/vendor/stb_image/include/"
        )

message(STATUS "Searching for Vulkan")
find_package(Vulkan REQUIRED)

if(NOT Vulkan_FOUND)
    message(STATUS "Vulkan not found.")
    target_link_libraries(HyperEngine PUBLIC fmt glad glfw glm imgui ImGuizmo nlohmann_json rttr_core_lib stb_image)
    target_compile_definitions(HyperEngine PUBLIC HP_SUPPORT_VULKAN)
else()
    message(STATUS "Vulkan found.")
    target_link_libraries(HyperEngine PUBLIC fmt glad glfw glm imgui ImGuizmo nlohmann_json rttr_core_lib stb_image Vulkan::Vulkan)
endif()