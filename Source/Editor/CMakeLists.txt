#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperEditor
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/main.cpp
        src/HyperEditor/Editor.cpp)

set(HEADERS
        include/HyperEditor/Editor.hpp)

hyperengine_define_executable(HyperEditor)
hyperengine_set_startup(HyperEditor)
hyperengine_set_working_directory(HyperEditor)
target_link_libraries(
        HyperEditor
        PRIVATE
        HyperEngine)

hyperengine_copy_folder_post_build(HyperEditor ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:HyperEditor>/assets)

hyperengine_move_target(HyperEditor HyperEditor)
hyperengine_move_folder(HyperEditor ${CMAKE_CURRENT_SOURCE_DIR}/assets HyperEditor/assets)
