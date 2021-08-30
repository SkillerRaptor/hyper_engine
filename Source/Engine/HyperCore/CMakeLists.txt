#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperCore
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperCore/Assertion.cpp
        src/HyperCore/Errors.cpp
        src/HyperCore/JobSystem.cpp
        src/HyperCore/Logger.cpp)

set(HEADERS
        include/HyperCore/Assertion.hpp
        include/HyperCore/Bits.hpp
        include/HyperCore/Compilers.hpp
        include/HyperCore/Errors.hpp
        include/HyperCore/Formatter.hpp
        include/HyperCore/Hasher.hpp
        include/HyperCore/JobSystem.hpp
        include/HyperCore/Logger.hpp
        include/HyperCore/Prerequisites.hpp
        include/HyperCore/Result.hpp
        include/HyperCore/SourceLocation.hpp
        include/HyperCore/TypeTraits.hpp
        include/HyperCore/Compilers/ClangSpecific.hpp
        include/HyperCore/Compilers/GCCSpecific.hpp
        include/HyperCore/Compilers/MSVCSpecific.hpp
        include/HyperCore/Memory/RingBuffer.hpp)

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")

hyperengine_define_module(HyperCore)
target_link_libraries(
        HyperCore
        PUBLIC
        CONAN_PKG::fmt)
