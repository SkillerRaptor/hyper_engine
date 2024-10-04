#-------------------------------------------------------------------------------------------
# Copyright (c) 2024, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

macro(hyperengine_group_source SOURCE)
    foreach (item IN ITEMS ${SOURCE})
        get_filename_component(src_path "${item}" PATH)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
        string(REPLACE ".." "\\" group_path "${group_path}")
        source_group("${group_path}" FILES "${item}")
    endforeach ()
endmacro()

macro(hyperengine_define_executable target)
    hyperengine_group_source(${SOURCES})
    if (HEADERS)
        hyperengine_group_source(${HEADERS})
    endif ()

    add_executable(${target} ${SOURCES} ${HEADERS})
    target_link_libraries(${target} PRIVATE ProjectOptions ProjectWarnings)
    target_include_directories(${target} PUBLIC include)

    if (WIN32)
        target_compile_definitions(${target} PUBLIC HE_WINDOWS=1)
    else ()
        target_compile_definitions(${target} PUBLIC HE_LINUX=1)
    endif ()
endmacro()

macro(hyperengine_define_library target)
    hyperengine_group_source(${SOURCES})
    if (HEADERS)
        hyperengine_group_source(${HEADERS})
    endif ()

    add_library(${target} STATIC ${SOURCES} ${HEADERS})
    target_link_libraries(${target} PRIVATE ProjectOptions ProjectWarnings)
    target_include_directories(${target} PUBLIC include)

    if (WIN32)
        target_compile_definitions(${target} PUBLIC HE_WINDOWS=1)
    else ()
        target_compile_definitions(${target} PUBLIC HE_LINUX=1)
    endif ()
endmacro()
