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
        src/HyperCore/Logger.cpp
        src/HyperCore/Events/EventManager.cpp
        src/HyperCore/Events/KeyEvents.cpp
        src/HyperCore/Events/MouseEvents.cpp
        src/HyperCore/Events/WindowEvents.cpp)

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
        include/HyperCore/Events/EventManager.hpp
        include/HyperCore/Events/EventWrapper.hpp
        include/HyperCore/Events/IEvent.hpp
        include/HyperCore/Events/KeyEvents.hpp
        include/HyperCore/Events/MouseEvents.hpp
        include/HyperCore/Events/WindowEvents.hpp)

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")

hyperengine_define_module(HyperCore)
target_link_libraries(
        HyperCore
        PUBLIC
        CONAN_PKG::fmt)
