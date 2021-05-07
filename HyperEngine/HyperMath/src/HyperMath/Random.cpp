#include <HyperMath/Random.hpp>

namespace HyperEngine
{
	Random::Random(uint32_t seed)
	{
		m_randomEngine.seed(seed);
	}
}
