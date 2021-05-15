#pragma once

#include <HyperCore/Assert.hpp>
#include <HyperCore/Logger.hpp>

#include <random>
#include <type_traits>
#include <utility>

namespace HyperMath
{
	class Random
	{
	public:
		Random(uint32_t seed = std::random_device()());
		
		template <class T>
		T Generate(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
		{
			if constexpr (min > max)
			{
				std::swap(min, max);
				HyperCore::Logger::Warning("HyperMath: The minimum is bigger than the maximum!");
			}
			
			constexpr bool isIntegral = std::is_integral_v<T>;
			if constexpr (isIntegral)
			{
				std::uniform_int_distribution<T> distribution{ min, max };
				return static_cast<T>(distribution(m_randomEngine));
			}
			
			constexpr bool isFloatingPoint = std::is_floating_point_v<T>;
			if constexpr (isFloatingPoint)
			{
				std::uniform_real_distribution<T> distribution{ min, max };
				return static_cast<T>(distribution(m_randomEngine));
			}
			
			constexpr bool isBool = std::is_same_v<T, bool>;
			if constexpr (isBool)
			{
				std::bernoulli_distribution distribution{ 0.5f };
				return static_cast<T>(distribution(m_randomEngine));
			}
			
			constexpr bool isValid = isIntegral | isFloatingPoint | isBool;
			HYPERENGINE_ASSERT(isValid, "Failed to generate random number of invalid arithmetic type!");
		}
	
	private:
		std::mt19937 m_randomEngine;
	};
}
