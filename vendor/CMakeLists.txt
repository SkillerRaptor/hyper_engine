cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

# Setting dependency settings
set(FMT_DOC False CACHE BOOL "" FORCE)
set(FMT_INSTALL False CACHE BOOL "" FORCE)
set(FMT_TEST False CACHE BOOL "" FORCE)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(BUILD_RTTR_DYNAMIC False CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS False CACHE BOOL "" FORCE)
set(BUILD_STATIC True CACHE BOOL "" FORCE)
set(BUILD_BENCHMARKS False CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES False CACHE BOOL "" FORCE)
set(BUILD_DOCUMENTATION False CACHE BOOL "" FORCE)
set(BUILD_INSTALLER False CACHE BOOL "" FORCE)
set(BUILD_PACKAGE False CACHE BOOL "" FORCE)

# Adding subdirectories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/fmt)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glad)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glfw)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/json)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/rttr)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stb_image)

# Adding imgui
file(GLOB "IMGUI_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp")
file(GLOB "IMGUI_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.h")
add_library(imgui ${IMGUI_SOURCES} ${IMGUI_HEADERS})
set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)

# Adding ImGuizmo
file(GLOB "IMGUIZMO_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo/*.cpp")
file(GLOB "IMGUIZMO_HEADERS" "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo/*.h")
add_library(ImGuizmo ${IMGUIZMO_SOURCES} ${IMGUIZMO_HEADERS})
target_include_directories(ImGuizmo PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/imgui/")
set_target_properties(ImGuizmo PROPERTIES LINKER_LANGUAGE CXX)

set_target_properties(fmt PROPERTIES FOLDER vendor/fmt)
set_target_properties(glad PROPERTIES FOLDER vendor/glad)
set_target_properties(uninstall PROPERTIES FOLDER vendor/uninstall)
set_target_properties(glfw PROPERTIES FOLDER vendor/glfw)
set_target_properties(imgui PROPERTIES FOLDER vendor/imgui)
set_target_properties(ImGuizmo PROPERTIES FOLDER vendor/ImGuizmo)
set_target_properties(rttr_core_lib PROPERTIES FOLDER vendor/rttr_core_lib)
set_target_properties(stb_image PROPERTIES FOLDER vendor/stb_image)