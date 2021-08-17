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
        include/HyperMath/Common.hpp
        include/HyperMath/Compare.hpp
        include/HyperMath/Constants.hpp
        include/HyperMath/Matrix2x2.hpp
        include/HyperMath/Matrix3x3.hpp
        include/HyperMath/Matrix4x4.hpp
        include/HyperMath/Quaternion.hpp
        include/HyperMath/Trigonometric.hpp
        include/HyperMath/Types.hpp
        include/HyperMath/Vector2.hpp
        include/HyperMath/Vector3.hpp
        include/HyperMath/Vector4.hpp)

hyperengine_group_source("${SOURCES}")
hyperengine_group_source("${HEADERS}")

hyperengine_define_module(HyperMath)
target_link_libraries(
        HyperMath
        PUBLIC
        HyperCore)
