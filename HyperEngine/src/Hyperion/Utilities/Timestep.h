#pragma once

namespace Hyperion
{
	class Timestep
	{
	private:
		double m_Seconds;

	public:
		Timestep(double seconds = 0.0);

		double GetSeconds() const;
		double GetMilliseconds() const;

		operator double() const;
	};
}