#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperMath
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperMath/HyperMath.cpp)

set(HEADERS
        include/HyperMath/Matrix2x2.hpp
        include/HyperMath/Matrix3x3.hpp
        include/HyperMath/Matrix4x4.hpp
        include/HyperMath/Quaternion.hpp
        include/HyperMath/Vector2.hpp
        include/HyperMath/Vector3.hpp
        include/HyperMath/Vector4.hpp)

group_source()

define_module(HyperMath)
target_link_libraries(HyperMath PUBLIC HyperCore)
