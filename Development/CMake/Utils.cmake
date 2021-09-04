#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# Utils
#-------------------------------------------------------------------------------------------
function(hyperengine_group_files files)
    foreach (item IN ITEMS ${files})
        get_filename_component(src_path "${item}" PATH)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
        string(REPLACE ".." "\\" group_path "${group_path}")
        source_group("${group_path}" FILES "${item}")
    endforeach ()
endfunction()

function(hyperengine_define_executable target)
    hyperengine_group_files("${SOURCES}")
    hyperengine_group_files("${HEADERS}")

    add_executable(${target} ${SOURCES} ${HEADERS})
    target_link_libraries(${target} PRIVATE ProjectOptions ProjectWarnings)
    target_include_directories(${target} PUBLIC include)
endfunction()

function(hyperengine_define_library target)
    hyperengine_group_files("${SOURCES}")
    hyperengine_group_files("${HEADERS}")

    add_library(${target} STATIC ${SOURCES} ${HEADERS})
    target_compile_features(${target}
            PUBLIC
            cxx_std_17)
    target_link_libraries(
            ${target}
            PRIVATE
            ProjectOptions
            ProjectWarnings)
    target_include_directories(
            ${target}
            PUBLIC
            include)
endfunction()

function(hyperengine_set_folder target folder)
    set_target_properties(
            ${target}
            PROPERTIES
            FOLDER ${folder})
endfunction()

function(hyperengine_set_startup target)
    if (NOT "${HYPERENGINE_ARCH}" STREQUAL "Windows")
        return()
    endif ()

    set_property(
            DIRECTORY ${CMAKE_SOURCE_DIR}
            PROPERTY
            VS_STARTUP_PROJECT ${target})
endfunction()

function(hyperengine_set_working_directory target)
    if (NOT "${HYPERENGINE_ARCH}" STREQUAL "Windows")
        return()
    endif ()

    set_target_properties(
            ${target}
            PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${target}>)
endfunction()

function(hyperengine_define_module target)
    hyperengine_define_library(${target})
    hyperengine_set_folder(${target} HyperModules)
endfunction()

function(hyperengine_make_directory directory)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${directory})
endfunction()

function(hyperengine_copy_file_post_build target file)
    add_custom_command(
            TARGET ${target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${file}" $<TARGET_FILE_DIR:${target}>)
endfunction()

function(hyperengine_copy_folder_post_build target source destination)
    add_custom_command(
            TARGET ${target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${source} ${destination})
endfunction()

function(hyperengine_move_target target destination)
    hyperengine_make_directory(Binary/${destination})
    add_custom_command(
            TARGET ${target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${CMAKE_BINARY_DIR}/Binary/${destination}/)
endfunction()

function(hyperengine_move_file target file destination)
    hyperengine_make_directory(Binary/${destination})
    add_custom_command(
            TARGET ${target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${file} ${CMAKE_BINARY_DIR}/Binary/${destination}/)
endfunction()

function(hyperengine_move_folder target folder destination)
    add_custom_command(
            TARGET ${target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${folder} ${CMAKE_BINARY_DIR}/Binary/${destination})
endfunction()
