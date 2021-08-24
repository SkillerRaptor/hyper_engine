#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# Compiler Warnings
#-------------------------------------------------------------------------------------------
function(enable_settings project_name)
    option(ENABLE_RTTI "Enable Run-time type information" OFF)
    option(ENABLE_EXCEPTIONS "Enable exceptions" ON)

    set(MSVC_SETTINGS
            )

    set(CLANG_SETTINGS
            )

    set(GCC_SETTINGS
            )

    if (ENABLE_RTTI)
        set(MSVC_SETTINGS
                /GR)

        set(CLANG_SETTINGS
                -frtti)

        set(GCC_SETTINGS
                -frtti)
    else ()
        set(MSVC_SETTINGS
                /GR-)

        set(CLANG_SETTINGS
                -fno-rtti)

        set(GCC_SETTINGS
                -fno-rtti)
    endif ()

    if (ENABLE_EXCEPTIONS)
        set(MSVC_SETTINGS
                /EHscr)

        set(CLANG_SETTINGS
                -fexceptions)

        set(GCC_SETTINGS
                -fexceptions)
    else ()
        set(MSVC_SETTINGS
                /EHscr-)

        set(CLANG_SETTINGS
                -fno-exceptions)

        set(GCC_SETTINGS
                -fno-exceptions)
    endif ()

    if (MSVC)
        set(PROJECT_SETTINGS ${MSVC_SETTINGS})
    elseif (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        set(PROJECT_SETTINGS ${CLANG_SETTINGS})
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(PROJECT_SETTINGS ${GCC_SETTINGS})
    else ()
        message(AUTHOR_WARNING "No compiler settings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
    endif ()

    target_compile_options(${project_name} INTERFACE ${PROJECT_SETTINGS})
endfunction()
