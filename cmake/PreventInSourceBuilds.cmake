#-------------------------------------------------------------------------------------------
# Copyright (c) 2026-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

function(he_assure_out_of_source_builds)
    file(REAL_PATH "${CMAKE_SOURCE_DIR}" srcdir)
    file(REAL_PATH "${CMAKE_BINARY_DIR}" bindir)

    if ("${srcdir}" STREQUAL "${bindir}")
        message("######################################################")
        message("Warning: in-source builds are disabled")
        message("Please create a separate build directory and run cmake from there")
        message("######################################################")
        message(FATAL_ERROR "Quitting configuration")
    endif ()
endfunction()

he_assure_out_of_source_builds()
