#include "Random.h"

namespace Hyperion
{
	std::mt19937 Random::s_RandomEngine;

	void Random::Init()
	{
		s_RandomEngine.seed(std::random_device()());
	}

	int Random::Int(int start, int end)
	{
		std::uniform_int_distribution<std::mt19937::result_type> distribution(start, end);
		return (int) distribution(s_RandomEngine);
	}

	float Random::Float()
	{
		std::uniform_int_distribution<std::mt19937::result_type> distribution;
		return (float) distribution(s_RandomEngine) / (float) std::numeric_limits<uint32_t>::max();
	}

	float Random::Float(int start, int end)
	{
		return (float) (start + (Float() * ((float) end - start)));
	}

	double Random::Double()
	{
		std::uniform_int_distribution<std::mt19937::result_type> distribution;
		return (double) distribution(s_RandomEngine) / (double) std::numeric_limits<uint32_t>::max();
	}

	double Random::Double(int start, int end)
	{
		return (double) (start + (Double() * ((double) end - start)));
	}

	bool Random::Bool()
	{
		return Float() >= 0.5 ? true : false;
	}
}