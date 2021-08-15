#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperCore
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperCore/Logger.cpp
        src/HyperCore/Events/Event.cpp
        src/HyperCore/Events/EventManager.cpp)

set(HEADERS
        include/HyperCore/Assertion.hpp
        include/HyperCore/Bits.hpp
        include/HyperCore/Compilers.hpp
        include/HyperCore/Errors.hpp
        include/HyperCore/Formatter.hpp
        include/HyperCore/Hasher.hpp
        include/HyperCore/Logger.hpp
        include/HyperCore/Prerequisites.hpp
        include/HyperCore/Result.hpp
        include/HyperCore/SourceLocation.hpp
        include/HyperCore/Compilers/ClangSpecific.hpp
        include/HyperCore/Compilers/GCCSpecific.hpp
        include/HyperCore/Compilers/MSVCSpecific.hpp
        include/HyperCore/Events/Event.hpp
        include/HyperCore/Events/EventManager.hpp
        include/HyperCore/Events/EventWrapper.hpp
        include/HyperCore/Events/KeyEvents.hpp
        include/HyperCore/Events/MouseEvents.hpp
        include/HyperCore/Events/WindowEvents.hpp
        include/HyperCore/Memory/RingBuffer.hpp
        include/HyperCore/Memory/SparsePool.hpp)

hyperengine_group_source(${SOURCES})
hyperengine_group_source(${HEADERS})

hyperengine_define_module(HyperCore)
target_link_libraries(
        HyperCore
        PUBLIC
        CONAN_PKG::fmt)
