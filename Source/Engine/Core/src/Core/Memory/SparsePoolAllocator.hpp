#pragma once

#include <Core/Assertion.hpp>
#include <Core/Logger.hpp>

namespace Core
{
	template <typename T>
	class SparsePoolAllocator
	{
	public:
		using ValueType = T;
		using Reference = T&;
		using ConstReference = const T&;
		using Pointer = T*;
		using ConstPointer = const T*;
		using SizeType = size_t;
		using DifferenceType = ptrdiff_t;
	
	public:
		explicit SparsePoolAllocator(SizeType size)
			: m_size{ size }
		{
			m_data = new ValueType[m_size];
			
			for (SizeType i = 0; i < m_size; ++i)
			{
				DifferenceType* address = reinterpret_cast<DifferenceType*>(&m_data[i]);
				*address = static_cast<DifferenceType>(i) + 1;
			}
		}
		
		~SparsePoolAllocator()
		{
			delete[] m_data;
		}
		
		Reference allocate(DifferenceType& position)
		{
			HE_VERIFY(position < m_size);
			
			position = static_cast<DifferenceType>(m_allocation_count);
			
			DifferenceType* address = reinterpret_cast<DifferenceType*>(&m_data[position]);
			m_next_allocation = *address;
			
			m_data[position] = T();
			m_allocation_count++;
			
			return m_data[position];
		}
		
		void deallocate(DifferenceType position)
		{
			HE_VERIFY(position < m_size);
			
			DifferenceType* address = reinterpret_cast<DifferenceType*>(&m_data[position]);
			*address = m_next_allocation;
			
			m_next_allocation = position;
			
			m_data[position] = T();
			m_allocation_count--;
		}
		
		void clear()
		{
			for (SizeType i = 0; i < m_size; ++i)
			{
				DifferenceType* address = reinterpret_cast<DifferenceType*>(&m_data[i]);
				*address = static_cast<DifferenceType>(i) + 1;
			}
			
			m_allocation_count = 0;
			m_next_allocation = 0;
		}
		
		Reference at(SizeType position)
		{
			HE_VERIFY(position < m_size);
			return m_data[position];
		}
		
		ConstReference at(SizeType position) const
		{
			HE_VERIFY(position < m_size);
			return m_data[position];
		}
		
		Reference operator[](SizeType position)
		{
			return m_data[position];
		}
		
		ConstReference operator[](SizeType position) const
		{
			return m_data[position];
		}
		
		Pointer data()
		{
			return m_data;
		}
		
		ConstPointer data() const
		{
			return m_data;
		}
		
		SizeType size() const
		{
			return m_allocation_count;
		}
		
		SizeType max_size() const
		{
			return m_size;
		}
	
	private:
		Pointer m_data{ nullptr };
		SizeType m_size{ 0 };
		SizeType m_allocation_count{ 0 };
		DifferenceType m_next_allocation{ 0 };
	};
}
