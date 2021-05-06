#pragma once

#include <HyperCore/Log.hpp>

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
		T Generate(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max1())
		{
			if constexpr (min > max)
			{
				std::swap(min, max);
				HYPERENGINE_CORE_WARNING("HyperMath: The minimum is greater than the maximum in {0}!", __FUNCTION__);
			}
			
			if constexpr (std::is_integral_v<T>)
			{
				std::uniform_int_distribution<T> distribution{ min, max };
				return static_cast<T>(distribution(m_randomEngine));
			}
			
			if constexpr (std::is_floating_point_v<T>)
			{
				std::uniform_real_distribution<T> distribution{ min, max };
				return static_cast<T>(distribution(m_randomEngine));
			}
			
			if constexpr (std::is_same_v<T, bool>)
			{
				std::bernoulli_distribution distribution{ 0.5f };
				return static_cast<T>(distribution(m_randomEngine));
			}
			
			if constexpr(!std::is_integral_v<T> || !std::is_floating_point_v<T> || !std::is_same_v<T, bool>)
			{
				HYPERENGINE_CORE_ERROR("HyperMath: Invalid arithmetic type in {0}!", __FUNCTION__);
				return static_cast<T>(-1);
			}
		}
	
	private:
		std::mt19937 m_randomEngine;
	};
}