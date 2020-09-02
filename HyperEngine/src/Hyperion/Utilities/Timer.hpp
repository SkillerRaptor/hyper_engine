#pragma once

#include <chrono>

namespace Hyperion
{
	class Timer
	{
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_EndTimepoint;

		bool m_Stopped = false;
		
	public:
		Timer();
		~Timer();

		void Restart();
		void Stop();
		double Elapsed();

		operator double() const;
	};
}