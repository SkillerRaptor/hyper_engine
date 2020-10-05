#pragma once

#include <iostream>

namespace Hyperion
{
	class Hasher
	{
	public:
		static uint32_t AsciiHasher(const std::string& string);
		static uint32_t PrimeHasher(const std::string& string);
	};
}