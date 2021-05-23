#pragma once

#include <Core/Logger.hpp>

namespace Core
{
	template <typename T>
	class SparseMemoryPool
	{
	public:
		explicit SparseMemoryPool(uint32_t pool_size)
			: m_poolSize{ pool_size }
		{
			static_assert(sizeof(T) >= 4, "The template argument size is too small!");
			
			m_buffer = new T[m_poolSize];
			LinkList();
		}
		
		~SparseMemoryPool()
		{
			delete m_buffer;
		}
		
		T& Allocate(uint32_t& index)
		{
			if (index >= m_poolSize)
			{
				Logger::Fatal("Index out of range!");
				std::exit(1);
			}
			
			index = m_allocationCount;
			
			uint32_t* address{ reinterpret_cast<uint32_t*>(&m_buffer[m_nextAllocation]) };
			m_nextAllocation = *address;
			m_allocationCount++;
			m_buffer[index] = T{};
			
			return m_buffer[index];
		}
		
		void Deallocate(uint32_t index)
		{
			m_allocationCount--;
			
			uint32_t* address{ reinterpret_cast<uint32_t*>(&m_buffer[index]) };
			*address = m_nextAllocation;
			m_nextAllocation = index;
			
			m_buffer[index] = T{};
		}
		
		void Clear()
		{
			LinkList();
			m_nextAllocation = 0;
			m_allocationCount = 0;
		}
		
		T& At(uint32_t position)
		{
			if (position >= m_poolSize)
			{
				Logger::Fatal("Index out of range!");
				std::exit(1);
			}
			return m_buffer[position];
		}
		
		const T& At(uint32_t position) const
		{
			if (position >= m_poolSize)
			{
				Logger::Fatal("Index out of range!");
				std::exit(1);
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
		
		T* GetBuffer()
		{
			return m_buffer;
		}
		
		const T* GetBuffer() const
		{
			return m_buffer;
		}
		
		uint32_t GetPoolSize() const
		{
			return m_poolSize;
		}
		
		uint32_t GetAllocationCount() const
		{
			return m_allocationCount;
		}
	
	private:
		void LinkList()
		{
			for (uint32_t i{ 0 }; i < m_poolSize; i++)
			{
				uint32_t* address{ reinterpret_cast<uint32_t*>(&m_buffer[i]) };
				*address = i + 1;
			}
		}
	
	private:
		T* m_buffer{ nullptr };
		uint32_t m_poolSize{ 0 };
		uint32_t m_allocationCount{ 0 };
		uint32_t m_nextAllocation{ 0 };
	};
}
