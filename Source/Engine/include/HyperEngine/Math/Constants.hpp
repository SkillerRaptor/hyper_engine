/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>

namespace HyperEngine::Constants
{
	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T e_v = static_cast<T>(2.718281828459045235360287471352662498L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T log2e_v = static_cast<T>(1.442695040888963407359924681001892137L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T log10e_v = static_cast<T>(0.434294481903251827651128918916605082L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T pi_v = static_cast<T>(3.141592653589793238462643383279502884L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T inv_pi_v = static_cast<T>(0.318309886183790671537767526745028724L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T inv_sqrtpi_v = static_cast<T>(0.564189583547756286948079451560772586L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T ln2_v = static_cast<T>(0.693147180559945309417232121458176568L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T ln10_v = static_cast<T>(2.302585092994045684017991454684364208L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T sqrt2_v = static_cast<T>(1.414213562373095048801688724209698079L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T sqrt3_v = static_cast<T>(1.732050807568877293527446341505872367L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T inv_sqrt3_v = static_cast<T>(0.577350269189625764509148780501957456L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T egamma_v = static_cast<T>(0.577215664901532860606512090082402431L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T phi_v = static_cast<T>(1.618033988749894848204586834365638118L);

	constexpr float e = e_v<float>;
	constexpr float log2e = log2e_v<float>;
	constexpr float log10e = log10e_v<float>;
	constexpr float pi = pi_v<float>;
	constexpr float inv_pi = inv_pi_v<float>;
	constexpr float inv_sqrtpi = inv_sqrtpi_v<float>;
	constexpr float ln2 = ln2_v<float>;
	constexpr float ln10 = ln10_v<float>;
	constexpr float sqrt2 = sqrt2_v<float>;
	constexpr float sqrt3 = sqrt3_v<float>;
	constexpr float inv_sqrt3 = inv_sqrt3_v<float>;
	constexpr float egamma = egamma_v<float>;
	constexpr float phi = phi_v<float>;
} // namespace HyperEngine::Constants
