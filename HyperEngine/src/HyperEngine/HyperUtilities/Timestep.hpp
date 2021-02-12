#pragma once

namespace HyperUtilities
{
	class Timestep
	{
	private:
		double m_DeltaTime;

	public:
		Timestep(double deltaTime = 0.0)
			: m_DeltaTime{ deltaTime }
		{
		}

		inline double GetSeconds() const
		{
			return m_DeltaTime;
		}

		inline double GetMilliseconds() const
		{
			return m_DeltaTime * 1000.0f;
		}

		inline operator double() const
		{
			return m_DeltaTime;
		}
	};
}
