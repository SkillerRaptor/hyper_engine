#include "Timestep.h"

namespace Hyperion {

	Timestep::Timestep(double seconds)
		: m_Seconds(seconds)
	{
	}

	double Timestep::GetSeconds() const
	{
		return m_Seconds;
	}

	double Timestep::GetMilliseconds() const
	{
		return m_Seconds * 1000;
	}

	Timestep::operator double() const
	{
		return m_Seconds;
	}
}