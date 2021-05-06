#include <HyperMath/Random.hpp>

namespace HyperMath
{
	Random::Random(uint32_t seed)
	{
		m_randomEngine.seed(seed);
	}
}