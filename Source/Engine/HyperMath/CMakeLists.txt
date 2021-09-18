#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperMath
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperMath/MathModule.cpp)

set(HEADERS
        include/HyperMath/Common.hpp
        include/HyperMath/Compare.hpp
        include/HyperMath/Constants.hpp
        include/HyperMath/Matrix2.hpp
        include/HyperMath/Matrix3.hpp
        include/HyperMath/Matrix4.hpp
        include/HyperMath/Quaternion.hpp
        include/HyperMath/Trigonometric.hpp
        include/HyperMath/Types.hpp
        include/HyperMath/Vector2.hpp
        include/HyperMath/Vector3.hpp
        include/HyperMath/Vector4.hpp)

hyperengine_create_module(HyperMath)
target_link_libraries(
        HyperMath
        PUBLIC
        HyperCore)
