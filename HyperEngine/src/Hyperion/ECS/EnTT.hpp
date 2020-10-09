#pragma once

namespace Hyperion
{
	class EnTT
	{
	private:
		uint32_t m_Id;

	public:
		EnTT(uint32_t id = 0)
			: m_Id(id) {}

		operator uint32_t() const { return m_Id; }

		bool operator==(const EnTT& other) const
		{
			return m_Id == other.m_Id;
		}

		bool operator!=(const EnTT& other) const
		{
			return !(*this == other);
		}
	};
}
