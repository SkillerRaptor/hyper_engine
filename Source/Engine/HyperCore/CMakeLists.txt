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
        src/HyperCore/CoreModule.cpp
        src/HyperCore/IModule.cpp
        src/HyperCore/JobSystem.cpp
        src/HyperCore/Logger.cpp
        src/HyperCore/ModuleManager.cpp)

set(HEADERS
        include/HyperCore/Align.hpp
        include/HyperCore/Assertion.hpp
        include/HyperCore/Bits.hpp
        include/HyperCore/Compilers.hpp
        include/HyperCore/CoreModule.hpp
        include/HyperCore/Formatter.hpp
        include/HyperCore/Hasher.hpp
        include/HyperCore/IModule.hpp
        include/HyperCore/JobSystem.hpp
        include/HyperCore/Logger.hpp
        include/HyperCore/Memory.hpp
        include/HyperCore/ModuleId.hpp
        include/HyperCore/ModuleManager.hpp
        include/HyperCore/Prerequisites.hpp
        include/HyperCore/SourceLocation.hpp
        include/HyperCore/Compilers/ClangSpecific.hpp
        include/HyperCore/Compilers/GCCSpecific.hpp
        include/HyperCore/Compilers/MSVCSpecific.hpp
        include/HyperCore/Containers/RingBuffer.hpp)

hyperengine_create_module(HyperCore)
target_link_libraries(
        HyperCore
        PUBLIC
        CONAN_PKG::fmt)
