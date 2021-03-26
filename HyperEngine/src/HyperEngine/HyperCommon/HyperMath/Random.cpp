#include "Random.hpp"

namespace HyperEngine
{
	std::mt19937 Random::s_randomEngine;
	
	void Random::Init(uint32_t seed)
	{
		s_randomEngine.seed(seed);
	}
}