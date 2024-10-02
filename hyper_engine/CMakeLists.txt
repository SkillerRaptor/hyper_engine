#-------------------------------------------------------------------------------------------
# Copyright (c) 2024, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------
set(SOURCES
    src/main.cpp)

hyperengine_define_executable(hyper_engine)
target_link_libraries(
    hyper_engine
    PRIVATE
    hyper_core
    hyper_platform
    argparse)
