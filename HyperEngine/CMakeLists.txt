cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(OUTPUT_DIR_DEBUG "${CMAKE_BINARY_DIR}/bin/HyperEngine/HyperEngine/Debug")
set(OUTPUT_DIR_RELEASE "${CMAKE_BINARY_DIR}/bin/HyperEngine/HyperEngine/Release")

file(GLOB_RECURSE "HYPERENGINE_SOURCE" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE "HYPERENGINE_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

foreach(item IN ITEMS ${HYPERENGINE_SOURCE} ${HYPERENGINE_HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE "/" "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach()

add_library(HyperEngine ${HYPERENGINE_SOURCE} ${HYPERENGINE_HEADERS})
target_include_directories(HyperEngine PUBLIC
        "${CMAKE_SOURCE_DIR}/HyperEngine/src/HyperEngine/"
        "${CMAKE_SOURCE_DIR}/HyperEngine/src/HyperEngine/HyperCommon"
        "${CMAKE_SOURCE_DIR}/vendor/fmt/include/"
        "${CMAKE_SOURCE_DIR}/vendor/glad/include/"
        "${CMAKE_SOURCE_DIR}/vendor/glfw/include/"
        "${CMAKE_SOURCE_DIR}/vendor/glm/"
        "${CMAKE_SOURCE_DIR}/vendor/json/include/"
        "${CMAKE_SOURCE_DIR}/vendor/rttr/src/")

target_link_libraries(HyperEngine PUBLIC
        fmt
        glad
        glfw
        glm
        nlohmann_json
        rttr_core_lib)

set_target_properties(HyperEngine PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/HyperEngine/Debug/")
set_target_properties(HyperEngine PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/HyperEngine/Release/")

message(STATUS "Searching for Vulkan")
find_package(Vulkan)

if(NOT Vulkan_FOUND)
    message(STATUS "Vulkan not found.")
    target_compile_definitions(HyperEngine PUBLIC GLFW_INCLUDE_NONE)
    target_compile_definitions(HyperEngine PUBLIC HYPERENGINE_BUILD_VULKAN=0)
else()
    message(STATUS "Vulkan found.")
    target_link_libraries(HyperEngine PUBLIC Vulkan::Vulkan)
    target_compile_definitions(HyperEngine PUBLIC HYPERENGINE_BUILD_VULKAN=1)
endif()