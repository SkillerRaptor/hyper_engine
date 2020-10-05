#include "Hasher.hpp"

namespace Hyperion
{
	uint32_t Hasher::AsciiHasher(const std::string& string)
	{
		uint32_t hash = 5381;
		for (size_t i = 0; i < string.size(); ++i)
			hash = 33 * hash + (unsigned char) string[i];
		return hash;
	}

	uint32_t Hasher::PrimeHasher(const std::string& string)
	{
		uint32_t hash = 0x811c9dc5;
		uint32_t prime = 0x1000193;

		for (size_t i = 0; i < string.size(); ++i)
			hash = (hash ^ string[i]) * prime;

		return hash;
	}
}