#pragma once

#include "Core/Core.hpp"

namespace Hyperion
{
	namespace Structures
	{
		template<class Array, size_t S>
		class ArrayIterator
		{
		public:
			using ValueType = typename Array::ValueType;
			using PointerType = ValueType*;
			using ReferenceType = ValueType&;

		private:
			PointerType m_Ptr;

		public:
			ArrayIterator(PointerType ptr)
				: m_Ptr(ptr) {}

			ArrayIterator& operator++()
			{
				m_Ptr++;
				return *this;
			}

			ArrayIterator operator++(int)
			{
				ArrayIterator iterator = *this;
				++(*this);
				return iterator;
			}

			ArrayIterator& operator--()
			{
				m_Ptr--;
				return *this;
			}

			ArrayIterator operator--(int)
			{
				ArrayIterator iterator = *this;
				--(*this);
				return iterator;
			}

			ReferenceType operator[](int index)
			{
				return *(m_Ptr + index);
			}

			PointerType operator->()
			{
				return m_Ptr;
			}

			ReferenceType operator*()
			{
				return *m_Ptr;
			}

			bool operator==(const ArrayIterator& other) const
			{
				return m_Ptr == other.m_Ptr;
			}

			bool operator!=(const ArrayIterator& other) const
			{
				return !(*this == other);
			}
		};

		template<class T, size_t S>
		class Array
		{
		public:
			using ValueType = T;
			using Iterator = ArrayIterator<Array<T, S>, S>;

		private:
			T m_Data[S];

		public:
			Array() = default;
			Array(const Array& other) = default;

			T& operator[](size_t index)
			{
				HP_CORE_ASSERT(index < S, "Index is out of Bounds");
				return m_Data[index];
			}

			const T& operator[](size_t index) const
			{
				HP_CORE_ASSERT(index < S, "Index is out of Bounds");
				return m_Data[index];
			}

			T* Data() { return m_Data; }
			const T* Data() const { return m_Data; }

			constexpr size_t Size() const { return S; }

			Iterator begin()
			{
				return Iterator(m_Data);
			}

			Iterator end()
			{
				return Iterator(m_Data + S);
			}
		};
	}
}