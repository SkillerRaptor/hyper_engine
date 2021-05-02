#-------------------------------------------------------------------------------------------
# Copyright (C) Hyperion. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

#-------------------------------------------------------------------------------------------
# Utility Functions
#-------------------------------------------------------------------------------------------
function(hyperengine_group_files FILES)
    foreach (item IN ITEMS ${FILES})
        get_filename_component(src_path "${item}" PATH)
        string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
        string(REPLACE ".." "\\" group_path "${group_path}")
        source_group("${group_path}" FILES "${item}")
    endforeach ()
endfunction()

function(hyperengine_set_output_directories TARGET)
    set_target_properties(${TARGET} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug/${TARGET}")
    set_target_properties(${TARGET} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin/RelWithDebInfo/${TARGET}/")
    set_target_properties(${TARGET} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin/MinSizeRel/${TARGET}")
    set_target_properties(${TARGET} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release/${TARGET}/")

    set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug/${TARGET}")
    set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin/RelWithDebInfo/${TARGET}/")
    set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin/MinSizeRel/${TARGET}")
    set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release/${TARGET}/")

    set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug/${TARGET}")
    set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin/RelWithDebInfo/${TARGET}/")
    set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin/MinSizeRel/${TARGET}")
    set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release/${TARGET}/")
endfunction()

function(hyperengine_set_cache_value KEY VALUE)
    set(${KEY} ${VALUE} CACHE BOOL "" FORCE)
endfunction()

function(hyperengine_post_copy_file TARGET FILE DESTINATION)
    add_custom_command(
            TARGET ${TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${FILE} ${DESTINATION})
endfunction()

function(hyperengine_post_copy_directory TARGET FOLDER DESTINATION)
    add_custom_command(
            TARGET ${TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${FOLDER} ${DESTINATION})
endfunction()

function(hyperengine_post_copy_assets TARGET)
    hyperengine_post_copy_directory(${TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/${TARGET}/assets)
endfunction()