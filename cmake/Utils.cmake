#-------------------------------------------------------------------------------------------
# Copyright (c) 2024-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

function(he_group_source source)
    foreach (item IN ITEMS ${source})
        get_filename_component(src_path "${item}" PATH)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
        string(REPLACE ".." "\\" group_path "${group_path}")
        source_group("${group_path}" FILES "${item}")
    endforeach ()
endfunction()

macro(he_define_executable target)
    he_group_source(${SOURCES})
    he_group_source(${HEADERS})

    add_executable(${target} ${SOURCES} ${HEADERS})
    target_include_directories(${target} PUBLIC include)
    target_link_libraries(${target} PRIVATE ProjectOptions ProjectWarnings)
    target_compile_options(${target} PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>")

    if (WIN32)
        target_compile_definitions(
                ${target}
                PRIVATE
                HE_PLATFORM_WINDOWS=1)

        target_compile_definitions(
                ${target}
                PRIVATE
                _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
                _CRT_SECURE_NO_WARNINGS
                NOMINMAX
                WIN32_LEAN_AND_MEAN)
    else ()
        target_compile_definitions(${target} PRIVATE HE_PLATFORM_LINUX=1)
    endif ()
endmacro()

macro(he_add_library target)
    he_group_source(${SOURCES})
    he_group_source(${HEADERS})

    add_library(${target} ${SOURCES} ${HEADERS})
    target_include_directories(${target} PUBLIC include)
    target_link_libraries(${target} PRIVATE ProjectOptions ProjectWarnings)
    target_compile_options(${target} PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>")

    if (WIN32)
        target_compile_definitions(
                ${target}
                PRIVATE
                HE_PLATFORM_WINDOWS=1)

        target_compile_definitions(
                ${target}
                PRIVATE
                _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
                _CRT_SECURE_NO_WARNINGS
                NOMINMAX
                WIN32_LEAN_AND_MEAN)
    else ()
        target_compile_definitions(${target} PRIVATE HE_PLATFORM_LINUX=1)
    endif ()
endmacro()

function(he_download_and_extract URL DESTINATION FOLDER_NAME)
    if (NOT EXISTS ${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip)
        message(STATUS " Downloading ${URL} and unpacking to ${DESTINATION}/${FOLDER_NAME}.")
        file(
                DOWNLOAD
                ${URL}
                ${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip)
    else ()
        message(STATUS "${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip already exists. No download required.")
    endif ()

    if (NOT EXISTS ${DESTINATION}/${FOLDER_NAME})
        message(STATUS "Extracting ${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip to ${DESTINATION}/${FOLDER_NAME}.")
        file(
                ARCHIVE_EXTRACT
                INPUT ${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip
                DESTINATION ${DESTINATION}/${FOLDER_NAME})
    else ()
        message(STATUS "${CMAKE_BINARY_DIR}/download/${FOLDER_NAME}.zip is already extracted to ${DESTINATION}/${FOLDER_NAME}.")
    endif ()
endfunction()

function(he_deploy_files SOURCE DESTINATION)
    set(DEPLOY_FILES_DESTINATION ${CMAKE_BINARY_DIR}/${DESTINATION})
    message(STATUS "Copying ${SOURCE} to ${DEPLOY_FILES_DESTINATION}")
    file(COPY ${SOURCE} DESTINATION ${DEPLOY_FILES_DESTINATION})
endfunction()

function(he_systemize target)
    get_target_property(${target}_include_dirs ${target} INTERFACE_INCLUDE_DIRECTORIES)
    set_target_properties(${target} PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${target}_include_dirs)
endfunction()