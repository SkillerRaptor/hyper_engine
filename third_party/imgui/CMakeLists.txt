#-------------------------------------------------------------------------------------------
# Copyright (c) 2026-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

set(SOURCES
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp)

set(HEADERS
        ${imgui_SOURCE_DIR}/imconfig.h
        ${imgui_SOURCE_DIR}/imgui.h
        ${imgui_SOURCE_DIR}/imgui_internal.h
        ${imgui_SOURCE_DIR}/imstb_rectpack.h
        ${imgui_SOURCE_DIR}/imstb_textedit.h
        ${imgui_SOURCE_DIR}/imstb_truetype.h
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.h)

if (WIN32)
    set(SOURCES
            ${SOURCES}
            ${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.cpp)

    set(HEADERS
            ${HEADERS}
            ${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.h)
endif ()

add_library(imgui ${SOURCES} ${HEADERS})
target_include_directories(
        imgui
        SYSTEM
        PUBLIC
        ${imgui_SOURCE_DIR}/
        ${imgui_SOURCE_DIR}/backends/)
target_compile_definitions(
        imgui
        PUBLIC
        IMGUI_IMPL_VULKAN_USE_VOLK)
target_link_libraries(
        imgui
        PUBLIC
        SDL3::SDL3
        volk)
