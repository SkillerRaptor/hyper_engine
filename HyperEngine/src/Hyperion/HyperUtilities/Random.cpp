#include "Random.hpp"

namespace Hyperion
{
	std::mt19937 Random::s_RandomEngine; void Random::Init()
	{
		s_RandomEngine.seed(std::random_device()());
	}

	int16_t Random::Int16()
	{
		std::uniform_int_distribution<int16_t> distribution;
		return static_cast<int16_t>(distribution(s_RandomEngine));
	}

	int16_t Random::Int16(int16_t start, int16_t end)
	{
		std::uniform_int_distribution<int16_t> distribution(start, end);
		return static_cast<int16_t>(distribution(s_RandomEngine));
	}

	int32_t Random::Int32()
	{
		std::uniform_int_distribution<int32_t> distribution;
		return static_cast<int32_t>(distribution(s_RandomEngine));
	}

	int32_t Random::Int32(int32_t start, int32_t end)
	{
		std::uniform_int_distribution<int32_t> distribution(start, end);
		return static_cast<int32_t>(distribution(s_RandomEngine));
	}

	int64_t Random::Int64()
	{
		std::uniform_int_distribution<int64_t> distribution;
		return static_cast<int64_t>(distribution(s_RandomEngine));
	}

	int64_t Random::Int64(int64_t start, int64_t end)
	{
		std::uniform_int_distribution<int64_t> distribution(start, end);
		return static_cast<int64_t>(distribution(s_RandomEngine));
	}

	uint16_t Random::UInt16()
	{
		std::uniform_int_distribution<uint16_t> distribution;
		return static_cast<uint16_t>(distribution(s_RandomEngine));
	}

	uint16_t Random::UInt16(uint16_t start, uint16_t end)
	{
		std::uniform_int_distribution<uint16_t> distribution(start, end);
		return static_cast<uint16_t>(distribution(s_RandomEngine));
	}

	uint32_t Random::UInt32()
	{
		std::uniform_int_distribution<uint32_t> distribution;
		return static_cast<uint32_t>(distribution(s_RandomEngine));
	}

	uint32_t Random::UInt32(uint32_t start, uint32_t end)
	{
		std::uniform_int_distribution<uint32_t> distribution(start, end);
		return static_cast<uint32_t>(distribution(s_RandomEngine));
	}

	uint64_t Random::UInt64()
	{
		std::uniform_int_distribution<uint64_t> distribution;
		return static_cast<uint64_t>(distribution(s_RandomEngine));
	}

	uint64_t Random::UInt64(uint64_t start, uint64_t end)
	{
		std::uniform_int_distribution<uint64_t> distribution(start, end);
		return static_cast<uint64_t>(distribution(s_RandomEngine));
	}

	float Random::Float()
	{
		std::uniform_real_distribution<float> distribution;
		return static_cast<float>(distribution(s_RandomEngine));
	}

	float Random::Float(float start, float end)
	{
		std::uniform_real_distribution<float> distribution(start, end);
		return static_cast<float>(distribution(s_RandomEngine));
	}

	double Random::Double()
	{
		std::uniform_real_distribution<double> distribution;
		return static_cast<double>(distribution(s_RandomEngine));
	}

	double Random::Double(double start, double end)
	{
		std::uniform_real_distribution<double> distribution(start, end);
		return static_cast<double>(distribution(s_RandomEngine));
	}

	bool Random::Bool()
	{
		return (Float() >= 0.5 ? true : false);
	}
}
