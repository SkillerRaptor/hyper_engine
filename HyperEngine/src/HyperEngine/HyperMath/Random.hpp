#pragma once

#include <random>

namespace HyperMath
{
	class Random
	{
	private:
		std::mt19937 m_RandomEngine;

	public:
		Random(uint32_t seed = std::random_device()())
		{
			m_RandomEngine.seed(seed);
		}

		inline int16_t Int16()
		{
			std::uniform_int_distribution<int16_t> distribution;
			return static_cast<int16_t>(distribution(m_RandomEngine));
		}

		inline int16_t Int16(int16_t start, int16_t end)
		{
			std::uniform_int_distribution<int16_t> distribution{ start, end };
			return static_cast<int16_t>(distribution(m_RandomEngine));
		}

		inline int32_t Int32()
		{
			std::uniform_int_distribution<int32_t> distribution;
			return static_cast<int32_t>(distribution(m_RandomEngine));
		}

		inline int32_t Int32(int32_t start, int32_t end)
		{
			std::uniform_int_distribution<int32_t> distribution{ start, end };
			return static_cast<int32_t>(distribution(m_RandomEngine));
		}

		inline int64_t Int64()
		{
			std::uniform_int_distribution<int64_t> distribution;
			return static_cast<int64_t>(distribution(m_RandomEngine));
		}

		inline int64_t Int64(int64_t start, int64_t end)
		{
			std::uniform_int_distribution<int64_t> distribution{ start, end };
			return static_cast<int64_t>(distribution(m_RandomEngine));
		}

		inline uint16_t UInt16()
		{
			std::uniform_int_distribution<uint16_t> distribution;
			return static_cast<uint16_t>(distribution(m_RandomEngine));
		}

		inline uint16_t UInt16(uint16_t start, uint16_t end)
		{
			std::uniform_int_distribution<uint16_t> distribution{ start, end };
			return static_cast<uint16_t>(distribution(m_RandomEngine));
		}

		inline uint32_t UInt32()
		{
			std::uniform_int_distribution<uint32_t> distribution;
			return static_cast<uint32_t>(distribution(m_RandomEngine));
		}

		inline uint32_t UInt32(uint32_t start, uint32_t end)
		{
			std::uniform_int_distribution<uint32_t> distribution{ start, end };
			return static_cast<uint32_t>(distribution(m_RandomEngine));
		}

		inline uint64_t UInt64()
		{
			std::uniform_int_distribution<uint64_t> distribution;
			return static_cast<uint64_t>(distribution(m_RandomEngine));
		}

		inline uint64_t UInt64(uint64_t start, uint64_t end)
		{
			std::uniform_int_distribution<uint64_t> distribution{ start, end };
			return static_cast<uint64_t>(distribution(m_RandomEngine));
		}

		inline float Float()
		{
			std::uniform_real_distribution<float> distribution;
			return static_cast<float>(distribution(m_RandomEngine));
		}

		inline float Float(float start, float end)
		{
			std::uniform_real_distribution<float> distribution{ start, end };
			return static_cast<float>(distribution(m_RandomEngine));
		}

		inline double Double()
		{
			std::uniform_real_distribution<double> distribution;
			return static_cast<double>(distribution(m_RandomEngine));
		}

		inline double Double(double start, double end)
		{
			std::uniform_real_distribution<double> distribution{ start, end };
			return static_cast<double>(distribution(m_RandomEngine));
		}

		inline bool Bool()
		{
			return (Float() >= 0.5 ? true : false);
		}
	};
}
