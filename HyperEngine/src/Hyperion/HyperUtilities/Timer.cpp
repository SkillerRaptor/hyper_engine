#include "Timer.hpp"

namespace Hyperion
{
	Timer::Timer()
		: m_StartTimepoint(std::chrono::high_resolution_clock::now())
	{
	}

	Timer::~Timer()
	{
		Stop();
	}

	void Timer::Restart()
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	void Timer::Stop()
	{
		m_Stopped = true;
		m_EndTimepoint = std::chrono::high_resolution_clock::now();
	}

	double Timer::Elapsed() const
	{
		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(m_EndTimepoint).time_since_epoch().count();

		if (!m_Stopped)
		{
			std::chrono::time_point<std::chrono::high_resolution_clock> endTimepoint = std::chrono::high_resolution_clock::now();
			end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
		}

		return (end - start) / 1000.0;
	}

	Timer::operator double() const
	{
		return Elapsed();
	}
}
