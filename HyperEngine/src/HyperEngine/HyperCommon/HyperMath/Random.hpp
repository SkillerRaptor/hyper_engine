#pragma once

#include <random>

namespace HyperEngine
{
	class Random
	{
	public:
		static void Init(uint32_t seed = std::random_device()());
		
		static inline int16_t Int16()
		{
			const std::uniform_int_distribution<int16_t> distribution;
			return static_cast<int16_t>(distribution(s_randomEngine));
		}
		
		static inline int16_t Int16(int16_t start, int16_t end)
		{
			const std::uniform_int_distribution<int16_t> distribution{ start, end };
			return static_cast<int16_t>(distribution(s_randomEngine));
		}
		
		static inline int32_t Int32()
		{
			const std::uniform_int_distribution<int32_t> distribution;
			return static_cast<int32_t>(distribution(s_randomEngine));
		}
		
		static inline int32_t Int32(int32_t start, int32_t end)
		{
			const std::uniform_int_distribution<int32_t> distribution{ start, end };
			return static_cast<int32_t>(distribution(s_randomEngine));
		}
		
		static inline int64_t Int64()
		{
			const std::uniform_int_distribution<int64_t> distribution;
			return static_cast<int64_t>(distribution(s_randomEngine));
		}
		
		static inline int64_t Int64(int64_t start, int64_t end)
		{
			const std::uniform_int_distribution<int64_t> distribution{ start, end };
			return static_cast<int64_t>(distribution(s_randomEngine));
		}
		
		static inline uint16_t UInt16()
		{
			const std::uniform_int_distribution<uint16_t> distribution;
			return static_cast<uint16_t>(distribution(s_randomEngine));
		}
		
		static inline uint16_t UInt16(uint16_t start, uint16_t end)
		{
			const std::uniform_int_distribution<uint16_t> distribution{ start, end };
			return static_cast<uint16_t>(distribution(s_randomEngine));
		}
		
		static inline uint32_t UInt32()
		{
			const std::uniform_int_distribution<uint32_t> distribution;
			return static_cast<uint32_t>(distribution(s_randomEngine));
		}
		
		static inline uint32_t UInt32(uint32_t start, uint32_t end)
		{
			const std::uniform_int_distribution<uint32_t> distribution{ start, end };
			return static_cast<uint32_t>(distribution(s_randomEngine));
		}
		
		static inline uint64_t UInt64()
		{
			const std::uniform_int_distribution<uint64_t> distribution;
			return static_cast<uint64_t>(distribution(s_randomEngine));
		}
		
		static inline uint64_t UInt64(uint64_t start, uint64_t end)
		{
			const std::uniform_int_distribution<uint64_t> distribution{ start, end };
			return static_cast<uint64_t>(distribution(s_randomEngine));
		}
		
		static inline float Float()
		{
			const std::uniform_real_distribution<float> distribution;
			return static_cast<float>(distribution(s_randomEngine));
		}
		
		static inline float Float(float start, float end)
		{
			const std::uniform_real_distribution<float> distribution{ start, end };
			return static_cast<float>(distribution(s_randomEngine));
		}
		
		static inline double Double()
		{
			const std::uniform_real_distribution<double> distribution;
			return static_cast<double>(distribution(s_randomEngine));
		}
		
		static inline double Double(double start, double end)
		{
			const std::uniform_real_distribution<double> distribution{ start, end };
			return static_cast<double>(distribution(s_randomEngine));
		}
		
		static inline bool Bool()
		{
			return Float() >= 0.5;
		}
	
	private:
		static std::mt19937 s_randomEngine;
	};
}