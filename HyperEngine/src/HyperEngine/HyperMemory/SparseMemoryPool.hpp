#pragma once

#include <cstdint>
#include <cstddef>

#include "HyperCore/Core.hpp"
#include "HyperUtilities/NonCopyable.hpp"

namespace HyperMemory
{
	template <typename T>
	class SparseMemoryPool : public HyperUtilities::NonCopyable
	{
	private:
		T* m_pMemory{ nullptr };
		uint32_t m_PoolSize{ 0 };
		uint32_t m_AllocationCount{ 0 };
		uint32_t m_NextAllocation{ 0 };
	
	public:
		explicit SparseMemoryPool(uint32_t poolSize)
			: m_PoolSize{ poolSize }
		{
			HP_ASSERT(sizeof(T) >= 4);
		
			m_pMemory = new T[poolSize];
			
			SetupLinkedList();
		}
		
		~SparseMemoryPool()
		{
			delete m_pMemory;
		}
		
		T& Allocate(uint32_t& index)
		{
			HP_ASSERT(index < m_PoolSize, "Index out of range!");
			
			index = m_AllocationCount;
			m_NextAllocation = *(reinterpret_cast<uint32_t*>(&m_pMemory[m_NextAllocation]));
			
			m_AllocationCount++;
			
			m_pMemory[index] = T{};
			
			return m_pMemory[index];
		}
		
		void Deallocate(uint32_t index)
		{
			m_AllocationCount--;
			
			*(reinterpret_cast<uint32_t*>(&m_pMemory[index])) = m_NextAllocation;
			m_NextAllocation = index;
			
			m_pMemory[index] = T{};
		}
		
		void Clear()
		{
			SetupLinkedList();
			m_NextAllocation = 0;
			m_AllocationCount = 0;
		}
		
		inline T& operator[](uint32_t index)
		{
			HP_ASSERT(index < m_PoolSize, "Index out of range!");
			return m_pMemory[index];
		}
		
		inline const T& operator[](uint32_t index) const
		{
			HP_ASSERT(index < m_PoolSize, "Index out of range!");
			return m_pMemory[index];
		}
		
		inline uint32_t GetPoolSize() const
		{
			return m_PoolSize;
		}
		
		inline uint32_t GetAllocationCount() const
		{
			return m_AllocationCount;
		}
	
	private:
		inline void SetupLinkedList()
		{
			for(uint32_t i = 0; i < m_PoolSize; i++)
				*(reinterpret_cast<uint32_t*>(&m_pMemory[i])) = i + 1;
		}
	};
}
