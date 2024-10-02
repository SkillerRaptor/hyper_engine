#-------------------------------------------------------------------------------------------
# Copyright (c) 2024, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

set(BUILD_SHARED_LIBS OFF)

#-------------------------------------------------------------------------------------------
# fmt
#-------------------------------------------------------------------------------------------
set(FMT_DOC OFF)
set(FMT_INSTALL OFF)
set(FMT_TEST OFF)
set(FMT_FUZZ OFF)
set(FMT_CUDA_TEST OFF)
set(FMT_OS ON)
set(FMT_MODULE OFF)
set(FMT_SYSTEM_HEADERS OFF)
set(FMT_UNICODE ON)
add_subdirectory(fmt)
set_target_properties(
        fmt
        PROPERTIES
        FOLDER "third_party")
