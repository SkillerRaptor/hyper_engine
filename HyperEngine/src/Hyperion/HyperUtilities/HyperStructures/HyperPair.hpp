#pragma once

namespace Hyperion
{
	template <class T1, class T2>
	class HyperPair
	{
	public:
		T1 First;
		T2 Second;

	public:
		HyperPair() = default;
		HyperPair(const T1& first, const T2& second)
			: First(first), Second(second) {}

		HyperPair(const HyperPair<T1, T2>& other)
		{
			if (this != &other)
			{
				First = other.First;
				Second = other.Second;
			}
		}

		HyperPair& operator=(const HyperPair<T1, T2>& other)
		{
			if (this != &other)
			{
				First = other.First;
				Second = other.Second;
			}
			return *this;
		}

		void Swap(HyperPair<T1, T2>& other)
		{
			T1 tempOne = other.First;
			other.First = First;
			First = tempOne;

			T2 tempTwo = other.Second;
			other.Second = Second;
			Second = tempTwo;
		}
	};

	template<class T1, class T2>
	HyperPair<T1, T2> MakePair(T1 first, T2 second)
	{
		return HyperPair<T1, T2>(first, second);
	}
}
