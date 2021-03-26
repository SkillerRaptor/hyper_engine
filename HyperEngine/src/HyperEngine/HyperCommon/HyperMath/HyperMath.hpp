#pragma once

#include <cstdint>
#include <cfloat>
#include <cmath>

namespace HyperEngine
{
	static constexpr const float gf_PI = 3.14159265358979323846264338327950288419716939937510f;
	static constexpr const double g_PI = 3.14159265358979323846264338327950288419716939937510;
	
	static constexpr const float gf_PI2 = gf_PI * 2.0f;
	static constexpr const double g_PI2 = g_PI * 2.0;
	
	using std::abs;
	using std::floor;
	using std::ceil;
	using std::trunc;
	
	using std::sin;
	using std::cos;
	using std::tan;
	
	using std::atan;
	using std::atan2;
	
	using std::exp;
	using std::log;
	using std::pow;
	using std::sqrt;
	
	static constexpr float Degrees2Radians(float&& degrees)
	{
		return degrees * (gf_PI / 180.0f);
	}
	
	static constexpr double Degrees2Radians(double&& degrees)
	{
		return degrees * (g_PI / 180.0);
	}
	
	static constexpr float Radians2Degrees(float&& radians)
	{
		return radians * (180.0f / gf_PI);
	}
	
	static constexpr double Radians2Degrees(double&& radians)
	{
		return radians * (180.0 / g_PI);
	}
	
	#ifdef min
		#undef min
	#endif
	
	#ifdef max
		#undef max
	#endif
	
	template <typename T>
	inline constexpr T min(T a, T b) noexcept
	{
		static_assert(std::is_arithmetic<T>::value, "The value T is not an arithmetic type!");
		return b < a ? b : a;
	}
	
	template <typename T>
	inline constexpr T max(T a, T b) noexcept
	{
		static_assert(std::is_arithmetic<T>::value, "The value T is not an arithmetic type!");
		return a < b ? b : a;
	}
	
	template <typename T>
	inline constexpr T clamp(T value, T low, T high) noexcept
	{
		static_assert(std::is_arithmetic<T>::value, "The value T is not an arithmetic type!");
		return min(max(value, low), high);
	}
	
	template <typename T, typename R>
	inline constexpr T clamp_to(T value, T low, T high) noexcept
	{
		static_assert(std::is_arithmetic<T>::value, "The value T is not an arithmetic type!");
		static_assert(std::is_arithmetic<R>::value, "The value T is not an arithmetic type!");
		return static_cast<R>(clamp<T>(value, low, high));
	}
	
	template <typename T>
	inline constexpr T saturate(T value) noexcept
	{
		static_assert(std::is_arithmetic<T>::value, "The value T is not an arithmetic type!");
		return clamp(value, static_cast<T>(0.0f), static_cast<T>(1.0f));
	}
	
	template <class T, class U>
	inline constexpr T lerp(const T& a, const T& b, U t) noexcept
	{
		static_assert(std::is_arithmetic<T>::value, "The value T is not an arithmetic type!");
		return a + T((b - a) * t);
	}
	
	template <typename T>
	inline constexpr void limit(T& value, T min, T max) noexcept
	{
		static_assert(std::is_arithmetic<T>::value, "The value T is not an arithmetic type!");
		if (value < min)
		{
			value = min;
		}
		else if (value > max)
		{
			value = max;
		}
	}
	
	template <typename T>
	inline constexpr T asin(T value) noexcept
	{
		static_assert(std::is_arithmetic<T>::value, "The value T is not an arithmetic type!");
		return std::asin(clamp<T>(value, static_cast<T>(-1), static_cast<T>(1)));
	}
	
	template <typename T>
	inline constexpr T acos(T value) noexcept
	{
		static_assert(std::is_arithmetic<T>::value, "The value T is not an arithmetic type!");
		return std::acos(clamp<T>(value, static_cast<T>(-1), static_cast<T>(1)));
	}
	
	template <typename T>
	inline constexpr T sqrt(T value) noexcept
	{
		static_assert(std::is_arithmetic<T>::value, "The value T is not an arithmetic type!");
		return std::sqrt(value);
	}
	
	template <typename T>
	inline constexpr T rsqrt(T value) noexcept
	{
		static_assert(std::is_arithmetic<T>::value, "The value T is not an arithmetic type!");
		return static_cast<T>(1) / value;
	}
	
	template <typename T>
	inline constexpr T mod(T a, T b) noexcept
	{
		static_assert(std::is_arithmetic<T>::value, "The value T is not an arithmetic type!");
		return a - trunc(a / b) * b;
	}
	
	template <typename T>
	inline constexpr T frac(T value) noexcept
	{
		static_assert(std::is_arithmetic<T>::value, "The value T is not an arithmetic type!");
		return value - floor(value);
	}
	
	template <typename T>
	inline constexpr T wrap(T value, T low, T high) noexcept
	{
		static_assert(std::is_arithmetic<T>::value, "The value T is not an arithmetic type!");
		const T range{ high - low };
		return range == 0 ? static_cast<T>(0) : (value - floor((value - low) / range) * range);
	}
}