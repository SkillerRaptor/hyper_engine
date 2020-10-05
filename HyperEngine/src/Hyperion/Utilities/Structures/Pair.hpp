#pragma once

namespace Hyperion
{
	template <class T1, class T2>
	class Pair
	{
	public:
		T1 First;
		T2 Second;

	public:
		Pair() = default;
		Pair(const T1& first, const T2& second)
			: First(first), Second(second) {}

		Pair(const Pair<T1, T2>& other)
		{
			if (this != &other)
			{
				First = other.First;
				Second = other.Second;
			}
		}

		Pair& operator=(const Pair<T1, T2>& other)
		{
			if (this != &other)
			{
				First = other.First;
				Second = other.Second;
			}
			return *this;
		}

		void Swap(Pair<T1, T2>& other)
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
	Pair<T1, T2> MakePair(T1 first, T2 second)
	{
		return Pair<T1, T2>(first, second);
	}
}