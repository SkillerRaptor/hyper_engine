/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Core/Assertion.hpp"
#include "HyperEngine/Core/Prerequisites.hpp"

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace HyperEngine
{
	class CHasher
	{
	public:
		HYPERENGINE_MAKE_SINGLETON(CHasher);
		
	private:
		static constexpr uint32_t s_fnv_basis_32{ 0x811C9DC5 };
		static constexpr uint32_t s_fnv_prime_32{ 0x01000193 };
		static constexpr uint64_t s_fnv_basis_64{ 0xCBF29CE484222325 };
		static constexpr uint64_t s_fnv_prime_64{ 0x00000100000001B3 };

	public:
		template <class T>
		static constexpr auto hash_fnv_32(const T& string) -> uint32_t
		{
			uint32_t result = s_fnv_basis_32;

			for (const char c : string)
			{
				result *= s_fnv_prime_32;
				result ^= static_cast<uint32_t>(c);
			}

			return result;
		}

		template <size_t N>
		static constexpr auto hash_fnv_32(const char (&string)[N]) -> uint32_t
		{
			return hash_fnv_32(std::string_view(string));
		}

		template <class T>
		static constexpr auto hash_fnv_64(const T& string) -> uint64_t
		{
			uint64_t result = s_fnv_basis_64;

			for (const char c : string)
			{
				result *= s_fnv_prime_64;
				result ^= static_cast<uint64_t>(c);
			}

			return result;
		}

		template <size_t N>
		static constexpr auto hash_fnv_64(const char (&string)[N]) -> uint64_t
		{
			return hash_fnv_64(std::string_view(string));
		}
	};
} // namespace HyperEngine
