/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>

namespace HyperMath::Numbers
{
	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T g_e_v = static_cast<T>(2.718281828459045235360287471352662498L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T g_log2e_v = static_cast<T>(1.442695040888963407359924681001892137L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T g_log10e_v = static_cast<T>(0.434294481903251827651128918916605082L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T g_pi_v = static_cast<T>(3.141592653589793238462643383279502884L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T g_inv_pi_v = static_cast<T>(0.318309886183790671537767526745028724L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T g_inv_sqrtpi_v = static_cast<T>(0.564189583547756286948079451560772586L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T g_ln2_v = static_cast<T>(0.693147180559945309417232121458176568L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T g_ln10_v = static_cast<T>(2.302585092994045684017991454684364208L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T g_sqrt2_v = static_cast<T>(1.414213562373095048801688724209698079L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T g_sqrt3_v = static_cast<T>(1.732050807568877293527446341505872367L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T g_inv_sqrt3_v = static_cast<T>(0.577350269189625764509148780501957456L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T g_egamma_v = static_cast<T>(0.577215664901532860606512090082402431L);

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	constexpr T g_phi_v = static_cast<T>(1.618033988749894848204586834365638118L);

	constexpr float g_e = g_e_v<float>;
	constexpr float g_log2e = g_log2e_v<float>;
	constexpr float g_log10e = g_log10e_v<float>;
	constexpr float g_pi = g_pi_v<float>;
	constexpr float g_inv_pi = g_inv_pi_v<float>;
	constexpr float g_inv_sqrtpi = g_inv_sqrtpi_v<float>;
	constexpr float g_ln2 = g_ln2_v<float>;
	constexpr float g_ln10 = g_ln10_v<float>;
	constexpr float g_sqrt2 = g_sqrt2_v<float>;
	constexpr float g_sqrt3 = g_sqrt3_v<float>;
	constexpr float g_inv_sqrt3 = g_inv_sqrt3_v<float>;
	constexpr float g_egamma = g_egamma_v<float>;
	constexpr float g_phi = g_phi_v<float>;
} // namespace HyperMath::Numbers
