#-------------------------------------------------------------------------------------------
# Copyright (c) 2026-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

set(SOURCES
        src/hyper_script/debug.cpp
        src/hyper_script/lexer.cpp
        src/hyper_script/parser.cpp
        src/hyper_script/token.cpp)

set(HEADERS
        include/hyper_script/ast.hpp
        include/hyper_script/debug.hpp
        include/hyper_script/lexer.hpp
        include/hyper_script/parser.hpp
        include/hyper_script/token.hpp)

he_add_library(hyper_script)
target_link_libraries(
        hyper_script
        PRIVATE
        hyper_core)