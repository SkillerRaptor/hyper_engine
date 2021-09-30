#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# Compiler Settings
#-------------------------------------------------------------------------------------------
function(enable_settings project_name)
    set(MSVC_LINK_SETTINGS
            /ignore:4099)

    set(CLANG_LINK_SETTINGS
            )

    set(GCC_LINK_SETTINGS
            )

    if (MSVC)
        set(PROJECT_LINK_SETTINGS ${MSVC_LINK_SETTINGS})
    elseif (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        set(PROJECT_LINK_SETTINGS ${CLANG_LINK_SETTINGS})
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(PROJECT_LINK_SETTINGS ${GCC_LINK_SETTINGS})
    else ()
        message(AUTHOR_WARNING "No compiler settings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
    endif ()

    target_link_options(
            ${project_name}
            INTERFACE
            ${PROJECT_LINK_SETTINGS})
endfunction()
