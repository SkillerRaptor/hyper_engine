#pragma once

#include <core/logger.hpp>

namespace core
{
	template <typename T>
	class sparse_memory_pool
	{
	public:
		explicit sparse_memory_pool(uint32_t pool_size)
			: m_pool_size{ pool_size }
		{
			static_assert(sizeof(T) >= 4, "The template argument size is too small!");
			
			m_buffer = new T[m_pool_size];
			link_list();
		}
		
		~sparse_memory_pool()
		{
			delete[] m_buffer;
		}
		
		T& allocate(uint32_t& index)
		{
			if (index >= m_pool_size)
			{
				logger::fatal("Index out of range!");
			}
			
			index = m_allocation_count;
			
			uint32_t* address{ reinterpret_cast<uint32_t*>(&m_buffer[m_next_allocation]) };
			m_next_allocation = *address;
			m_allocation_count++;
			m_buffer[index] = T{};
			
			return m_buffer[index];
		}
		
		void deallocate(uint32_t index)
		{
			m_allocation_count--;
			
			uint32_t* address{ reinterpret_cast<uint32_t*>(&m_buffer[index]) };
			*address = m_next_allocation;
			m_next_allocation = index;
			
			m_buffer[index] = T{};
		}
		
		void clear()
		{
			link_list();
			m_next_allocation = 0;
			m_allocation_count = 0;
		}
		
		T& at(uint32_t position)
		{
			if (position >= m_pool_size)
			{
				logger::fatal("Index out of range!");
			}
			return m_buffer[position];
		}
		
		const T& at(uint32_t position) const
		{
			if (position >= m_pool_size)
			{
				logger::fatal("Index out of range!");
			}
			return m_buffer[position];
		}
		
		T& operator[](uint32_t position)
		{
			return m_buffer[position];
		}
		
		const T& operator[](uint32_t position) const
		{
			return m_buffer[position];
		}
		
		T* data()
		{
			return m_buffer;
		}
		
		const T* data() const
		{
			return m_buffer;
		}
		
		uint32_t size() const
		{
			return m_pool_size;
		}
		
		uint32_t count() const
		{
			return m_allocation_count;
		}
	
	private:
		void link_list()
		{
			for (uint32_t i{ 0 }; i < m_pool_size; i++)
			{
				uint32_t* address{ reinterpret_cast<uint32_t*>(&m_buffer[i]) };
				*address = i + 1;
			}
		}
	
	private:
		T* m_buffer{ nullptr };
		uint32_t m_pool_size{ 0 };
		uint32_t m_allocation_count{ 0 };
		uint32_t m_next_allocation{ 0 };
	};
}
