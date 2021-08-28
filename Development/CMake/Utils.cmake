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
    add_executable(${target} ${SOURCES} ${HEADERS})
    target_link_libraries(${target} PRIVATE ProjectOptions ProjectWarnings)
    target_include_directories(${target} PUBLIC include)
endfunction()

function(hyperengine_define_library target)
    add_library(${target} STATIC ${SOURCES} ${HEADERS})
    target_compile_features(${target} PUBLIC cxx_std_17)
    target_link_libraries(${target} PRIVATE ProjectOptions ProjectWarnings)
    target_include_directories(${target} PUBLIC include)
endfunction()

function(hyperengine_define_module target)
    hyperengine_define_library(${target})
    set_target_properties(${target} PROPERTIES FOLDER HyperModules)
endfunction()
